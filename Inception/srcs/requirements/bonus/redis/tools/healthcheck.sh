#!/bin/sh

set -eu

SECRET_PATH=/run/secrets/redis_password

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

{
	printf 'AUTH default %s\n' "$password"
	printf 'PING\n'
} | redis-cli -s /run/redis/redis.sock --raw --no-auth-warning 2>/dev/null \
	| grep -qx 'PONG'
