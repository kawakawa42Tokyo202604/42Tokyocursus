#!/bin/sh

# Install a root-owned copy. Never authorize an editable repository script.
set -eu
PATH=/usr/sbin:/usr/bin:/sbin:/bin
export PATH
unset ENV BASH_ENV CDPATH DOCKER_CONTEXT DOCKER_CONFIG DOCKER_TLS_VERIFY DOCKER_CERT_PATH
DATA_DIR=/home/takawaka/data
DOCKER_HOST=unix:///var/run/docker.sock
export DOCKER_HOST

fail() {
	echo "$1" >&2
	exit 1
}

[ "$#" -eq 1 ] || fail "Expected check, clean-data or recover"
case "$1" in check|clean-data|recover) ;; *) fail "Unknown storage operation" ;; esac
[ "$(id -u)" -eq 0 ] || fail "This installed helper must run through sudo"
[ "$(readlink -m -- "$DATA_DIR")" = "$DATA_DIR" ] \
	|| fail "The VM data path must not contain symlinks"
expected_root=$DATA_DIR/docker
[ "$(readlink -m -- "$expected_root")" = "$expected_root" ] \
	|| fail "The Docker storage path must not contain symlinks"
actual_root=$(docker info --format '{{.DockerRootDir}}')
[ "$actual_root" = "$expected_root" ] || fail "The local Docker data-root does not match this VM"

umask 077
exec 9>/run/inception-storage-admin.lock
flock -n 9 || fail "Another VM storage operation is running"

case "$1" in
	check) exit 0 ;;
	clean-data)
		remaining=$(docker ps -aq)
		[ -z "$remaining" ] || fail "Containers remain; refusing data deletion"
		remaining=$(docker images -qa)
		[ -z "$remaining" ] || fail "Images remain; refusing data deletion"
		remaining=$(docker volume ls -q)
		[ -z "$remaining" ] || fail "Volumes remain; refusing data deletion"
		remaining=$(docker network ls --filter type=custom -q)
		[ -z "$remaining" ] || fail "Custom networks remain; refusing data deletion"
		project_root=$(pwd -P)
		for path in "$project_root" "$project_root/srcs/.env" \
			"$project_root/secrets" "$project_root"/secrets/*.txt; do
			canonical_path=$(readlink -m -- "$path")
			case "$canonical_path/" in
				"$DATA_DIR/"*) fail "Keep the source and configuration outside the VM data directory" ;;
			esac
		done
		mounts=$(findmnt -rn -o TARGET)
		for mount in $mounts; do
			case "$mount" in "$DATA_DIR/"*) fail "A mount remains under VM data" ;; esac
		done
		[ -e "$DATA_DIR" ] || exit 0
		cd -- "$DATA_DIR"
		[ "$(pwd -P)" = "$DATA_DIR" ] || fail "The VM data directory changed"
		# Relative paths keep removal inside this directory even if its parent is renamed.
		rm -rf --one-file-system -- ./* ;;
	recover)
		[ ! -d "$expected_root" ] || exit 0
		[ ! -e "$expected_root" ] && [ ! -L "$expected_root" ] \
			|| fail "Docker data-root is not a directory"
		remaining=$(docker ps -aq)
		[ -z "$remaining" ] || fail "Containers remain; refusing Docker storage recovery"
		systemctl is-active --quiet docker.service \
			|| fail "The system Docker service is not active"
		daemon_pid=$(systemctl show --property=MainPID --value docker.service)
		case "$daemon_pid" in ''|0|*[!0-9]*) fail "Cannot identify dockerd" ;; esac
		daemon_exe=$(readlink "/proc/$daemon_pid/exe")
		[ "${daemon_exe##*/}" = dockerd ] || fail "The service is not dockerd"
		printf '%s\n' 'Docker storage was removed; restarting the empty system Docker daemon.'
		systemctl restart docker.service
		actual_root=$(docker info --format '{{.DockerRootDir}}')
		[ "$actual_root" = "$expected_root" ] && [ -d "$expected_root" ] \
			|| fail "Docker storage recovery failed"
		printf '%s\n' 'Docker storage recovered; continuing the normal Compose build.' ;;
esac
