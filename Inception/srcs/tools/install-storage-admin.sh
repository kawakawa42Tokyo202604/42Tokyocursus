#!/bin/sh
# VM の初回設定でストレージ管理ヘルパーと sudo 権限を導入する（通常の Make 操作では実行しない）。

set -eu
PROJECT_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
[ "$(id -u)" -eq 0 ] || { echo 'Run this installer as the VM administrator' >&2; exit 1; }
[ -x /usr/bin/sudo ] && [ -x /usr/sbin/visudo ] \
	|| { echo 'Install sudo before configuring the VM helper' >&2; exit 1; }
policy=$(mktemp /etc/sudoers.d/inception-storage.XXXXXX)
trap 'rm -f -- "$policy"' EXIT
trap 'exit 1' HUP INT TERM
cp "$PROJECT_ROOT/srcs/docker/inception-storage.sudoers" "$policy"
/usr/sbin/visudo -cf "$policy"
install -o root -g root -m 755 "$PROJECT_ROOT/srcs/docker/inception-storage-admin.sh" \
	/usr/local/sbin/inception-storage-admin
install -o root -g root -m 440 "$policy" /etc/sudoers.d/inception-storage
/usr/sbin/visudo -c
printf '%s\n' 'VM storage permissions installed. takawaka can run make and make fclean.'
