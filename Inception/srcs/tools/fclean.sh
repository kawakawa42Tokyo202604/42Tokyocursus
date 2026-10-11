#!/bin/sh
# 安全条件を確認し、評価用リセットとして Docker リソース全体と VM のデータを削除する。

set -eu

PROJECT_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd -P)
DATA_DIR=${DATA_DIR:-/home/takawaka/data}

fail() {
	echo "$1" >&2
	exit 1
}

needs_admin=0
if [ "$(id -u)" -ne 0 ]; then
	command -v sudo >/dev/null \
		|| fail "Install the VM storage administrator helper; see DEV_DOC.md"
	needs_admin=1
fi
case "$DATA_DIR" in
	/home/*/data) ;;
	*) fail "Full cleanup only supports /home/<login>/data" ;;
esac
data_login=${DATA_DIR#/home/}
data_login=${data_login%/data}
case "$data_login" in
	''|.|..|*[!a-zA-Z0-9_.-]*) fail "Invalid login in DATA_DIR" ;;
esac
[ "$(readlink -m -- "$DATA_DIR")" = "$DATA_DIR" ] \
	|| fail "DATA_DIR must be a canonical path without symlinks"
expected_root=$DATA_DIR/docker
[ "$(readlink -m -- "$expected_root")" = "$expected_root" ] \
	|| fail "Docker data-root must not be a symlink"

for path in "$PROJECT_ROOT" "$PROJECT_ROOT/srcs/.env" \
	"$PROJECT_ROOT/secrets" "$PROJECT_ROOT"/secrets/*.txt; do
	canonical_path=$(readlink -m -- "$path")
	case "$canonical_path/" in
		"$DATA_DIR/"*) fail "Keep the repository, environment and secrets outside DATA_DIR" ;;
	esac
done

if [ -n "${DOCKER_CONTEXT:-}" ]; then
	endpoint=$(docker context inspect "$DOCKER_CONTEXT" \
		--format '{{.Endpoints.docker.Host}}')
elif [ -n "${DOCKER_HOST:-}" ]; then
	endpoint=$DOCKER_HOST
else
	endpoint=$(docker context inspect --format '{{.Endpoints.docker.Host}}')
fi
case "$endpoint" in
	unix:///var/run/docker.sock|unix:///run/docker.sock) ;;
	*) fail "Full cleanup requires the VM's local system Docker socket" ;;
esac
actual_root=$(docker info --format '{{.DockerRootDir}}')
[ "$actual_root" = "$expected_root" ] \
	|| fail "Docker data-root must be $expected_root; refusing full cleanup"
command -v findmnt >/dev/null || fail "findmnt is required to check remaining mounts"
if [ "$needs_admin" -eq 1 ]; then
	sudo -n /usr/local/sbin/inception-storage-admin check </dev/null \
		|| fail "VM storage administrator permission is missing; see DEV_DOC.md"
fi

printf 'Full cleanup deletes all Docker resources and %s/*, including site data and backups.\n' "$DATA_DIR"

# Docker IDs and volume names do not contain whitespace. Split their lists only.
containers=$(docker ps -aq)
if [ -n "$containers" ]; then
	docker stop $containers
	docker rm $containers
fi
images=$(docker images -qa)
if [ -n "$images" ]; then
	images=$(printf '%s\n' "$images" | LC_ALL=C sort -u)
	docker rmi -f $images
fi
volumes=$(docker volume ls -q)
if [ -n "$volumes" ]; then
	docker volume rm $volumes
fi
networks=$(docker network ls --filter type=custom -q)
if [ -n "$networks" ]; then
	docker network rm $networks
fi

remaining=$(docker ps -aq)
[ -z "$remaining" ] || fail "Containers remain; refusing to delete DATA_DIR"
remaining=$(docker images -qa)
[ -z "$remaining" ] || fail "Images remain; refusing to delete DATA_DIR"
remaining=$(docker volume ls -q)
[ -z "$remaining" ] || fail "Volumes remain; refusing to delete DATA_DIR"
remaining=$(docker network ls --filter type=custom -q)
[ -z "$remaining" ] || fail "Custom networks remain; refusing to delete DATA_DIR"

# Container removal should release Docker mounts before deleting its storage.
mounts=$(findmnt -rn -o TARGET)
for mount in $mounts; do
	case "$mount" in
		"$DATA_DIR/"*) fail "A mount remains below DATA_DIR; refusing to delete it" ;;
	esac
done

# Match the evaluation sheet's /home/<login>/data/* scope (non-hidden entries).
if [ "$needs_admin" -eq 1 ]; then
	sudo -n /usr/local/sbin/inception-storage-admin clean-data </dev/null
else
	rm -rf -- "$DATA_DIR"/*
fi
printf '%s\n' 'Full cleanup completed. Run make to rebuild the stack.'
