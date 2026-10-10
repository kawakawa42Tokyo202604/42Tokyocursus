#!/bin/sh

set -eu

BACKUP_ROOT=/var/backups/inception
WORK_ROOT=/tmp/inception-restore-check
RESTORE_DB_HOST=${RESTORE_DB_HOST:-restore-db}
RESTORE_DB_PORT=${RESTORE_DB_PORT:-3306}
RESTORE_DB=

fail() {
	echo "$1" >&2
	exit 1
}

cleanup() {
	status=$?
	rm -rf -- "$WORK_ROOT"
	exit "$status"
}

trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

[ -d "$BACKUP_ROOT" ] || fail "Backup volume is unavailable"

latest_name=$(find "$BACKUP_ROOT" -mindepth 1 -maxdepth 1 \
	-type d -name '20*' -printf '%f\n' | LC_ALL=C sort | tail -n 1)
[ -n "$latest_name" ] || fail "No completed backup was found"
case "$latest_name" in
	''|*[!A-Za-z0-9T_.-]*) fail "Invalid backup directory name" ;;
esac

latest="$BACKUP_ROOT/$latest_name"
for required_file in SHA256SUMS manifest.txt database.sql.gz wordpress.tar.gz; do
	[ -f "$latest/$required_file" ] || fail "Backup is missing $required_file"
done

if ! (cd "$latest" && sha256sum -c SHA256SUMS); then
	fail "Backup checksum verification failed"
fi
gzip -t "$latest/database.sql.gz"
tar -tzf "$latest/wordpress.tar.gz" >/dev/null

mkdir -p "$WORK_ROOT/wordpress"
tar -xzf "$latest/wordpress.tar.gz" \
	--directory="$WORK_ROOT/wordpress" \
	--no-same-owner --no-same-permissions
[ -f "$WORK_ROOT/wordpress/wp-config.php" ] || fail "Restored WordPress config is missing"
[ -f "$WORK_ROOT/wordpress/wp-includes/version.php" ] || \
	fail "Restored WordPress core is incomplete"

attempt=1
while [ "$attempt" -le 30 ]; do
	if mariadb-admin --protocol=tcp --host="$RESTORE_DB_HOST" \
		--port="$RESTORE_DB_PORT" --user=root --skip-password \
		ping --silent >/dev/null 2>&1; then
		break
	fi
	sleep 1
	attempt=$((attempt + 1))
done
if [ "$attempt" -gt 30 ]; then
	fail "Temporary MariaDB did not become ready"
fi

RESTORE_DB="restore_check_$(date -u '+%Y%m%d%H%M%S')_$$"
mariadb --protocol=tcp --host="$RESTORE_DB_HOST" --port="$RESTORE_DB_PORT" \
	--user=root --skip-password \
	--execute="CREATE DATABASE \`$RESTORE_DB\`;"

gzip -dc "$latest/database.sql.gz" > "$WORK_ROOT/database.sql"
mariadb --protocol=tcp --host="$RESTORE_DB_HOST" --port="$RESTORE_DB_PORT" \
	--user=root --skip-password "$RESTORE_DB" \
	< "$WORK_ROOT/database.sql"

table_count=$(mariadb --protocol=tcp --host="$RESTORE_DB_HOST" \
	--port="$RESTORE_DB_PORT" --user=root --skip-password \
	--batch --skip-column-names \
	--execute="SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = '$RESTORE_DB';")
case "$table_count" in
	''|*[!0-9]*) fail "Restored database table count is invalid" ;;
esac
[ "$table_count" -gt 0 ] || fail "Restored database has no tables"

table_prefix=$(grep -E '^\$table_prefix[[:space:]]*=' \
	"$WORK_ROOT/wordpress/wp-config.php" \
	| sed -n "s/.*= ['\"]\\([A-Za-z0-9_]*\\)['\"].*/\\1/p" \
	| head -n 1)
case "$table_prefix" in
	''|*[!A-Za-z0-9_]*) fail "Unable to determine the WordPress table prefix" ;;
esac

option_count=$(mariadb --protocol=tcp --host="$RESTORE_DB_HOST" \
	--port="$RESTORE_DB_PORT" --user=root --skip-password \
	--batch --skip-column-names \
	--execute="SELECT COUNT(*) FROM \`$RESTORE_DB\`.\`${table_prefix}options\`;" 2>/dev/null)
case "$option_count" in
	''|*[!0-9]*) fail "Restored WordPress options table is invalid" ;;
esac
[ "$option_count" -gt 0 ] || fail "Restored WordPress options table is empty"

printf 'Restore verification succeeded: %s (%s tables, %s options)\n' \
	"$latest_name" "$table_count" "$option_count"
