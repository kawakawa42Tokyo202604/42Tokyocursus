# Recovery after evaluation cleanup

The project uses plain Docker named volumes with the VM Docker data-root at
`/home/takawaka/data/docker`. The evaluation's removal of
`/home/takawaka/data/*` therefore deletes the daemon's metadata as well as the
application data. Keeping the old daemon running can make the next build fail
with a missing `docker/tmp` directory.

## What Make does

`make`, `make up`, `make build`, `make backup`, and `make restore-check` run
`srcs/tools/prepare-storage.sh`, then the existing configuration checks, before
calling Compose. The recovery script:

1. Returns immediately if the expected Docker data-root directory exists.
2. If it is missing, verifies that Docker reports that exact data-root, the
   active Docker endpoint is the local system socket, and
   no running or stopped containers remain.
3. Uses the root-owned helper when called by takawaka; it checks that
   `docker.service` is active and its main process is `dockerd`.
4. Restarts that service once and confirms that the daemon is accessible and
   has recreated the expected directory.

If a prerequisite or restart fails, Make stops before building. The script
does not delete files, remove Docker objects, migrate storage, change daemon
configuration, or create secrets. `make check-config` remains read-only.
Recovery supports the VM's rootful Docker managed by systemd; remote daemons
and rootless Docker endpoints require their own administrator procedure.

On the supplied VM, the [one-time setup in DEV_DOC](../../DEV_DOC.md#prerequisites) installs
`/usr/local/sbin/inception-storage-admin` and its narrow sudo policy.
`takawaka` can then run `make` and `make fclean` directly. Only fixed helper
operations run as root; builds and Compose remain under the ordinary account.

This is a fresh installation after intentional data deletion. Existing
`srcs/.env` and `secrets/` provide the configuration and initial credentials.
Deleted posts, database contents, and backup generations are not recovered.
Partial deletion of an existing data-root is not repaired automatically.

## Verify the evaluation sequence

Use a disposable VM with the same Docker configuration and project source.
The cleanup below deletes all Docker resources and application data in that
VM. Copy any data that must be retained to a verified archive outside
`/home/takawaka/data` first. Keep the local environment and secrets outside
that directory too.

In the takawaka shell with the installed helper, from the repository root:

1. Start a working installation and check its data-root:

   ```sh
   make
   docker info --format '{{.DockerRootDir}}'
   ```

2. Run the complete evaluation cleanup:

   ```sh
   make fclean
   docker ps -aq
   docker images -qa
   docker volume ls -q
   docker network ls
   ```

   This target requires the installed helper permission and the matching
   local system Docker daemon.
   It stops/removes all containers, images, volumes and custom networks,
   checks that resources and nested mounts are gone, then deletes
   `/home/takawaka/data/*`. It skips empty resource lists and keeps the
   built-in networks. Unsafe paths, source/configuration inside the deletion
   path, or failed removals stop the sequence. The helper provides the
   permissions requested by the evaluation's final `sudo rm -rf` command.

3. Leave Docker running and run `make`. Expect the storage-recovery message,
   then the normal image build and Compose startup. Inspect:

   ```sh
   make
   make check-config
   make ps
   docker volume inspect inception_mariadb_data inception_wordpress_data
   curl --insecure --head https://takawaka.42.fr
   ```

   The two required volumes must use driver `local`, have no bind options, and
   have mountpoints beneath `/home/takawaka/data/docker/volumes`. MariaDB,
   Redis, and WordPress must become healthy. Verify the two WordPress users,
   login, HTTPS response, and database access.

4. Save some application content, record
   `systemctl show --property=MainPID --value docker.service`, and run `make`
   again. The daemon PID and saved content should remain unchanged. Also
   verify persistence after container recreation and VM reboot.

## Automated checks

`make test` exercises the decision logic with mocked Docker, systemd and
cleanup commands. It covers existing/missing storage, endpoint selection,
remaining resources, missing privileges, unsafe cleanup paths, failed
removals and recovery failures. It makes no changes to a running Docker daemon
or real application data.

A separate local test reproduced the missing-storage build failure with a
private Docker 27.3.1 daemon, then ran the production recovery script through a
test service adapter. Image rebuild, new named volumes, networks, containers,
and data persistence succeeded. That test used rootless Docker with `vfs` and
does not replace the VM's systemd and full WordPress stack validation.
Use the sequence above on the project VM for that validation; the
[review walkthrough](../../USER_DOC.md#peer-review-walkthrough) covers the
full application checks.
