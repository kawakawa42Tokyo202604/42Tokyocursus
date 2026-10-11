#!/bin/sh
# Docker の保存先が削除された場合に復旧条件を確認し、デーモンの再起動で保存先を再作成する。

set -eu

DATA_DIR=${DATA_DIR:-/home/takawaka/data}

fail() {
	echo "$1" >&2
	exit 1
}

case "$DATA_DIR" in
	/*) ;;
	*) fail "DATA_DIR must be an absolute path" ;;
esac
expected_root=$(readlink -m "$DATA_DIR/docker")

# Ordinary builds and restarts never restart the Docker daemon.
[ ! -d "$expected_root" ] || exit 0
[ ! -e "$expected_root" ] && [ ! -L "$expected_root" ] \
	|| fail "Docker data-root is not a directory: $expected_root"

actual_root=$(docker info --format '{{.DockerRootDir}}') \
	|| fail "Cannot inspect Docker; start the VM's Docker service as administrator"
[ "$actual_root" = "$expected_root" ] \
	|| fail "Docker data-root must be $expected_root; follow DEV_DOC.md before starting"

# The evaluation removes containers first, then deletes DATA_DIR while dockerd
# is still running. Its deleted metadata must be reopened by restarting it.
if [ -n "${DOCKER_CONTEXT:-}" ]; then
	endpoint=$(docker context inspect "$DOCKER_CONTEXT" \
		--format '{{.Endpoints.docker.Host}}') || fail "Cannot inspect Docker context"
elif [ -n "${DOCKER_HOST:-}" ]; then
	endpoint=$DOCKER_HOST
else
	endpoint=$(docker context inspect \
		--format '{{.Endpoints.docker.Host}}') || fail "Cannot inspect Docker context"
fi
case "$endpoint" in
	unix:///var/run/docker.sock|unix:///run/docker.sock) ;;
	*) fail "Automatic recovery requires the VM's local system Docker socket" ;;
esac

containers=$(docker ps --all --quiet) \
	|| fail "Cannot verify that Docker has no containers; recover it manually"
[ -z "$containers" ] \
	|| fail "Docker storage is missing but containers remain; automatic recovery refused"
if [ "$(id -u)" -ne 0 ]; then
	command -v sudo >/dev/null \
		|| fail "Install the VM storage administrator helper; see DEV_DOC.md"
	sudo -n /usr/local/sbin/inception-storage-admin recover </dev/null \
		|| fail "Cannot recover Docker storage; configure the VM helper from DEV_DOC.md"
	actual_root=$(docker info --format '{{.DockerRootDir}}')
	[ "$actual_root" = "$expected_root" ] && [ -d "$expected_root" ] \
		|| fail "Docker did not recreate the expected storage directory"
	exit 0
fi
systemctl is-active --quiet docker.service \
	|| fail "The system Docker service is not active; recover it manually"
daemon_pid=$(systemctl show --property=MainPID --value docker.service)
case "$daemon_pid" in
	''|0|*[!0-9]*) fail "Cannot identify the system Docker daemon" ;;
esac
daemon_exe=$(readlink "/proc/$daemon_pid/exe") \
	|| fail "Cannot verify the system Docker daemon"
[ "${daemon_exe##*/}" = dockerd ] \
	|| fail "The system Docker service is not running dockerd"

printf '%s\n' 'Docker storage was removed; restarting the empty system Docker daemon.'
systemctl restart docker.service || fail "Unable to restart Docker after storage removal"
actual_root=$(docker info --format '{{.DockerRootDir}}') \
	|| fail "Docker did not become available after recovery"
[ "$actual_root" = "$expected_root" ] && [ -d "$expected_root" ] \
	|| fail "Docker did not recreate the expected storage directory"
printf '%s\n' 'Docker storage recovered; continuing the normal Compose build.'
