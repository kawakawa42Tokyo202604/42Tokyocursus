#!/bin/sh

set -eu
umask 077

BACKUP_ROOT=/var/backups/inception
SOURCE_ROOT=/source/wordpress
DB_SECRET=/run/secrets/db_password
RUNTIME_DIR=/run/inception-backup
DB_CLIENT_CONFIG=$RUNTIME_DIR/.mariadb.cnf
LOCK_FILE=$BACKUP_ROOT/.backup.lock
BACKUP_RETENTION=${BACKUP_RETENTION:-7}
TEMP_DIR=

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

cleanup() {
	status=$?
	if [ -n "$TEMP_DIR" ]; then
		rm -rf -- "$TEMP_DIR"
	fi
	rm -f -- "$DB_CLIENT_CONFIG"
	exit "$status"
}

cleanup_old_backups() {
	completed_count=$(find "$BACKUP_ROOT" -mindepth 1 -maxdepth 1 \
		-type d -name '20*' -printf '%f\n' | wc -l)
	remove_count=$((completed_count - BACKUP_RETENTION))
	if [ "$remove_count" -le 0 ]; then
		return 0
	fi

	find "$BACKUP_ROOT" -mindepth 1 -maxdepth 1 -type d -name '20*' \
		-printf '%f\n' | LC_ALL=C sort | head -n "$remove_count" |
	while IFS= read -r old_name; do
		case "$old_name" in
			20*) rm -rf -- "$BACKUP_ROOT/$old_name" ;;
		esac
	done
}

if [ "${1:-now}" != now ] || [ "$#" -gt 1 ]; then
	fail "Usage: backup.sh now"
fi

case "${MYSQL_DATABASE:-}" in
	''|*[!A-Za-z0-9_]*)
		echo "MYSQL_DATABASE contains an invalid character" >&2
		exit 1
		;;
esac
case "${MYSQL_USER:-}" in
	''|*[!A-Za-z0-9_]*)
		echo "MYSQL_USER contains an invalid character" >&2
		exit 1
		;;
esac
case "$BACKUP_RETENTION" in
	''|*[!0-9]*)
		echo "BACKUP_RETENTION must be a positive integer" >&2
		exit 1
		;;
esac
if [ "$BACKUP_RETENTION" -lt 1 ] || [ "$BACKUP_RETENTION" -gt 1000 ]; then
	echo "BACKUP_RETENTION must be between 1 and 1000" >&2
	exit 1
fi

[ -d "$SOURCE_ROOT" ] || fail "WordPress source volume is unavailable"
[ -f "$SOURCE_ROOT/wp-config.php" ] || fail "WordPress source is not initialized"

mkdir -p "$BACKUP_ROOT" "$RUNTIME_DIR"
chmod 700 "$BACKUP_ROOT" "$RUNTIME_DIR"

exec 9>"$LOCK_FILE"
if ! flock -n 9; then
	echo "Another backup is already running" >&2
	exit 0
fi

trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

db_password=$(read_secret "$DB_SECRET") || exit 1
{
	printf '%s\n' '[client]'
	printf '%s\n' 'host=mariadb'
	printf '%s\n' 'port=3306'
	printf 'user=%s\n' "$MYSQL_USER"
	printf 'password=%s\n' "$db_password"
} > "$DB_CLIENT_CONFIG"
unset db_password
chmod 600 "$DB_CLIENT_CONFIG"

TEMP_DIR=$(mktemp -d "$BACKUP_ROOT/.incomplete.XXXXXX")

if ! mariadb-dump \
	--defaults-extra-file="$DB_CLIENT_CONFIG" \
	--protocol=tcp \
	--single-transaction \
	--quick \
	--skip-lock-tables \
	--routines \
	--events \
	--triggers \
	--no-create-db \
	"$MYSQL_DATABASE" > "$TEMP_DIR/database.sql"; then
	fail "MariaDB backup failed"
fi
gzip -9 "$TEMP_DIR/database.sql"

if ! tar -czf "$TEMP_DIR/wordpress.tar.gz" \
	--numeric-owner --directory="$SOURCE_ROOT" .; then
	fail "WordPress files backup failed"
fi

gzip -t "$TEMP_DIR/database.sql.gz"
tar -tzf "$TEMP_DIR/wordpress.tar.gz" >/dev/null

created_at=$(date -u '+%Y-%m-%dT%H:%M:%SZ')
{
	printf '%s\n' 'format=inception-backup-v1'
	printf 'created_at=%s\n' "$created_at"
	printf 'database=%s\n' "$MYSQL_DATABASE"
	printf '%s\n' 'database_archive=database.sql.gz'
	printf '%s\n' 'wordpress_archive=wordpress.tar.gz'
} > "$TEMP_DIR/manifest.txt"

(cd "$TEMP_DIR" && sha256sum database.sql.gz wordpress.tar.gz manifest.txt > SHA256SUMS)

backup_stamp=$(date -u '+%Y%m%dT%H%M%SZ')
final_dir="$BACKUP_ROOT/$backup_stamp"
if [ -e "$final_dir" ]; then
	final_dir="$BACKUP_ROOT/${backup_stamp}-$(date -u '+%s%N')"
fi
if [ -e "$final_dir" ]; then
	fail "Backup destination already exists"
fi
mv "$TEMP_DIR" "$final_dir"
TEMP_DIR=

cleanup_old_backups
printf 'Backup created: %s\n' "$final_dir"
