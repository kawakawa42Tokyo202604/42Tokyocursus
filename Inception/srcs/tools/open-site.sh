#!/bin/sh

set -eu

PROJECT_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
ENV_FILE=$PROJECT_ROOT/srcs/.env

fail() {
	echo "$1" >&2
	exit 1
}

if [ ! -f "$ENV_FILE" ] || [ ! -r "$ENV_FILE" ]; then
	fail "Missing or unreadable environment file: $ENV_FILE"
fi

DOMAIN_NAME=$(sed -n 's/^DOMAIN_NAME=//p' "$ENV_FILE" | sed -n '1p')
case "$DOMAIN_NAME" in
	''|*[!A-Za-z0-9.-]*|.*|*.)
		fail "DOMAIN_NAME is missing or invalid in $ENV_FILE"
		;;
esac

getent ahosts "$DOMAIN_NAME" >/dev/null \
	|| fail "Domain does not resolve; configure DNS or /etc/hosts first"

if command -v firefox >/dev/null 2>&1; then
	exec firefox --new-window "https://$DOMAIN_NAME"
elif command -v firefox-esr >/dev/null 2>&1; then
	exec firefox-esr --new-window "https://$DOMAIN_NAME"
elif command -v google-chrome >/dev/null 2>&1; then
	BROWSER=google-chrome
elif command -v chromium >/dev/null 2>&1; then
	BROWSER=chromium
else
	fail "Firefox, Google Chrome or Chromium is required"
fi

exec "$BROWSER" --new-window "https://$DOMAIN_NAME"
