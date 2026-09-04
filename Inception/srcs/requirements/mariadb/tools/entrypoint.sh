#!/bin/sh

set -eu

DATA_DIR=/var/lib/mysql
SOCKET_DIR=/run/mysqld
SOCKET_PATH=$SOCKET_DIR/mysqld.sock
PID_PATH=$SOCKET_DIR/mysqld.pid
INIT_MARKER=$DATA_DIR/.inception-initialized
SQL_FILE=$SOCKET_DIR/.inception-init.sql
ROOT_SECRET=/run/secrets/db_root_password
DB_SECRET=/run/secrets/db_password

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

sql_escape() {
	printf '%s' "$1" | sed -e 's/\\/\\\\/g' -e "s/'/''/g"
}

write_initialization_sql() {
	db_password_sql=$(sql_escape "$DB_PASSWORD")
	root_password_sql=$(sql_escape "$ROOT_PASSWORD")

	{
		printf 'FLUSH PRIVILEGES;\n'
		printf 'CREATE DATABASE IF NOT EXISTS `%s`;\n' "$MYSQL_DATABASE"
		printf "CREATE USER IF NOT EXISTS '%s'@'%%' IDENTIFIED BY '%s';\n" \
			"$MYSQL_USER" "$db_password_sql"
		printf "ALTER USER '%s'@'%%' IDENTIFIED BY '%s';\n" \
			"$MYSQL_USER" "$db_password_sql"
		printf 'REVOKE ALL PRIVILEGES, GRANT OPTION FROM '\''%s'\''@'\''%%'\'';\n' \
			"$MYSQL_USER"
		printf 'GRANT ALL PRIVILEGES ON `%s`.* TO '\''%s'\''@'\''%%'\'';\n' \
			"$MYSQL_DATABASE" "$MYSQL_USER"
		printf "CREATE USER IF NOT EXISTS 'root'@'localhost' IDENTIFIED BY '%s';\n" \
			"$root_password_sql"
		printf "ALTER USER 'root'@'localhost' IDENTIFIED BY '%s';\n" \
			"$root_password_sql"
		printf 'FLUSH PRIVILEGES;\n'
	} > "$SQL_FILE"
	chmod 600 "$SQL_FILE"
}

initialize_database() {
    if [ ! -d "$DATA_DIR/mysql" ]; then
        existing_entry=$(find "$DATA_DIR" -mindepth 1 -maxdepth 1 \
			! -name 'lost+found' -print -quit)
		if [ -n "$existing_entry" ]; then
			echo "MariaDB data directory has no system database" >&2
			return 1
		fi
		if ! mariadb-install-db --user=mysql --datadir="$DATA_DIR" --skip-test-db \
			>/dev/null; then
			echo "MariaDB system database initialization failed" >&2
			return 1
		fi
	fi

	write_initialization_sql
	if ! mariadbd --user=mysql --datadir="$DATA_DIR" --bootstrap \
		--skip-networking --console < "$SQL_FILE"; then
		echo "MariaDB bootstrap initialization failed" >&2
		return 1
	fi

	printf '%s\n' 'initialized' > "$INIT_MARKER"
	chmod 600 "$INIT_MARKER"
        chown mysql:mysql "$INIT_MARKER"
}

cleanup() {
    status=$?
    rm -f "$SOCKET_PATH" "$PID_PATH" "$SQL_FILE"
    trap - EXIT
    exit "$status"
}

trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

if [ -z "${MYSQL_DATABASE:-}" ] || [ -z "${MYSQL_USER:-}" ]; then
    echo "MYSQL_DATABASE and MYSQL_USER are required" >&2
    exit 1
fi

case "$MYSQL_DATABASE" in
    *[!A-Za-z0-9_]* )
        echo "MYSQL_DATABASE contains an invalid character" >&2
        exit 1
        ;;
esac

case "$MYSQL_USER" in
    *[!A-Za-z0-9_]* )
        echo "MYSQL_USER contains an invalid character" >&2
        exit 1
        ;;
esac

ROOT_PASSWORD=$(read_secret "$ROOT_SECRET")
DB_PASSWORD=$(read_secret "$DB_SECRET")

mkdir -p "$SOCKET_DIR" "$DATA_DIR"
chown mysql:mysql "$SOCKET_DIR" "$DATA_DIR"
chown -R mysql:mysql "$DATA_DIR"

if [ -f "$INIT_MARKER" ]; then
	if [ ! -d "$DATA_DIR/mysql" ]; then
		echo "MariaDB initialization marker exists without system database" >&2
		exit 1
	fi
else
	initialize_database
fi

exec mariadbd --user=mysql --datadir="$DATA_DIR" --socket="$SOCKET_PATH" \
	--pid-file="$PID_PATH" --console
