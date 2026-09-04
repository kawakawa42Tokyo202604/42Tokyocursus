#!/bin/sh

set -eu

PROJECT_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
ENV_FILE=$PROJECT_ROOT/srcs/.env
PROFILE_DIR=${TMPDIR:-/tmp}/inception-review

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

VM_IP=${VM_IP:-}
if [ -z "$VM_IP" ]; then
	VM_IP=$(ip -4 route get 1.1.1.1 2>/dev/null \
		| awk '{ for (i = 1; i <= NF; i++) if ($i == "src") { print $(i + 1); exit } }')
fi
case "$VM_IP" in
	''|*[!0-9.]*)
		fail "Unable to determine the VM IPv4 address; set VM_IP explicitly"
		;;
esac

if command -v google-chrome >/dev/null 2>&1; then
	BROWSER=google-chrome
elif command -v chromium >/dev/null 2>&1; then
	BROWSER=chromium
else
	fail "Google Chrome or Chromium is required"
fi

exec "$BROWSER" \
	--user-data-dir="$PROFILE_DIR" \
	--no-first-run \
	--new-window \
	"--host-resolver-rules=MAP $DOMAIN_NAME $VM_IP" \
	"https://$DOMAIN_NAME"
