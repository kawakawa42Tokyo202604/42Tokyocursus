#!/bin/sh

# Test the root helper with command mocks and isolated storage/lock paths.
set -eu
PROJECT_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
TEST_ROOT=$(mktemp -d "${TMPDIR:-/tmp}/inception-admin-test.XXXXXX")
ADMIN_TEST_RM=$(command -v rm)
ADMIN_TEST_READLINK=$(command -v readlink)
export ADMIN_TEST_READLINK
trap '"$ADMIN_TEST_RM" -rf -- "$TEST_ROOT"' EXIT
trap 'exit 1' HUP INT TERM
mkdir "$TEST_ROOT/bin"
sed -e "s|^PATH=/usr/sbin:/usr/bin:/sbin:/bin$|PATH=$TEST_ROOT/bin:/usr/sbin:/usr/bin:/sbin:/bin|" \
	-e 's|^DATA_DIR=/home/takawaka/data$|DATA_DIR=$ADMIN_TEST_DATA|' \
	-e 's|9>/run/inception-storage-admin.lock|9>"$ADMIN_TEST_STATE/lock"|' \
	"$PROJECT_ROOT/srcs/docker/inception-storage-admin.sh" > "$TEST_ROOT/helper.sh"

cat > "$TEST_ROOT/bin/id" <<'MOCK'
#!/bin/sh
printf '%s\n' "${MOCK_UID:-0}"
MOCK
cat > "$TEST_ROOT/bin/readlink" <<'MOCK'
#!/bin/sh
case "$1" in
	-m) exec "$ADMIN_TEST_READLINK" "$@" ;;
	*) printf '%s\n' "${MOCK_DAEMON:-/usr/bin/dockerd}" ;;
esac
MOCK
cat > "$TEST_ROOT/bin/docker" <<'MOCK'
#!/bin/sh
set -eu
[ "$DOCKER_HOST" = unix:///var/run/docker.sock ]
[ -z "${DOCKER_CONTEXT:-}" ]
printf 'docker %s\n' "$*" >> "$ADMIN_TEST_STATE/trace"
case "$1" in
	info) printf '%s\n' "${MOCK_ROOT:-$ADMIN_TEST_DATA/docker}" ;;
	ps) printf '%s' "${MOCK_CONTAINERS:-}" ;;
	images) printf '%s' "${MOCK_IMAGES:-}" ;;
	volume) printf '%s' "${MOCK_VOLUMES:-}" ;;
	network) printf '%s' "${MOCK_NETWORKS:-}" ;;
	*) exit 1 ;;
esac
MOCK
cat > "$TEST_ROOT/bin/findmnt" <<'MOCK'
#!/bin/sh
printf '%s\n' / "${MOCK_MOUNT:-/proc}"
MOCK
cat > "$TEST_ROOT/bin/flock" <<'MOCK'
#!/bin/sh
[ "$*" = '-n 9' ] && [ "${MOCK_LOCKED:-0}" = 0 ]
MOCK
cat > "$TEST_ROOT/bin/rm" <<'MOCK'
#!/bin/sh
printf 'rm %s\n' "$*" >> "$ADMIN_TEST_STATE/trace"
MOCK
cat > "$TEST_ROOT/bin/systemctl" <<'MOCK'
#!/bin/sh
set -eu
printf 'systemctl %s\n' "$*" >> "$ADMIN_TEST_STATE/trace"
case "$1" in
	is-active) exit 0 ;;
	show) printf '%s\n' 1234 ;;
	restart) mkdir -p "$ADMIN_TEST_DATA/docker" ;;
	*) exit 1 ;;
esac
MOCK
chmod 755 "$TEST_ROOT/bin/"*
count=0
run_case() {
	name=$1
	operation=$2
	expected=$3
	shift 3
	ADMIN_TEST_STATE=$TEST_ROOT/$name
	ADMIN_TEST_DATA=$ADMIN_TEST_STATE/data
	export ADMIN_TEST_STATE ADMIN_TEST_DATA
	mkdir -p "$ADMIN_TEST_DATA"
	: > "$ADMIN_TEST_DATA/marker"
	: > "$ADMIN_TEST_STATE/trace"
	case "$name" in
		existing_storage) mkdir "$ADMIN_TEST_DATA/docker" ;;
	esac
	status=0
	env "$@" sh "$TEST_ROOT/helper.sh" "$operation" > "$ADMIN_TEST_STATE/output" 2>&1 || status=$?
	passed=0
	case "$expected" in
		check) [ "$status" -ne 0 ] || passed=1 ;;
		clean)
			if [ "$status" -eq 0 ] \
				&& grep -Fxq 'rm -rf --one-file-system -- ./marker' "$ADMIN_TEST_STATE/trace"; then passed=1; fi ;;
		recover)
			if [ "$status" -eq 0 ] && [ -d "$ADMIN_TEST_DATA/docker" ] \
				&& grep -Fxq 'systemctl restart docker.service' "$ADMIN_TEST_STATE/trace"; then passed=1; fi ;;
		refuse) [ "$status" -eq 0 ] || passed=1 ;;
	esac
	if [ "$expected" = check ] || [ "$expected" = refuse ]; then
		! grep -Eq '^(rm |systemctl restart)' "$ADMIN_TEST_STATE/trace" || passed=0
	fi
	if [ "$passed" -ne 1 ] || [ ! -f "$ADMIN_TEST_DATA/marker" ]; then
		printf 'FAIL: %s (exit=%s)\n' "$name" "$status" >&2
		cat "$ADMIN_TEST_STATE/output" "$ADMIN_TEST_STATE/trace" >&2
		exit 1
	fi
	count=$((count + 1))
	printf 'ok %s - %s\n' "$count" "$name"
}

run_case preflight check check
run_case endpoint_override check check DOCKER_HOST=ssh://remote DOCKER_CONTEXT=remote
run_case unsupported_operation invalid refuse
run_case non_root check refuse MOCK_UID=1000
run_case wrong_daemon_root clean-data refuse MOCK_ROOT=/var/lib/docker
run_case clean_empty clean-data clean
run_case containers_remain clean-data refuse MOCK_CONTAINERS=container
run_case images_remain clean-data refuse MOCK_IMAGES=image
run_case volumes_remain clean-data refuse MOCK_VOLUMES=volume
run_case networks_remain clean-data refuse MOCK_NETWORKS=network
run_case mount_remains clean-data refuse "MOCK_MOUNT=$TEST_ROOT/mount_remains/data/mount"
run_case lock_busy clean-data refuse MOCK_LOCKED=1
run_case recover_missing recover recover
run_case existing_storage recover check
run_case recover_with_containers recover refuse MOCK_CONTAINERS=container
run_case wrong_daemon_process recover refuse MOCK_DAEMON=/usr/bin/other
printf 'All %s administrator helper checks passed.\n' "$count"
