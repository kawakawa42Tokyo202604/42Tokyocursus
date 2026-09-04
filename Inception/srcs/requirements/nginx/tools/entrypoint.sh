#!/bin/sh

set -eu

SSL_DIR=/etc/nginx/ssl
CERT_PATH=$SSL_DIR/server.crt
KEY_PATH=$SSL_DIR/server.key
CONFIG_TEMPLATE=/etc/nginx/nginx.conf.template
CONFIG_PATH=/etc/nginx/nginx.conf
CERT_TMP=
KEY_TMP=
CONFIG_TMP=

fail() {
	echo "$1" >&2
	exit 1
}

validate_domain() {
	if [ -z "${DOMAIN_NAME:-}" ]; then
		echo "DOMAIN_NAME is required" >&2
		return 1
	fi
	case "$DOMAIN_NAME" in
		*[!A-Za-z0-9.-]*|.*|*.)
			echo "DOMAIN_NAME contains an invalid hostname" >&2
			return 1
			;;
	esac
}

generate_certificate() {
	if [ -f "$CERT_PATH" ] && [ -f "$KEY_PATH" ]; then
		return 0
	fi

	if [ -e "$CERT_PATH" ] || [ -e "$KEY_PATH" ]; then
		rm -f "$CERT_PATH" "$KEY_PATH"
	fi

	mkdir -p "$SSL_DIR"
	chmod 700 "$SSL_DIR"
	CERT_TMP=$(mktemp "$SSL_DIR/server.crt.XXXXXX")
	KEY_TMP=$(mktemp "$SSL_DIR/server.key.XXXXXX")
	if ! openssl req -x509 -nodes -newkey rsa:2048 -sha256 -days 3650 \
		-keyout "$KEY_TMP" -out "$CERT_TMP" \
		-subj "/CN=$DOMAIN_NAME" \
		-addext "subjectAltName=DNS:$DOMAIN_NAME" >/dev/null 2>&1; then
		echo "Unable to generate the TLS certificate" >&2
		return 1
	fi

	chmod 600 "$KEY_TMP"
	chmod 644 "$CERT_TMP"
	mv "$KEY_TMP" "$KEY_PATH"
	mv "$CERT_TMP" "$CERT_PATH"
	KEY_TMP=
	CERT_TMP=
}

render_config() {
	if [ ! -f "$CONFIG_TEMPLATE" ] || [ ! -r "$CONFIG_TEMPLATE" ]; then
		echo "NGINX configuration template is missing or unreadable" >&2
		return 1
	fi

	CONFIG_TMP=$(mktemp /etc/nginx/nginx.conf.XXXXXX)
	if ! sed "s/__DOMAIN_NAME__/$DOMAIN_NAME/g" "$CONFIG_TEMPLATE" > "$CONFIG_TMP"; then
		echo "Unable to render the NGINX configuration" >&2
		return 1
	fi
	chmod 644 "$CONFIG_TMP"
	mv "$CONFIG_TMP" "$CONFIG_PATH"
	CONFIG_TMP=
}

cleanup() {
	status=$?
	if [ -n "$CERT_TMP" ]; then
		rm -f "$CERT_TMP" || true
	fi
	if [ -n "$KEY_TMP" ]; then
		rm -f "$KEY_TMP" || true
	fi
	if [ -n "$CONFIG_TMP" ]; then
		rm -f "$CONFIG_TMP" || true
	fi
	trap - EXIT
	exit "$status"
}

trap cleanup EXIT

validate_domain || fail "NGINX environment validation failed"
generate_certificate || fail "TLS certificate setup failed"
render_config || fail "NGINX configuration setup failed"
nginx -t -c "$CONFIG_PATH" || fail "NGINX configuration test failed"

if [ "$#" -eq 0 ]; then
	set -- nginx -g 'daemon off;'
fi
exec "$@"
