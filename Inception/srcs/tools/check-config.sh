#!/bin/sh
# 環境設定、秘密ファイルの内容・権限、Compose と Docker の保存先・ボリュームを検証する。

set -eu

PROJECT_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
DATA_DIR=${DATA_DIR:-/home/takawaka/data}

fail() {
	echo "$1" >&2
	exit 1
}

cd "$PROJECT_ROOT"
[ -f srcs/.env ] && [ -r srcs/.env ] \
	|| fail "Missing srcs/.env; copy srcs/.env.example and configure it first"

[ -d secrets ] || fail "Missing secrets directory; run srcs/tools/generate-secrets.sh"
mode=$(stat -c '%a' secrets)
[ "$((0$mode & 077))" -eq 0 ] || fail "Restrict secrets directory permissions to 700"
for name in db_root_password db_password wp_admin_password wp_user_password redis_password ftp_password; do
	file=secrets/$name.txt
	[ -f "$file" ] && [ -r "$file" ] && [ -s "$file" ] \
		|| fail "Missing, unreadable or empty secret: $file"
	mode=$(stat -c '%a' "$file")
	[ "$((0$mode & 077))" -eq 0 ] || fail "Restrict secret permissions to 600: $file"
	value=$(cat "$file")
	[ -n "$value" ] || fail "Empty secret: $file"
	if printf '%s' "$value" | LC_ALL=C grep -q '[[:cntrl:]]'; then
		fail "Secret must be a single line: $file"
	fi
	unset value
done

docker compose -f srcs/docker-compose.yml --env-file srcs/.env config --quiet
case "$DATA_DIR" in
	/*) ;;
	*) fail "DATA_DIR must be an absolute path" ;;
esac
expected_root=$(readlink -m "$DATA_DIR/docker")
actual_root=$(docker info --format '{{.DockerRootDir}}') \
	|| fail "Cannot access Docker; use the VM administrator account"
[ "$actual_root" = "$expected_root" ] \
	|| fail "Docker data-root must be $expected_root; follow DEV_DOC.md before starting"

project=$(docker compose -f srcs/docker-compose.yml --env-file srcs/.env config \
	| sed -n 's/^name: //p')
[ -n "$project" ] || fail "Cannot determine the Compose project name"
for name in mariadb_data wordpress_data backup_data; do
	volume=${project}_$name
	if docker volume inspect "$volume" >/dev/null 2>&1; then
		details=$(docker volume inspect "$volume" \
			--format '{{.Driver}}|{{json .Options}}|{{.Mountpoint}}')
		case "$details" in
			"local|null|$actual_root/volumes/$volume/_data"|"local|{}|$actual_root/volumes/$volume/_data") ;;
			*) fail "Volume $volume is not a plain local named volume under Docker data-root" ;;
		esac
	fi
done
printf '%s\n' 'Configuration and Docker storage checks passed.'
