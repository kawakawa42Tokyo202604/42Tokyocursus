#!/bin/sh
# 実 Docker や実データを操作せず、fclean の安全条件と操作順序をモックで検証する。

set -eu

PROJECT_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
TEST_ROOT=$(mktemp -d "${TMPDIR:-/tmp}/inception-fclean-test.XXXXXX")
FCLEAN_TEST_RM=$(command -v rm)
FCLEAN_TEST_READLINK=$(command -v readlink)
FCLEAN_TEST_MAKE=$(command -v make)
export FCLEAN_TEST_READLINK
trap '"$FCLEAN_TEST_RM" -rf -- "$TEST_ROOT"' EXIT
trap 'exit 1' HUP INT TERM
mkdir -p "$TEST_ROOT/bin" "$TEST_ROOT/project/srcs/tools"
cp "$PROJECT_ROOT/Makefile" "$TEST_ROOT/project/Makefile"
cp "$PROJECT_ROOT/srcs/tools/fclean.sh" "$TEST_ROOT/project/srcs/tools/fclean.sh"

cat > "$TEST_ROOT/bin/id" <<'MOCK'
#!/bin/sh
[ "$*" = '-u' ] || exit 1
printf '%s\n' "${MOCK_UID:-0}"
MOCK

cat > "$TEST_ROOT/bin/readlink" <<'MOCK'
#!/bin/sh
set -eu
case "$*" in
	"-m -- $DATA_DIR")
		if [ "${MOCK_DATA_SYMLINK:-0}" = 1 ]; then
			printf '%s\n' /elsewhere/data
			exit 0
		fi ;;
	"-m -- $DATA_DIR/docker")
		if [ "${MOCK_ROOT_SYMLINK:-0}" = 1 ]; then
			printf '%s\n' /elsewhere/docker
			exit 0
		fi ;;
	*/srcs/.env)
		if [ "${MOCK_ENV_IN_DATA:-0}" = 1 ]; then
			printf '%s\n' "$DATA_DIR/environment"
			exit 0
		fi ;;
	*/secrets)
		if [ "${MOCK_SECRETS_IN_DATA:-0}" = 1 ]; then
			printf '%s\n' "$DATA_DIR/secrets"
			exit 0
		fi ;;
esac
exec "$FCLEAN_TEST_READLINK" "$@"
MOCK

cat > "$TEST_ROOT/bin/docker" <<'MOCK'
#!/bin/sh
set -eu
printf 'docker %s\n' "$*" >> "$FCLEAN_TEST_TRACE"
action=$1
case "$1" in volume|network) action="$1 $2" ;; esac
[ "${MOCK_FAILURE:-}" != "$action" ] || exit 1
case "$action" in
	context) printf '%s\n' "${MOCK_ENDPOINT:-unix:///var/run/docker.sock}" ;;
	info) printf '%s\n' "${MOCK_ROOT:-$DATA_DIR/docker}" ;;
	ps|images|'volume ls'|'network ls')
		case "$action" in
			ps) kind=containers; values=$(printf '%s\n' container-a container-b) ;;
			images) kind=images; values=$(printf '%s\n' image-a image-a image-b) ;;
			'volume ls') kind=volumes; values=$(printf '%s\n' db-volume wp-volume backup-volume) ;;
			'network ls')
				[ "$*" = 'network ls --filter type=custom -q' ] || exit 1
				kind=networks; values=$(printf '%s\n' app-network restore-network) ;;
		esac
		if [ -f "$FCLEAN_TEST_STATE/$kind" ]; then
			[ "${MOCK_POST_QUERY_FAILURE:-}" != "$kind" ] || exit 1
			[ "${MOCK_REMAINING:-}" = "$kind" ] || exit 0
		elif [ "${MOCK_EMPTY:-0}" = 1 ]; then
			exit 0
		fi
		printf '%s\n' "$values" ;;
	stop) ;;
	rm) : > "$FCLEAN_TEST_STATE/containers" ;;
	rmi) : > "$FCLEAN_TEST_STATE/images" ;;
	'volume rm') : > "$FCLEAN_TEST_STATE/volumes" ;;
	'network rm') : > "$FCLEAN_TEST_STATE/networks" ;;
	*) exit 1 ;;
esac
MOCK

cat > "$TEST_ROOT/bin/findmnt" <<'MOCK'
#!/bin/sh
[ "$*" = '-rn -o TARGET' ] || exit 1
[ "${MOCK_MOUNT_FAILURE:-0}" = 0 ] || exit 1
if [ "${MOCK_MOUNT_REMAINS:-0}" = 1 ]; then
	printf '%s\n' "$DATA_DIR/docker/mounted"
else
	printf '%s\n' / /proc /run
fi
MOCK

cat > "$TEST_ROOT/bin/rm" <<'MOCK'
#!/bin/sh
# Do not forward any argument to real rm, even if the production guards fail.
printf 'rm %s\n' "$*" >> "$FCLEAN_TEST_TRACE"
[ "$*" = '-rf -- /home/inception-fclean-test/data/*' ] || exit 1
MOCK

cat > "$TEST_ROOT/bin/sudo" <<'MOCK'
#!/bin/sh
set -eu
printf 'sudo %s\n' "$*" >> "$FCLEAN_TEST_TRACE"
[ "${MOCK_SUDO_ALLOWED:-0}" = 1 ] || exit 1
case "$*" in
	'-n /usr/local/sbin/inception-storage-admin check') ;;
	'-n /usr/local/sbin/inception-storage-admin clean-data')
		[ "${MOCK_SUDO_CLEAN_FAILURE:-0}" = 0 ] || exit 1
		printf 'rm -rf -- /home/inception-fclean-test/data/*\n' >> "$FCLEAN_TEST_TRACE" ;;
	*) exit 1 ;;
esac
MOCK

chmod 755 "$TEST_ROOT/bin/"*
PATH=$TEST_ROOT/bin:$PATH
export PATH
unset DOCKER_HOST DOCKER_CONTEXT
DATA_DIR=/home/inception-fclean-test/data
export DATA_DIR
count=0

run_case() {
	name=$1
	expected=$2
	shift 2
	FCLEAN_TEST_STATE=$TEST_ROOT/$name
	FCLEAN_TEST_TRACE=$FCLEAN_TEST_STATE/trace
	export FCLEAN_TEST_STATE FCLEAN_TEST_TRACE
	mkdir "$FCLEAN_TEST_STATE"
	: > "$FCLEAN_TEST_TRACE"
	status=0
	case "$name" in
		make_*) env "$@" "$FCLEAN_TEST_MAKE" -s -C "$TEST_ROOT/project" \
			DATA_DIR="$DATA_DIR" fclean > "$FCLEAN_TEST_STATE/output" 2>&1 || status=$? ;;
		*) env "$@" sh "$TEST_ROOT/project/srcs/tools/fclean.sh" \
			> "$FCLEAN_TEST_STATE/output" 2>&1 || status=$? ;;
	esac
	passed=0
	case "$expected" in
		success)
			if [ "$status" -eq 0 ] \
				&& [ "$(tail -n 1 "$FCLEAN_TEST_TRACE")" = 'rm -rf -- /home/inception-fclean-test/data/*' ]; then
				passed=1
			fi ;;
		guard)
			if [ "$status" -ne 0 ] \
				&& ! grep -Eq '^(rm |docker (stop |rm |rmi |volume rm |network rm ))' "$FCLEAN_TEST_TRACE"; then
				passed=1
			fi ;;
		failure)
			if [ "$status" -ne 0 ] && ! grep -q '^rm ' "$FCLEAN_TEST_TRACE"; then
				passed=1
			fi ;;
	esac
	if [ "$passed" -ne 1 ]; then
		printf 'FAIL: %s (exit=%s)\n' "$name" "$status" >&2
		cat "$FCLEAN_TEST_STATE/output" "$FCLEAN_TEST_TRACE" >&2
		exit 1
	fi
	case "$name" in
		complete|make_fclean)
			grep '^docker \(stop\|rm\|rmi\|volume rm\|network rm\) ' "$FCLEAN_TEST_TRACE" \
				> "$FCLEAN_TEST_STATE/mutations"
			cat > "$FCLEAN_TEST_STATE/expected" <<'EXPECTED'
docker stop container-a container-b
docker rm container-a container-b
docker rmi -f image-a image-b
docker volume rm db-volume wp-volume backup-volume
docker network rm app-network restore-network
EXPECTED
			cmp "$FCLEAN_TEST_STATE/expected" "$FCLEAN_TEST_STATE/mutations" ;;
		empty)
			! grep -Eq '^docker (stop |rm |rmi |volume rm |network rm )' "$FCLEAN_TEST_TRACE" ;;
	esac
	count=$((count + 1))
	printf 'ok %s - %s\n' "$count" "$name"
}

run_case complete success
run_case empty success MOCK_EMPTY=1
run_case local_host success DOCKER_HOST=unix:///run/docker.sock
run_case context_overrides_host success DOCKER_CONTEXT=default DOCKER_HOST=tcp://remote:2375
run_case non_root guard MOCK_UID=1000
run_case non_root_authorized success MOCK_UID=1000 MOCK_SUDO_ALLOWED=1
run_case non_root_cleanup_failed failure MOCK_UID=1000 MOCK_SUDO_ALLOWED=1 MOCK_SUDO_CLEAN_FAILURE=1
run_case invalid_path guard DATA_DIR=/
run_case relative_path guard DATA_DIR=relative
run_case traversal_path guard DATA_DIR=/home/../data
run_case nested_path guard DATA_DIR=/home/some/path/data
run_case data_symlink guard MOCK_DATA_SYMLINK=1
run_case root_symlink guard MOCK_ROOT_SYMLINK=1
run_case environment_in_data guard MOCK_ENV_IN_DATA=1
run_case secrets_in_data guard MOCK_SECRETS_IN_DATA=1
run_case wrong_daemon guard MOCK_ROOT=/var/lib/docker
run_case remote_host guard DOCKER_HOST=tcp://remote:2375
run_case remote_context guard DOCKER_CONTEXT=remote MOCK_ENDPOINT=ssh://remote
run_case rootless_socket guard MOCK_ENDPOINT=unix:///run/user/1000/docker.sock
run_case context_unavailable guard MOCK_FAILURE=context
run_case docker_unavailable guard MOCK_FAILURE=info
run_case container_list_unavailable guard MOCK_FAILURE=ps
run_case stop_failed failure MOCK_FAILURE=stop
run_case container_removal_failed failure MOCK_FAILURE=rm
run_case image_list_failed failure MOCK_FAILURE=images
run_case image_removal_failed failure MOCK_FAILURE=rmi
run_case volume_removal_failed failure 'MOCK_FAILURE=volume rm'
run_case network_removal_failed failure 'MOCK_FAILURE=network rm'
run_case containers_remain failure MOCK_REMAINING=containers
run_case images_remain failure MOCK_REMAINING=images
run_case volumes_remain failure MOCK_REMAINING=volumes
run_case networks_remain failure MOCK_REMAINING=networks
run_case verification_query_failed failure MOCK_POST_QUERY_FAILURE=containers
run_case mount_remains failure MOCK_MOUNT_REMAINS=1
run_case mount_check_failed failure MOCK_MOUNT_FAILURE=1
run_case make_fclean success
run_case make_non_root guard MOCK_UID=1000
run_case make_non_root_authorized success MOCK_UID=1000 MOCK_SUDO_ALLOWED=1
printf 'All %s full cleanup checks passed.\n' "$count"
