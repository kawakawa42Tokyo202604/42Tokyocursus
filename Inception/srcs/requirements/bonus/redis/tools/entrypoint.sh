#!/bin/sh

set -eu

SECRET_PATH=/run/secrets/redis_password
CONFIG_TEMPLATE=/etc/redis/redis.conf.template
RUNTIME_DIR=/run/redis
CONFIG_PATH=$RUNTIME_DIR/redis.conf
CONFIG_TMP=

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
	if [ -n "$CONFIG_TMP" ]; then
		rm -f "$CONFIG_TMP" || true
	fi
	trap - EXIT
	exit "$status"
}

trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

if [ ! -f "$CONFIG_TEMPLATE" ] || [ ! -r "$CONFIG_TEMPLATE" ]; then
	echo "Redis configuration template is missing or unreadable" >&2
	exit 1
fi

REDIS_PASSWORD=$(read_secret "$SECRET_PATH") || exit 1
mkdir -p "$RUNTIME_DIR"
chmod 700 "$RUNTIME_DIR"
CONFIG_TMP=$(mktemp "$RUNTIME_DIR/redis.conf.XXXXXX")
cat "$CONFIG_TEMPLATE" > "$CONFIG_TMP"
printf 'requirepass %s\n' "$REDIS_PASSWORD" >> "$CONFIG_TMP"
unset REDIS_PASSWORD
chmod 600 "$CONFIG_TMP"
mv "$CONFIG_TMP" "$CONFIG_PATH"
CONFIG_TMP=

if [ "$#" -eq 0 ]; then
	set -- redis-server "$CONFIG_PATH" --daemonize no
fi
exec "$@"
