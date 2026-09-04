#!/bin/sh

set -eu

BACKUP_SCHEDULE=${BACKUP_SCHEDULE:-'0 3 * * *'}
BACKUP_RETENTION=${BACKUP_RETENTION:-7}
CRON_FILE=/etc/cron.d/inception-backup

if printf '%s' "$BACKUP_SCHEDULE" | LC_ALL=C grep -q '[[:cntrl:]]'; then
	echo "BACKUP_SCHEDULE contains a control character" >&2
	exit 1
fi

field_count=$(printf '%s\n' "$BACKUP_SCHEDULE" | wc -w)
if [ "$field_count" -ne 5 ]; then
	echo "BACKUP_SCHEDULE must contain five cron fields" >&2
	exit 1
fi

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

printf '%s root /usr/local/bin/backup.sh now >> /proc/1/fd/1 2>> /proc/1/fd/2\n' \
	"$BACKUP_SCHEDULE" > "$CRON_FILE"
chmod 644 "$CRON_FILE"

if [ "$#" -eq 0 ]; then
	set -- cron -f
fi

exec "$@"
