#!/bin/sh

set -eu

PROJECT_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
SECRET_DIR=$PROJECT_ROOT/secrets

umask 077
mkdir -p "$SECRET_DIR"
chmod 700 "$SECRET_DIR"

TEMP_FILE=
cleanup() {
	if [ -n "$TEMP_FILE" ]; then
		rm -f "$TEMP_FILE"
	fi
}

trap cleanup EXIT INT TERM

for name in \
	db_root_password \
	db_password \
	wp_admin_password \
	wp_user_password \
	redis_password \
	ftp_password
do
	file="$SECRET_DIR/$name.txt"

	if [ -e "$file" ] || [ -L "$file" ]; then
		echo "Already exists: $file" >&2
		continue
	fi

	TEMP_FILE=$(mktemp "$SECRET_DIR/.$name.XXXXXX")
	openssl rand -base64 24 > "$TEMP_FILE"
	chmod 600 "$TEMP_FILE"
	mv "$TEMP_FILE" "$file"
	TEMP_FILE=
done
