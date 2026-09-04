#!/bin/sh

set -eu

WEB_ROOT=/var/www/html
SECRET_PATH=/run/secrets/ftp_password
CONFIG_TEMPLATE=/etc/vsftpd/vsftpd.conf.template
CONFIG_PATH=/etc/vsftpd/vsftpd.conf
CONFIG_TMP=

fail() {
	echo "$1" >&2
	exit 1
}

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

render_config() {
	if [ ! -f "$CONFIG_TEMPLATE" ] || [ ! -r "$CONFIG_TEMPLATE" ]; then
		return 1
	fi
	CONFIG_TMP=$(mktemp /etc/vsftpd/vsftpd.conf.XXXXXX)
	if ! sed "s/__FTP_PASV_ADDRESS__/$FTP_PASV_ADDRESS/g" \
		"$CONFIG_TEMPLATE" > "$CONFIG_TMP"; then
		return 1
	fi
	chmod 644 "$CONFIG_TMP"
	mv "$CONFIG_TMP" "$CONFIG_PATH"
	CONFIG_TMP=
}

prepare_user() {
	case "$FTP_USER" in
		''|root|*[!A-Za-z0-9._-]*)
			echo "FTP_USER is missing or invalid" >&2
			return 1
			;;
	esac

	if ! getent passwd "$FTP_USER" >/dev/null 2>&1; then
		useradd --system --home-dir "$WEB_ROOT" --no-create-home \
			--shell /usr/sbin/nologin "$FTP_USER"
	elif [ "$(id -u "$FTP_USER")" -eq 0 ]; then
		echo "FTP_USER must not be root" >&2
		return 1
	fi

	if ! usermod --append --groups www-data "$FTP_USER"; then
		return 1
	fi

	if ! printf '%s:%s\n' "$FTP_USER" "$FTP_PASSWORD" | chpasswd; then
		return 1
	fi
}

prepare_permissions() {
	if [ ! -d "$WEB_ROOT" ]; then
		echo "WordPress volume is missing: $WEB_ROOT" >&2
		return 1
	fi

	find "$WEB_ROOT" -type d -exec setfacl -m "u:$FTP_USER:rwx" {} +
	find "$WEB_ROOT" -type d -exec setfacl -m "d:u:$FTP_USER:rwx" {} +
	find "$WEB_ROOT" -type f -exec setfacl -m "u:$FTP_USER:rw" {} +
	if [ -f "$WEB_ROOT/wp-config.php" ]; then
		setfacl -m "u:$FTP_USER:r--" "$WEB_ROOT/wp-config.php"
	fi
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

if [ -z "${FTP_USER:-}" ] || [ -z "${FTP_PASV_ADDRESS:-}" ]; then
	fail "FTP_USER and FTP_PASV_ADDRESS are required"
fi
case "$FTP_PASV_ADDRESS" in
	*[!0-9.]*|.*|*.)
		fail "FTP_PASV_ADDRESS must be an IPv4 address"
		;;
esac

FTP_PASSWORD=$(read_secret "$SECRET_PATH") || fail "FTP password validation failed"
prepare_user || fail "Unable to prepare the FTP user"
prepare_permissions || fail "Unable to prepare FTP permissions"
render_config || fail "Unable to render vsftpd configuration"
unset FTP_PASSWORD

if [ "$#" -eq 0 ]; then
	set -- vsftpd "$CONFIG_PATH"
fi
exec "$@"
