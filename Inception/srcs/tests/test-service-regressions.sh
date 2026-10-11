#!/bin/sh
# 実サービスや利用者を変更せず、DB・Redis の認証、FTP 制限、バックアップと復元を一時領域とモックで検証する。

set -eu
PROJECT_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
TEST_ROOT=$(mktemp -d "${TMPDIR:-/tmp}/inception-service-test.XXXXXX")
trap 'rm -rf -- "$TEST_ROOT"' EXIT
trap 'exit 1' HUP INT TERM
mkdir -p "$TEST_ROOT/bin" "$TEST_ROOT/wordpress" "$TEST_ROOT/backups"
SERVICE_TEST_ROOT=$TEST_ROOT
SERVICE_TEST_SECRET=$TEST_ROOT/secret
SERVICE_TEST_TRACE=$TEST_ROOT/trace
export SERVICE_TEST_ROOT SERVICE_TEST_SECRET SERVICE_TEST_TRACE

sed -e "s|^SECRET_PATH=.*|SECRET_PATH=$SERVICE_TEST_SECRET|" \
	-e "s|/tmp/mariadb-healthcheck|$TEST_ROOT/mariadb-healthcheck|" \
	"$PROJECT_ROOT/srcs/requirements/mariadb/tools/healthcheck.sh" > "$TEST_ROOT/db-health.sh"
sed '/^trap cleanup EXIT/,$d' \
	"$PROJECT_ROOT/srcs/requirements/wordpress/tools/entrypoint.sh" > "$TEST_ROOT/wp-functions.sh"
cat > "$TEST_ROOT/wp-client.sh" <<'SCRIPT'
#!/bin/sh
set -eu
. "$SERVICE_TEST_ROOT/wp-functions.sh"
DB_SECRET=$SERVICE_TEST_SECRET
RUNTIME_DIR=$SERVICE_TEST_ROOT/wp-runtime
DB_CLIENT_CONFIG=$RUNTIME_DIR/.mariadb.cnf
MYSQL_USER=wp_user
write_database_client_config
wait_for_database
SCRIPT
sed -e "s|^BACKUP_ROOT=.*|BACKUP_ROOT=$TEST_ROOT/backups|" \
	-e "s|^SOURCE_ROOT=.*|SOURCE_ROOT=$TEST_ROOT/wordpress|" \
	-e "s|^DB_SECRET=.*|DB_SECRET=$SERVICE_TEST_SECRET|" \
	-e "s|^RUNTIME_DIR=.*|RUNTIME_DIR=$TEST_ROOT/backup-runtime|" \
	"$PROJECT_ROOT/srcs/requirements/bonus/backup/tools/backup.sh" > "$TEST_ROOT/backup.sh"
sed -e "s|^BACKUP_ROOT=.*|BACKUP_ROOT=$TEST_ROOT/backups|" \
	-e "s|^WORK_ROOT=.*|WORK_ROOT=$TEST_ROOT/restore-work|" \
	"$PROJECT_ROOT/srcs/requirements/bonus/restore-check/tools/restore-check.sh" > "$TEST_ROOT/restore.sh"
sed -e "s|^WEB_ROOT=.*|WEB_ROOT=$TEST_ROOT/wordpress|" \
	-e "s|^SECRET_PATH=.*|SECRET_PATH=$SERVICE_TEST_SECRET|" \
	-e "s|^CONFIG_TEMPLATE=.*|CONFIG_TEMPLATE=$TEST_ROOT/ftp-template|" \
	-e "s|^CONFIG_PATH=.*|CONFIG_PATH=$TEST_ROOT/vsftpd.conf|" \
	-e "s|/etc/vsftpd/vsftpd.conf.XXXXXX|$TEST_ROOT/vsftpd.conf.XXXXXX|" \
	"$PROJECT_ROOT/srcs/requirements/bonus/ftp/tools/entrypoint.sh" > "$TEST_ROOT/ftp.sh"
sed -e "s|^SECRET_PATH=.*|SECRET_PATH=$SERVICE_TEST_SECRET|" \
	-e "s|^CONFIG_TEMPLATE=.*|CONFIG_TEMPLATE=$TEST_ROOT/redis-template|" \
	-e "s|^RUNTIME_DIR=.*|RUNTIME_DIR=$TEST_ROOT/redis-runtime|" \
	"$PROJECT_ROOT/srcs/requirements/bonus/redis/tools/entrypoint.sh" > "$TEST_ROOT/redis.sh"
sed "s|^SECRET_PATH=.*|SECRET_PATH=$SERVICE_TEST_SECRET|" \
	"$PROJECT_ROOT/srcs/requirements/bonus/redis/tools/healthcheck.sh" > "$TEST_ROOT/redis-health.sh"
cp "$PROJECT_ROOT/srcs/requirements/bonus/redis/conf/redis.conf" "$TEST_ROOT/redis-template"
cp "$PROJECT_ROOT/srcs/requirements/bonus/ftp/conf/vsftpd.conf" "$TEST_ROOT/ftp-template"
mkdir "$TEST_ROOT/wordpress/wp-includes"
printf '%s\n' '<?php' "\$table_prefix = 'wp_';" > "$TEST_ROOT/wordpress/wp-config.php"
printf '%s\n' '<?php // dummy core' > "$TEST_ROOT/wordpress/wp-includes/version.php"

cat > "$TEST_ROOT/bin/check-client" <<'MOCK'
#!/bin/sh
set -eu
config=
for argument do
	case "$argument" in --defaults-extra-file=*) config=${argument#*=} ;; esac
done
[ -n "$config" ] || exit 0
if command -v my_print_defaults >/dev/null 2>&1; then
	parsed=$(my_print_defaults --defaults-file="$config" client | sed -n 's/^--password=//p')
	expected=$(cat "$SERVICE_TEST_SECRET")
	[ "$parsed" = "$expected" ] || { echo 'Client password round-trip failed' >&2; exit 1; }
fi
MOCK
cat > "$TEST_ROOT/bin/mariadb" <<'MOCK'
#!/bin/sh
set -eu
printf 'mariadb %s\n' "$*" >> "$SERVICE_TEST_TRACE"
check-client "$@"
[ "${MOCK_DB_DENIED:-0}" = 0 ] || exit 1
case "$*" in
	*information_schema.tables*) printf '2\n' ;;
	*options*) printf '3\n' ;;
	*--execute=*) printf '1\n' ;;
	*) cat >> "$SERVICE_TEST_ROOT/imported.sql" ;;
esac
MOCK
cat > "$TEST_ROOT/bin/mariadb-admin" <<'MOCK'
#!/bin/sh
# Ping succeeds even with invalid credentials; regressions must detect this.
exit 0
MOCK
cat > "$TEST_ROOT/bin/mariadb-dump" <<'MOCK'
#!/bin/sh
set -eu
check-client "$@"
[ "${MOCK_DUMP_FAILURE:-0}" = 0 ] || exit 1
printf '%s\n' 'CREATE TABLE fixture (id INT);'
MOCK
cat > "$TEST_ROOT/bin/flock" <<'MOCK'
#!/bin/sh
[ "${MOCK_LOCK_BUSY:-0}" = 0 ]
MOCK
cat > "$TEST_ROOT/bin/id" <<'MOCK'
#!/bin/sh
[ "$1" = -u ] || exit 1
case "$2" in
	www-data) printf '33\n' ;;
	*) printf '%s\n' "${MOCK_FTP_UID:-100}" ;;
esac
MOCK
cat > "$TEST_ROOT/bin/getent" <<'MOCK'
#!/bin/sh
[ "${MOCK_FTP_EXISTS:-1}" = 1 ]
MOCK
for command_name in useradd usermod setfacl chown; do
	cat > "$TEST_ROOT/bin/$command_name" <<'MOCK'
#!/bin/sh
printf '%s\n' "${0##*/} $*" >> "$SERVICE_TEST_TRACE"
MOCK
done
cat > "$TEST_ROOT/bin/chpasswd" <<'MOCK'
#!/bin/sh
cat >/dev/null
printf 'chpasswd\n' >> "$SERVICE_TEST_TRACE"
MOCK
cat > "$TEST_ROOT/bin/setpriv" <<'MOCK'
#!/bin/sh
shift 3
exec "$@"
MOCK
cat > "$TEST_ROOT/bin/sleep" <<'MOCK'
#!/bin/sh
exit 0
MOCK
cat > "$TEST_ROOT/bin/redis-cli" <<'MOCK'
#!/bin/sh
cat > "$SERVICE_TEST_ROOT/redis-commands"
printf 'PONG\n'
MOCK
cat > "$TEST_ROOT/bin/docker" <<'MOCK'
#!/bin/sh
set -eu
printf 'docker %s\n' "$*" >> "$SERVICE_TEST_TRACE"
for argument do last_argument=$argument; done
case "$*" in
	*'run --rm --no-deps --build backup'*)
		[ "${MOCK_MAKE_BACKUP_FAILURE:-0}" = 0 ] || exit 1
		printf '%s\n' "$last_argument" > "$SERVICE_TEST_ROOT/make-generation" ;;
	*'run --rm restore-check'*)
		[ "$last_argument" = "$(cat "$SERVICE_TEST_ROOT/make-generation")" ]
		[ "${MOCK_MAKE_RESTORE_FAILURE:-0}" = 0 ] || exit 1 ;;
esac
MOCK
chmod 755 "$TEST_ROOT/bin/"*
PATH=$TEST_ROOT/bin:$PATH
export PATH
MYSQL_DATABASE=wordpress
MYSQL_USER=wp_user
FTP_PASV_ADDRESS=127.0.0.1
BACKUP_RETENTION=1000
export MYSQL_DATABASE MYSQL_USER FTP_PASV_ADDRESS BACKUP_RETENTION
count=0

run_case() {
	case_name=$1
	expected=$2
	shift 2
	status=0
	: > "$SERVICE_TEST_TRACE"
	"$@" > "$TEST_ROOT/output" 2>&1 || status=$?
	if { [ "$expected" = success ] && [ "$status" -eq 0 ]; } \
		|| { [ "$expected" = failure ] && [ "$status" -ne 0 ]; }; then
		count=$((count + 1))
		printf 'ok %s - %s\n' "$count" "$case_name"
	else
		printf 'FAIL: %s (exit=%s)\n' "$case_name" "$status" >&2
		cat "$TEST_ROOT/output" >&2
		exit 1
	fi
}

if ! command -v my_print_defaults >/dev/null 2>&1; then
	printf 'SKIP: MariaDB password parser checks (my_print_defaults is unavailable).\n'
fi
for password_case in space quotes backslashes mixed; do
	case "$password_case" in
		space) printf '%s\n' ' leading and trailing ' ;;
		quotes) printf '%s\n' '"double" and '\''single'\''' ;;
		backslashes) printf '%s\n' 'literal\n\t\x41\\ending\' ;;
		mixed) printf '%s\n' ' "quote"\path #;=$[] ` ' ;;
	esac > "$SERVICE_TEST_SECRET"
	run_case "db_password_$password_case" success sh "$TEST_ROOT/db-health.sh"
	run_case "wp_password_$password_case" success sh "$TEST_ROOT/wp-client.sh"
	run_case "backup_password_$password_case" success sh "$TEST_ROOT/backup.sh" now "20261011T000000Z-$password_case"
done
run_case db_rejects_bad_credentials failure env MOCK_DB_DENIED=1 sh "$TEST_ROOT/db-health.sh"
run_case wp_wait_rejects_bad_credentials failure env MOCK_DB_DENIED=1 sh "$TEST_ROOT/wp-client.sh"

for ftp_case in www-data php-alias root-alias; do
	case "$ftp_case" in root-alias) ftp_uid=0 ;; *) ftp_uid=33 ;; esac
	run_case "ftp_rejects_$ftp_case" failure env FTP_USER="$ftp_case" MOCK_FTP_UID="$ftp_uid" sh "$TEST_ROOT/ftp.sh" true
	[ ! -s "$SERVICE_TEST_TRACE" ] || { echo 'Rejected FTP account was modified' >&2; exit 1; }
done
run_case ftp_existing_dedicated_user success env FTP_USER=ftpuser sh "$TEST_ROOT/ftp.sh" true
run_case ftp_new_dedicated_user success env FTP_USER=ftpuser MOCK_FTP_EXISTS=0 sh "$TEST_ROOT/ftp.sh" true
run_case backup_lock_conflict failure env MOCK_LOCK_BUSY=1 sh "$TEST_ROOT/backup.sh" now
run_case backup_dump_failure failure env MOCK_DUMP_FAILURE=1 sh "$TEST_ROOT/backup.sh" now 20261011T000000Z-failed
[ ! -e "$TEST_ROOT/backups/20261011T000000Z-failed" ]
run_case backup_rejects_duplicate failure sh "$TEST_ROOT/backup.sh" now 20261011T000000Z-space
run_case backup_rejects_traversal failure sh "$TEST_ROOT/backup.sh" now ../outside

# A corrupt newer generation must not replace the explicitly requested backup.
mkdir "$TEST_ROOT/backups/20991231T235959Z"
run_case restore_requested_generation success sh "$TEST_ROOT/restore.sh" 20261011T000000Z-space
grep -q 'succeeded: 20261011T000000Z-space ' "$TEST_ROOT/output"
run_case restore_latest_detects_corruption failure sh "$TEST_ROOT/restore.sh"
run_case restore_missing_generation failure sh "$TEST_ROOT/restore.sh" 20261011T000000Z-missing
run_case restore_rejects_traversal failure sh "$TEST_ROOT/restore.sh" ../outside

run_case backup_retention_keeps_requested_generation success env BACKUP_RETENTION=1 \
	sh "$TEST_ROOT/backup.sh" now 20261011T000000Z-retained
[ -f "$TEST_ROOT/backups/20261011T000000Z-retained/SHA256SUMS" ]
run_case restore_retained_generation success sh "$TEST_ROOT/restore.sh" 20261011T000000Z-retained

printf '%s\n' ' "quote"\path #;=$[] ` ' > "$SERVICE_TEST_SECRET"
run_case redis_renders_password success sh "$TEST_ROOT/redis.sh" true
run_case redis_healthcheck_quotes_password success sh "$TEST_ROOT/redis-health.sh"
cat > "$TEST_ROOT/expected-redis-commands" <<'EXPECTED'
AUTH default " \"quote\"\\path #;=$[] ` "
PING
EXPECTED
cmp "$TEST_ROOT/expected-redis-commands" "$TEST_ROOT/redis-commands"
if command -v redis-server >/dev/null 2>&1; then
	status=0
	redis-server "$TEST_ROOT/redis-runtime/redis.conf" --port 0 --unixsocket '' \
		--dir "$TEST_ROOT" --daemonize no > "$TEST_ROOT/redis-parser-output" 2>&1 || status=$?
	[ "$status" -ne 0 ] && grep -q 'Configured to not listen anywhere' "$TEST_ROOT/redis-parser-output" \
		|| { cat "$TEST_ROOT/redis-parser-output" >&2; exit 1; }
	count=$((count + 1))
	printf 'ok %s - redis_configuration_parser\n' "$count"
else
	printf 'SKIP: Redis configuration parser check (redis-server is unavailable).\n'
fi

mkdir -p "$TEST_ROOT/project/srcs/tools"
cp "$PROJECT_ROOT/Makefile" "$TEST_ROOT/project/Makefile"
for script_name in prepare-storage check-config; do
	printf '#!/bin/sh\nexit 0\n' > "$TEST_ROOT/project/srcs/tools/$script_name.sh"
	chmod 755 "$TEST_ROOT/project/srcs/tools/$script_name.sh"
done
run_case make_restore_binds_generation success make -s -C "$TEST_ROOT/project" restore-check
grep -q 'run --rm restore-check 20' "$SERVICE_TEST_TRACE"
run_case make_restore_stops_on_backup_failure failure env MOCK_MAKE_BACKUP_FAILURE=1 make -s -C "$TEST_ROOT/project" restore-check
! grep -Eq 'build restore-db|run --rm restore-check' "$SERVICE_TEST_TRACE"
run_case make_restore_propagates_failure failure env MOCK_MAKE_RESTORE_FAILURE=1 make -s -C "$TEST_ROOT/project" restore-check
grep -q 'rm -sf restore-db' "$SERVICE_TEST_TRACE"
printf 'All %s service regression checks passed.\n' "$count"
