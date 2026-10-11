#!/bin/sh
# 秘密ファイルの root パスワードで MariaDB に接続し、SELECT 1 の成功を確認する。

set -eu

SECRET_PATH=/run/secrets/db_root_password
SOCKET_PATH=/run/mysqld/mysqld.sock
CONFIG_TMP=

cleanup() {
	status=$?
	if [ -n "$CONFIG_TMP" ]; then
		rm -f "$CONFIG_TMP"
	fi
	trap - EXIT
	exit "$status"
}

trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

if [ ! -f "$SECRET_PATH" ] || [ ! -r "$SECRET_PATH" ]; then
	exit 1
fi

if ! password=$(cat "$SECRET_PATH"); then
	exit 1
fi
if [ -z "$password" ]; then
	exit 1
fi
if printf '%s' "$password" | LC_ALL=C grep -q '[[:cntrl:]]'; then
	exit 1
fi

CONFIG_TMP=$(mktemp /tmp/mariadb-healthcheck.XXXXXX)
chmod 600 "$CONFIG_TMP"
{
	printf '%s\n' '[client]'
	printf '%s\n' 'user=root'
	printf 'password="%s"\n' "$(printf '%s' "$password" \
		| sed -e 's/\\/\\\\/g' -e 's/"/\\"/g')"
	printf 'protocol=socket\n'
	printf 'socket=%s\n' "$SOCKET_PATH"
} > "$CONFIG_TMP"
unset password

mariadb --defaults-extra-file="$CONFIG_TMP" --connect-timeout=2 \
	--batch --skip-column-names --execute='SELECT 1' >/dev/null
