#!/bin/sh
# バックアップの実行間隔と保持数を検証し、cron の定期実行設定を作成して起動する。

set -eu

BACKUP_SCHEDULE=${BACKUP_SCHEDULE:-'0 3 * * *'}
BACKUP_RETENTION=${BACKUP_RETENTION:-7}
CRON_FILE=/etc/cron.d/inception-backup

case "${MYSQL_DATABASE:-}" in
	''|*[!A-Za-z0-9_]*)
		echo "MYSQL_DATABASE contains an invalid character" >&2
		exit 1
		;;
esac
case "${MYSQL_USER:-}" in
	''|*[!A-Za-z0-9_]*)
		echo "MYSQL_USER contains an invalid character" >&2
		exit 1
		;;
esac

if printf '%s' "$BACKUP_SCHEDULE" | LC_ALL=C grep -q '[[:cntrl:]]'; then
	echo "BACKUP_SCHEDULE contains a control character" >&2
	exit 1
fi

field_count=$(printf '%s\n' "$BACKUP_SCHEDULE" | wc -w)
if [ "$field_count" -ne 5 ]; then
	echo "BACKUP_SCHEDULE must contain five cron fields" >&2
	exit 1
fi

CRON_VALIDATION_FILE=$(mktemp /tmp/inception-backup-cron.XXXXXX)
if ! printf '%s /usr/local/bin/backup.sh now\n' "$BACKUP_SCHEDULE" \
	> "$CRON_VALIDATION_FILE" \
	|| ! crontab -n "$CRON_VALIDATION_FILE"; then
	rm -f "$CRON_VALIDATION_FILE"
	echo "BACKUP_SCHEDULE is not a valid cron expression" >&2
	exit 1
fi
rm -f "$CRON_VALIDATION_FILE"

case "$BACKUP_RETENTION" in
	''|*[!0-9]*)
		echo "BACKUP_RETENTION must be a positive integer" >&2
		exit 1
		;;
esac
if [ "$BACKUP_RETENTION" -lt 1 ] || [ "$BACKUP_RETENTION" -gt 1000 ]; then
	echo "BACKUP_RETENTION must be between 1 and 1000" >&2
	exit 1
fi

{
	printf 'MYSQL_DATABASE=%s\n' "$MYSQL_DATABASE"
	printf 'MYSQL_USER=%s\n' "$MYSQL_USER"
	printf 'BACKUP_RETENTION=%s\n' "$BACKUP_RETENTION"
	printf '%s root /usr/local/bin/backup.sh now >> /proc/1/fd/1 2>> /proc/1/fd/2\n' \
		"$BACKUP_SCHEDULE"
} > "$CRON_FILE"
chmod 644 "$CRON_FILE"

if [ "$#" -eq 0 ]; then
	set -- cron -f
fi

exec "$@"
