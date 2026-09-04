#!/bin/sh

set -eu

WEB_ROOT=/var/www/html
SOURCE_ROOT=/usr/src/wordpress
WP_CONFIG=$WEB_ROOT/wp-config.php
DB_HOST=mariadb
DB_PORT=3306
DB_SECRET=/run/secrets/db_password
REDIS_HOST=redis
REDIS_PORT=6379
REDIS_SECRET=/run/secrets/redis_password
REDIS_CONFIG_TOOL=/usr/local/bin/configure-wp-redis.php
REDIS_PLUGIN=redis-cache
WP_ADMIN_SECRET=/run/secrets/wp_admin_password
WP_USER_SECRET=/run/secrets/wp_user_password
RUNTIME_DIR=/run/wordpress-entrypoint
DB_CLIENT_CONFIG=$RUNTIME_DIR/.mariadb.cnf
CONFIG_TMP=

fail() {
	echo "$1" >&2
	exit 1
}

read_secret() {
	secret_path=$1
	if [ ! -f "$secret_path" ] || [ ! -r "$secret_path" ]; then
		echo "Missing or unreadable secret: $secret_path" >&2
		return 1
	fi

	if ! secret_value=$(cat "$secret_path"); then
		echo "Unable to read secret: $secret_path" >&2
		return 1
	fi
	if [ -z "$secret_value" ]; then
		echo "Secret is empty: $secret_path" >&2
		return 1
	fi
	if printf '%s' "$secret_value" | LC_ALL=C grep -q '[[:cntrl:]]'; then
		echo "Secret must be a single line: $secret_path" >&2
		return 1
	fi
	printf '%s' "$secret_value"
}

validate_secret_file() {
	secret_value=
	if ! read_secret "$1" >/dev/null; then
		return 1
	fi
	unset secret_value
}

validate_text() {
	variable_name=$1
	variable_value=$2
	if printf '%s' "$variable_value" | LC_ALL=C grep -q '[[:cntrl:]]'; then
		echo "$variable_name contains a control character" >&2
		return 1
	fi
}

validate_domain() {
	case "$DOMAIN_NAME" in
		*[!A-Za-z0-9.-]*)
			echo "DOMAIN_NAME contains an invalid character" >&2
			return 1
			;;
	esac
}

validate_identifier() {
	variable_name=$1
	variable_value=$2
	case "$variable_value" in
		*[!A-Za-z0-9_]*)
			echo "$variable_name contains an invalid character" >&2
			return 1
			;;
	esac
}

validate_username() {
	variable_name=$1
	variable_value=$2
	case "$variable_value" in
		*[!A-Za-z0-9._-]*)
			echo "$variable_name contains an invalid character" >&2
			return 1
			;;
	esac
}

validate_email() {
	variable_name=$1
	variable_value=$2
	validate_text "$variable_name" "$variable_value" || return 1
	case "$variable_value" in
		*[[:space:]]*)
			echo "$variable_name must not contain whitespace" >&2
			return 1
			;;
		*@*)
			;;
		*)
			echo "$variable_name must contain an @" >&2
			return 1
			;;
	esac
}

validate_environment() {
	if [ -z "${DOMAIN_NAME:-}" ] \
		|| [ -z "${MYSQL_DATABASE:-}" ] \
		|| [ -z "${MYSQL_USER:-}" ] \
		|| [ -z "${WORDPRESS_ADMIN_USER:-}" ] \
		|| [ -z "${WORDPRESS_ADMIN_EMAIL:-}" ] \
		|| [ -z "${WORDPRESS_USER:-}" ] \
		|| [ -z "${WORDPRESS_USER_EMAIL:-}" ]; then
		echo "Required WordPress environment variables are missing" >&2
		return 1
	fi

	validate_text DOMAIN_NAME "$DOMAIN_NAME" || return 1
	validate_text MYSQL_DATABASE "$MYSQL_DATABASE" || return 1
	validate_text MYSQL_USER "$MYSQL_USER" || return 1
	validate_text WORDPRESS_ADMIN_USER "$WORDPRESS_ADMIN_USER" || return 1
	validate_text WORDPRESS_ADMIN_EMAIL "$WORDPRESS_ADMIN_EMAIL" || return 1
	validate_text WORDPRESS_USER "$WORDPRESS_USER" || return 1
	validate_text WORDPRESS_USER_EMAIL "$WORDPRESS_USER_EMAIL" || return 1
	validate_domain || return 1
	validate_identifier MYSQL_DATABASE "$MYSQL_DATABASE" || return 1
	validate_identifier MYSQL_USER "$MYSQL_USER" || return 1
	validate_username WORDPRESS_ADMIN_USER "$WORDPRESS_ADMIN_USER" || return 1
	validate_username WORDPRESS_USER "$WORDPRESS_USER" || return 1
	validate_email WORDPRESS_ADMIN_EMAIL "$WORDPRESS_ADMIN_EMAIL" || return 1
	validate_email WORDPRESS_USER_EMAIL "$WORDPRESS_USER_EMAIL" || return 1

	admin_user_lower=$(printf '%s' "$WORDPRESS_ADMIN_USER" \
		| LC_ALL=C tr '[:upper:]' '[:lower:]')
	case "$admin_user_lower" in
		*admin*|*administrator*)
			echo "WORDPRESS_ADMIN_USER contains a forbidden name" >&2
			return 1
			;;
	esac
}

write_database_client_config() {
	client_password=$(read_secret "$DB_SECRET") || return 1
	mkdir -p "$RUNTIME_DIR"
	chmod 700 "$RUNTIME_DIR"
	{
		printf '%s\n' '[client]'
		printf 'host=%s\n' "$DB_HOST"
		printf 'port=%s\n' "$DB_PORT"
		printf 'user=%s\n' "$MYSQL_USER"
		printf 'password=%s\n' "$client_password"
	} > "$DB_CLIENT_CONFIG"
	unset client_password
	chmod 600 "$DB_CLIENT_CONFIG"
}

wait_for_database() {
	attempt=1
	max_attempts=30
	while [ "$attempt" -le "$max_attempts" ]; do
		if mariadb-admin --defaults-extra-file="$DB_CLIENT_CONFIG" \
			--protocol=tcp --connect-timeout=2 \
			--host="$DB_HOST" --port="$DB_PORT" \
			ping --silent >/dev/null 2>&1; then
			return 0
		fi

		if [ "$attempt" -lt "$max_attempts" ]; then
			sleep 2
		fi
		attempt=$((attempt + 1))
	done

	echo "MariaDB did not become ready after $max_attempts attempts" >&2
	return 1
}

wait_for_redis() {
	attempt=1
	max_attempts=30
	while [ "$attempt" -le "$max_attempts" ]; do
		if php -r '
$redis = new Redis();
if (!$redis->connect("redis", 6379, 2.0)) {
	exit(1);
}
$password = file_get_contents("/run/secrets/redis_password");
if (false === $password || !$redis->auth(rtrim($password, "\r\n"))) {
	exit(1);
}
exit($redis->ping() ? 0 : 1);
' >/dev/null 2>&1; then
			return 0
		fi

		if [ "$attempt" -lt "$max_attempts" ]; then
			sleep 2
		fi
		attempt=$((attempt + 1))
	done

	echo "Redis did not become ready after $max_attempts attempts" >&2
	return 1
}

copy_wordpress_files() {
	mkdir -p "$WEB_ROOT"
	if [ ! -f "$WEB_ROOT/index.php" ] || [ ! -f "$WEB_ROOT/wp-includes/version.php" ]; then
		existing_entry=$(find "$WEB_ROOT" -mindepth 1 -maxdepth 1 -print -quit)
		if [ -n "$existing_entry" ]; then
			echo "WordPress directory is incomplete and is not empty" >&2
			return 1
		fi
		if ! cp -a "$SOURCE_ROOT"/. "$WEB_ROOT"/; then
			echo "Unable to copy WordPress files" >&2
			return 1
		fi
	fi

	if [ ! -f "$WEB_ROOT/index.php" ] || [ ! -f "$WEB_ROOT/wp-includes/version.php" ]; then
		echo "WordPress files are incomplete" >&2
		return 1
	fi
	chown -R www-data:www-data "$WEB_ROOT"
}

copy_redis_plugin() {
	plugin_source=$SOURCE_ROOT/wp-content/plugins/$REDIS_PLUGIN
	plugin_target=$WEB_ROOT/wp-content/plugins/$REDIS_PLUGIN
	if [ ! -f "$plugin_source/redis-cache.php" ]; then
		echo "Redis Object Cache plugin is missing from the image" >&2
		return 1
	fi
	if [ -f "$plugin_target/redis-cache.php" ]; then
		return 0
	fi
	if [ -e "$plugin_target" ] || [ -L "$plugin_target" ]; then
		echo "Redis Object Cache plugin directory is incomplete" >&2
		return 1
	fi
	mkdir -p "$WEB_ROOT/wp-content/plugins"
	cp -a "$plugin_source" "$plugin_target"
}

create_wp_config() {
	CONFIG_TMP=$(mktemp "$WEB_ROOT/.wp-config.php.XXXXXX") || {
		echo "Unable to create temporary wp-config.php" >&2
		return 1
	}
	if ! WP_CONFIG_PATH="$CONFIG_TMP" \
		WP_DB_NAME="$MYSQL_DATABASE" \
		WP_DB_USER="$MYSQL_USER" \
		WP_DB_HOST="$DB_HOST:$DB_PORT" \
		php /usr/local/bin/create-wp-config.php < "$DB_SECRET"; then
		echo "Unable to create wp-config.php" >&2
		return 1
	fi
	if ! chown root:www-data "$CONFIG_TMP" \
		|| ! chmod 640 "$CONFIG_TMP" \
		|| ! mv "$CONFIG_TMP" "$WP_CONFIG"; then
		echo "Unable to install wp-config.php" >&2
		return 1
	fi
	CONFIG_TMP=
}

ensure_wp_config() {
	if [ -e "$WP_CONFIG" ]; then
		if [ ! -f "$WP_CONFIG" ]; then
			echo "wp-config.php is not a regular file" >&2
			return 1
		fi
		return 0
	fi
	create_wp_config
}

configure_redis_wp_config() {
	if grep -q 'WP_REDIS_HOST' "$WP_CONFIG"; then
		return 0
	fi
	CONFIG_TMP=$(mktemp "$WEB_ROOT/.wp-config.php.XXXXXX") || {
		echo "Unable to create temporary wp-config.php for Redis" >&2
		return 1
	}
	if ! WP_CONFIG_PATH="$WP_CONFIG" \
		WP_CONFIG_OUTPUT="$CONFIG_TMP" \
		WP_REDIS_HOST="$REDIS_HOST" \
		WP_REDIS_PORT="$REDIS_PORT" \
		WP_REDIS_PASSWORD_FILE="$REDIS_SECRET" \
		php "$REDIS_CONFIG_TOOL"; then
		echo "Unable to configure Redis in wp-config.php" >&2
		return 1
	fi
	if ! chown root:www-data "$CONFIG_TMP" \
		|| ! chmod 640 "$CONFIG_TMP" \
		|| ! mv "$CONFIG_TMP" "$WP_CONFIG"; then
		echo "Unable to install Redis-enabled wp-config.php" >&2
		return 1
	fi
	CONFIG_TMP=
}

wp_cli() {
	wp --allow-root --path="$WEB_ROOT" "$@"
}

install_wordpress() {
	if wp_cli core is-installed --quiet >/dev/null 2>&1; then
		return 0
	fi
	if ! wp_cli core install \
		--url="https://$DOMAIN_NAME" \
		--title='Inception' \
		--admin_user="$WORDPRESS_ADMIN_USER" \
		--admin_email="$WORDPRESS_ADMIN_EMAIL" \
		--locale=en_US \
		--skip-email \
		--prompt=admin_password \
		--quiet < "$WP_ADMIN_SECRET" >/dev/null; then
		echo "WordPress installation failed" >&2
		return 1
	fi
}

ensure_user() {
	user_login=$1
	user_email=$2
	user_role=$3
	password_file=$4
	if wp_cli user get "$user_login" --field=ID --quiet >/dev/null 2>&1; then
		return 0
	fi
	if ! wp_cli user create "$user_login" "$user_email" \
		--role="$user_role" \
		--prompt=user_pass \
		--porcelain \
		--quiet < "$password_file" >/dev/null; then
		echo "Unable to create WordPress user: $user_login" >&2
		return 1
	fi
}

enable_redis_cache() {
	if ! wp_cli plugin is-installed "$REDIS_PLUGIN" --quiet >/dev/null 2>&1; then
		echo "Redis Object Cache plugin is not installed" >&2
		return 1
	fi
	if ! wp_cli plugin is-active "$REDIS_PLUGIN" --quiet >/dev/null 2>&1; then
		if ! wp_cli plugin activate "$REDIS_PLUGIN" --quiet >/dev/null 2>&1; then
			echo "Unable to activate Redis Object Cache plugin" >&2
			return 1
		fi
	fi
	if ! wp_cli redis enable --quiet >/dev/null 2>&1; then
		echo "Unable to enable Redis object cache" >&2
		return 1
	fi
}

set_wordpress_permissions() {
	chown -R www-data:www-data "$WEB_ROOT"
	chown root:www-data "$WP_CONFIG"
	chmod 640 "$WP_CONFIG"
}

cleanup() {
	status=$?
	if [ -n "$CONFIG_TMP" ]; then
		rm -f "$CONFIG_TMP" || true
	fi
	rm -f "$DB_CLIENT_CONFIG" || true
	trap - EXIT
	exit "$status"
}

trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

validate_environment || fail "WordPress environment validation failed"
validate_secret_file "$DB_SECRET" || fail "Database secret validation failed"
validate_secret_file "$REDIS_SECRET" || fail "Redis secret validation failed"
validate_secret_file "$WP_ADMIN_SECRET" || fail "WordPress administrator secret validation failed"
validate_secret_file "$WP_USER_SECRET" || fail "WordPress user secret validation failed"
write_database_client_config || fail "Unable to prepare the MariaDB client"
wait_for_database || fail "Unable to connect to MariaDB"
wait_for_redis || fail "Unable to connect to Redis"
copy_wordpress_files || fail "Unable to prepare the WordPress directory"
copy_redis_plugin || fail "Unable to prepare the Redis Object Cache plugin"
ensure_wp_config || fail "Unable to prepare wp-config.php"
configure_redis_wp_config || fail "Unable to configure Redis"
install_wordpress || fail "Unable to initialize WordPress"
ensure_user "$WORDPRESS_ADMIN_USER" "$WORDPRESS_ADMIN_EMAIL" administrator \
	"$WP_ADMIN_SECRET" || fail "Unable to ensure the WordPress administrator"
ensure_user "$WORDPRESS_USER" "$WORDPRESS_USER_EMAIL" editor \
	"$WP_USER_SECRET" || fail "Unable to ensure the regular WordPress user"
enable_redis_cache || fail "Unable to enable Redis cache"
set_wordpress_permissions

rm -f "$DB_CLIENT_CONFIG"
rmdir "$RUNTIME_DIR" 2>/dev/null || true
unset secret_value client_password

if [ "$#" -eq 0 ]; then
	set -- php-fpm8.2 --nodaemonize
fi
exec "$@"
