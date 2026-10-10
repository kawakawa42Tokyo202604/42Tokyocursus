#!/bin/sh

# Exercise recovery decisions without touching Docker, systemd, or host storage.
set -eu

SCRIPT=$(CDPATH= cd -- "$(dirname -- "$0")/../tools" && pwd)/prepare-storage.sh
TEST_ROOT=$(mktemp -d "${TMPDIR:-/tmp}/inception-storage-test.XXXXXX")
trap 'rm -rf -- "$TEST_ROOT"' EXIT
trap 'exit 1' HUP INT TERM
mkdir "$TEST_ROOT/bin"
STORAGE_TEST_READLINK=$(command -v readlink)
STORAGE_TEST_MAKE=$(command -v make)
export STORAGE_TEST_READLINK

cat > "$TEST_ROOT/bin/docker" <<'MOCK'
#!/bin/sh
set -eu
printf 'docker %s\n' "$*" >> "$STORAGE_TEST_TRACE"
case "$1" in
	info)
		[ "${MOCK_INFO_FAILURE:-0}" = 0 ] || exit 1
		if [ -f "$STORAGE_TEST_RESTART" ]; then
			[ "${MOCK_POST_INFO_FAILURE:-0}" = 0 ] || exit 1
			printf '%s\n' "${MOCK_POST_ROOT:-$DATA_DIR/docker}"
		else
			printf '%s\n' "${MOCK_ROOT:-$DATA_DIR/docker}"
		fi ;;
	context)
		[ "${MOCK_CONTEXT_FAILURE:-0}" = 0 ] || exit 1
		printf '%s\n' "${MOCK_ENDPOINT:-unix:///var/run/docker.sock}" ;;
	ps)
		[ "${MOCK_PS_FAILURE:-0}" = 0 ] || exit 1
		printf '%s' "${MOCK_CONTAINERS:-}" ;;
	compose)
		case "$*" in
			*' config --services') printf '%s\n' mariadb wordpress ;;
		esac ;;
	*) exit 1 ;;
esac
MOCK

cat > "$TEST_ROOT/bin/id" <<'MOCK'
#!/bin/sh
[ "$*" = '-u' ] || exit 1
printf '%s\n' "${MOCK_UID:-0}"
MOCK

cat > "$TEST_ROOT/bin/readlink" <<'MOCK'
#!/bin/sh
if [ "$1" = -m ]; then
	exec "$STORAGE_TEST_READLINK" "$@"
fi
[ "${MOCK_EXE_FAILURE:-0}" = 0 ] || exit 1
printf '%s\n' "${MOCK_EXE:-/usr/bin/dockerd}"
MOCK

cat > "$TEST_ROOT/bin/systemctl" <<'MOCK'
#!/bin/sh
set -eu
printf 'systemctl %s\n' "$*" >> "$STORAGE_TEST_TRACE"
case "$*" in
	'is-active --quiet docker.service') [ "${MOCK_ACTIVE:-1}" = 1 ] ;;
	'show --property=MainPID --value docker.service')
		printf '%s\n' "${MOCK_PID-1234}" ;;
	'restart docker.service')
		: > "$STORAGE_TEST_RESTART"
		[ "${MOCK_RESTART_FAILURE:-0}" = 0 ] || exit 1
		[ "${MOCK_RECREATE:-1}" = 0 ] || mkdir -p "$DATA_DIR/docker" ;;
	*) exit 1 ;;
esac
MOCK

cat > "$TEST_ROOT/bin/sudo" <<'MOCK'
#!/bin/sh
set -eu
printf 'sudo %s\n' "$*" >> "$STORAGE_TEST_TRACE"
[ "$*" = '-n /usr/local/sbin/inception-storage-admin recover' ] || exit 1
[ "${MOCK_SUDO_ALLOWED:-0}" = 1 ] || exit 1
: > "$STORAGE_TEST_RESTART"
mkdir -p "$DATA_DIR/docker"
MOCK

mkdir -p "$TEST_ROOT/project/srcs/tools"
cp "$(dirname "$SCRIPT")/../../Makefile" "$TEST_ROOT/project/Makefile"
cp "$SCRIPT" "$TEST_ROOT/project/srcs/tools/prepare-storage.sh"
cat > "$TEST_ROOT/project/srcs/tools/check-config.sh" <<'MOCK'
#!/bin/sh
printf 'check-config\n' >> "$STORAGE_TEST_TRACE"
[ -d "$DATA_DIR/docker" ] && [ "${MOCK_CONFIG_FAILURE:-0}" = 0 ]
MOCK
chmod 755 "$TEST_ROOT/project/srcs/tools/"*.sh

chmod 755 "$TEST_ROOT/bin/"*
PATH=$TEST_ROOT/bin:$PATH
export PATH
unset DOCKER_HOST DOCKER_CONTEXT
count=0

run_case() {
	name=$1
	expect_success=$2
	expect_restart=$3
	shift 3
	DATA_DIR=$TEST_ROOT/$name/data
	STORAGE_TEST_TRACE=$TEST_ROOT/$name/trace
	STORAGE_TEST_RESTART=$TEST_ROOT/$name/restart
	export DATA_DIR STORAGE_TEST_TRACE STORAGE_TEST_RESTART
	mkdir -p "$DATA_DIR"
	: > "$STORAGE_TEST_TRACE"
	case "$name" in
		healthy*|make_existing_storage) mkdir "$DATA_DIR/docker"; : > "$DATA_DIR/docker/preserved" ;;
		root_is_file) : > "$DATA_DIR/docker" ;;
		broken_symlink) ln -s "$TEST_ROOT/missing" "$DATA_DIR/docker" ;;
	esac
	status=0
	case "$name" in
		make_*)
			env "$@" "$STORAGE_TEST_MAKE" -s -C "$TEST_ROOT/project" \
				> "$TEST_ROOT/$name/output" 2>&1 || status=$? ;;
		*) env "$@" "$SCRIPT" > "$TEST_ROOT/$name/output" 2>&1 || status=$? ;;
	esac
	restarted=0
	[ ! -f "$STORAGE_TEST_RESTART" ] || restarted=1
	passed=0
	if [ "$expect_success" = yes ] && [ "$status" -eq 0 ]; then
		passed=1
	elif [ "$expect_success" = no ] && [ "$status" -ne 0 ]; then
		passed=1
	fi
	if [ "$passed" -ne 1 ] || [ "$restarted" -ne "$expect_restart" ]; then
		printf 'FAIL: %s (exit=%s restart=%s)\n' "$name" "$status" "$restarted" >&2
		cat "$TEST_ROOT/$name/output" "$STORAGE_TEST_TRACE" >&2
		exit 1
	fi
	case "$name" in
		healthy*)
			[ -f "$DATA_DIR/docker/preserved" ] && [ ! -s "$STORAGE_TEST_TRACE" ] ;;
	esac
	if [ "$expect_success" = yes ] && [ "$expect_restart" -eq 1 ]; then
		[ -d "$DATA_DIR/docker" ]
	fi
	case "$name" in
		make_*)
			if [ "$expect_success" = yes ]; then
				[ "$(tail -n 5 "$STORAGE_TEST_TRACE")" = "$(printf '%s\n' \
					'check-config' \
					'docker compose -f srcs/docker-compose.yml --env-file srcs/.env config --services' \
					'docker compose -f srcs/docker-compose.yml --env-file srcs/.env build mariadb' \
					'docker compose -f srcs/docker-compose.yml --env-file srcs/.env build wordpress' \
					'docker compose -f srcs/docker-compose.yml --env-file srcs/.env up -d --no-build')" ]
			else
				! grep -q '^docker compose ' "$STORAGE_TEST_TRACE"
			fi ;;
	esac
	count=$((count + 1))
	printf 'ok %s - %s\n' "$count" "$name"
}

run_case healthy yes 0
run_case healthy_unprivileged yes 0 MOCK_UID=1000 MOCK_INFO_FAILURE=1
run_case removed_storage yes 1
run_case local_host yes 1 DOCKER_HOST=unix:///run/docker.sock
run_case context_overrides_host yes 1 DOCKER_CONTEXT=default DOCKER_HOST=tcp://remote:2375
run_case relative_path no 0 DATA_DIR=relative
run_case root_is_file no 0
run_case broken_symlink no 0
run_case wrong_daemon_root no 0 MOCK_ROOT=/var/lib/docker
run_case non_root_user no 0 MOCK_UID=1000
run_case non_root_authorized yes 1 MOCK_UID=1000 MOCK_SUDO_ALLOWED=1
run_case make_non_root_authorized yes 1 MOCK_UID=1000 MOCK_SUDO_ALLOWED=1
run_case non_root_remote_host no 0 MOCK_UID=1000 MOCK_SUDO_ALLOWED=1 DOCKER_HOST=tcp://remote:2375
run_case remote_host no 0 DOCKER_HOST=tcp://remote:2375
run_case remote_context no 0 DOCKER_CONTEXT=remote MOCK_ENDPOINT=ssh://remote
run_case rootless_socket no 0 MOCK_ENDPOINT=unix:///run/user/1000/docker.sock
run_case context_unavailable no 0 MOCK_CONTEXT_FAILURE=1
run_case docker_unavailable no 0 MOCK_INFO_FAILURE=1
run_case containers_remain no 0 MOCK_CONTAINERS=container-id
run_case container_list_unavailable no 0 MOCK_PS_FAILURE=1
run_case system_daemon_inactive no 0 MOCK_ACTIVE=0
run_case missing_daemon_pid no 0 MOCK_PID=
run_case zero_daemon_pid no 0 MOCK_PID=0
run_case invalid_daemon_pid no 0 MOCK_PID=invalid
run_case daemon_not_dockerd no 0 MOCK_EXE=/usr/bin/other-daemon
run_case daemon_exe_unavailable no 0 MOCK_EXE_FAILURE=1
run_case restart_failed no 1 MOCK_RESTART_FAILURE=1
run_case api_failed_after_restart no 1 MOCK_POST_INFO_FAILURE=1
run_case wrong_root_after_restart no 1 MOCK_POST_ROOT=/var/lib/docker
run_case storage_not_recreated no 1 MOCK_RECREATE=0
run_case make_recovers_before_build yes 1
run_case make_existing_storage yes 0
run_case make_refuses_remaining_containers no 0 MOCK_CONTAINERS=container-id
run_case make_stops_on_invalid_config no 1 MOCK_CONFIG_FAILURE=1
printf 'All %s storage recovery checks passed.\n' "$count"
