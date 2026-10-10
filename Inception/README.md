*This project has been created as part of the 42 curriculum by \<takawaka\>.*

# Inception

## Description

Inception is a system administration project that builds a small web
infrastructure with Docker Compose inside a virtual machine. Its purpose is to
practice image creation, service isolation, TLS configuration, container
networking, secret handling, and persistent storage.

The mandatory stack contains three services, each running in its own container
and built from its own Dockerfile:

- **NGINX** is the only public entry point. It accepts HTTPS connections on port
  443 using TLS 1.2 or TLS 1.3 and forwards PHP requests to WordPress.
- **WordPress with PHP-FPM** serves the application. It does not contain NGINX.
- **MariaDB** stores WordPress data. It does not contain NGINX.

NGINX, WordPress, and MariaDB communicate through a dedicated Docker network.
MariaDB data, WordPress files, and backup generations are stored in named
volumes so that they remain available when containers are recreated. By
default, Docker's data-root is `/home/takawaka/data/docker`. The volumes use
the local driver without bind options and are stored in its `volumes/` directory.

The optional bonus services are also included:

- **Redis** provides the WordPress object cache and is available only on the
  Docker network.
- **FTP** provides access to the WordPress volume on port 21 and passive ports
  21100-21110.
- **Static site** serves a non-PHP page on port 8080.
- **Adminer** provides a MariaDB web interface on port 8081.
- **Backup** periodically saves a MariaDB dump and the WordPress files without
  publishing a host port.
- **Restore-check** is an on-demand `ops` profile that restores the latest
  backup into temporary storage for verification.

### Architecture

```text
Browser
   |
   | HTTPS :443 (TLS 1.2 / TLS 1.3)
   v
 NGINX
   |
   | FastCGI on the Docker network
   v
 WordPress + PHP-FPM
   |
   | MariaDB protocol on the Docker network
   v
 MariaDB

Persistent data:
  /home/takawaka/data/docker/volumes/
    inception_wordpress_data/_data -> WordPress files
    inception_mariadb_data/_data   -> MariaDB data
    inception_backup_data/_data   -> Backup archives

Bonus host ports:
  FTP -> 127.0.0.1:21 and 127.0.0.1:21100-21110
  Static site -> 8080
  Adminer -> 127.0.0.1:8081
```

For the mandatory stack, only NGINX publishes the required host port 443.
WordPress, MariaDB, and Redis remain accessible only to containers attached to
the project network. The bonus services publish only the explicit additional
ports listed above.

### Included sources

| Path | Purpose |
| --- | --- |
| `Makefile` | Builds, starts, stops, and inspects the Compose project. |
| `srcs/docker-compose.yml` | Defines services, networks, volumes, and secrets. |
| `srcs/.env` | Stores local, non-secret configuration and is ignored by Git. |
| `srcs/.env.example` | Documents the expected non-secret configuration. |
| `srcs/docker/` | VM Docker storage configuration, administration helper, and recovery guide. |
| `srcs/tools/` | Secret generation, startup checks, and browser launcher. |
| `srcs/requirements/nginx/` | NGINX Dockerfile, TLS configuration, and tools. |
| `srcs/requirements/wordpress/` | WordPress/PHP-FPM Dockerfile, configuration, and tools. |
| `srcs/requirements/mariadb/` | MariaDB Dockerfile, configuration, and tools. |
| `srcs/requirements/bonus/` | Redis, FTP, static-site, Adminer, Backup, and Restore-check services. |
| `srcs/requirements/bonus/backup/` | Scheduled backup image and backup scripts. |
| `srcs/requirements/bonus/restore-check/` | Backup archive and restore verification client. |
| `srcs/requirements/bonus/restore-db/` | Temporary MariaDB service used only by Restore-check. |
| `secrets/` | Local secret files; its contents are ignored by Git. |
| `USER_DOC.md` | End-user and administrator instructions. |
| `DEV_DOC.md` | Developer setup, operation, and persistence details. |

### Docker storage files

`srcs/docker/` contains the VM's Docker storage configuration example and
the helper used for storage administration.

| File | Purpose |
| --- | --- |
| [daemon.json](srcs/docker/daemon.json) | Example configuration setting Docker's `data-root` to `/home/takawaka/data/docker`. |
| [inception-storage-admin.sh](srcs/docker/inception-storage-admin.sh) | Checks the storage path, deletes data after Docker resource cleanup, and restarts Docker to recreate missing storage. |
| [inception-storage.sudoers](srcs/docker/inception-storage.sudoers) | Allows `takawaka` to run the installed helper's `check`, `clean-data`, and `recover` operations through sudo without a password. |
| [STORAGE_RECOVERY.md](srcs/docker/STORAGE_RECOVERY.md) | Explains storage recovery after `make fclean`, its conditions, and verification steps. |

The daemon configuration and helper require an initial administrator setup.
After that setup, `takawaka` can run Make commands, including cleanup and
recovery. See [DEV_DOC](DEV_DOC.md#docker-storage-files) for the setup links
and the calls from Make to the helper. Storage recovery recreates Docker's
storage directory; deleted site data requires a separate backup restore.

### Main design choices

- Each service has one responsibility and runs in a dedicated container.
- Service images are built locally from project Dockerfiles; only a permitted
  Debian or Alpine base image may be pulled.
- Images use explicit version tags. The `latest` tag is not used.
- NGINX is the sole public entry point for the mandatory WordPress site, and
  the mandatory entry point uses only port 443.
- Bonus services publish separate, documented ports and do not expose
  WordPress or MariaDB directly.
- A user-defined Docker network provides service-name DNS and isolates internal
  traffic from the host network.
- Passwords are supplied as secret files. The `.env` file is reserved for
  non-secret values such as domain names, database names, and usernames.
- The backup service runs cron in the foreground and stores each completed
  generation atomically with checksums under owner-only permissions. The
  restore check never mounts the live application volumes and uses an internal,
  private network with an ephemeral MariaDB service and no published ports.
- Containers run their actual foreground process as PID 1 and use a restart
  policy. Infinite-loop keepalive commands are not used.

### Technology comparisons

| Topic | Comparison | Choice in this project |
| --- | --- | --- |
| Virtual machines vs Docker | A VM virtualizes hardware and runs a complete guest kernel. A container shares the host kernel and isolates processes and filesystems, making it lighter and faster to recreate. | The Docker stack runs inside the required VM. Each application component is isolated in its own container rather than its own VM. |
| Secrets vs environment variables | Environment variables are convenient for ordinary configuration but may be exposed through process inspection, configuration output, or logs. Secrets are mounted as files only into authorized services. | Non-sensitive settings use `srcs/.env`; passwords use ignored files under `secrets/` and Docker Compose secrets. |
| Docker network vs host network | A user-defined bridge network isolates container traffic and provides DNS by service name. Host networking shares the host network namespace and reduces isolation. | A dedicated Docker network connects the services. Host networking and legacy links are not used. |
| Docker volumes vs bind mounts | Docker volumes have a lifecycle managed by Docker. Bind mounts couple a container path to a host directory. A named volume with `o: bind` still uses a bind mount internally. | Plain local named volumes have no driver options. The VM's Docker data-root places their data under `/home/takawaka/data/docker/volumes/`. |

## Instructions

### Prerequisites

- A Linux virtual machine
- Docker Engine with the Docker Compose v2 plugin
- GNU Make
- OpenSSL for generating local secrets and HTTPS verification
- Permission to run Docker and create `/home/takawaka/data`
- A local DNS or `/etc/hosts` entry mapping the VM address to
  `takawaka.42.fr`

### Configuration

1. Create the local environment file:

   ```sh
   test -e srcs/.env || cp srcs/.env.example srcs/.env
   vim srcs/.env
   ```

2. Review every required non-secret value in `srcs/.env`. The four WordPress
   username/email fields may be blank in `srcs/.env.example`, but must be filled
   in the copied `srcs/.env` before startup. See the
   [environment-file instructions](DEV_DOC.md#2-create-the-environment-file)
   for the exact fields. Valid demo usernames and email addresses may be used
   for local testing or a defense. Use different usernames and different valid
   email addresses for the two WordPress accounts. Email delivery is not required
   for setup or login; use reachable addresses for email notifications and
   password recovery.
   Existing `srcs/.env` files must not be overwritten.
   The WordPress administrator username must not contain `admin` or
   `administrator`, regardless of capitalization. `BACKUP_SCHEDULE` uses
   five cron fields and `BACKUP_RETENTION` controls how many completed
   generations are kept.

3. Generate the local secret files expected by the final Compose configuration:

   ```sh
   ./srcs/tools/generate-secrets.sh
   ```

   The script creates one file for each password using `openssl` and leaves
   existing files untouched. The files are:

   ```text
   secrets/db_root_password.txt
   secrets/db_password.txt
   secrets/wp_admin_password.txt
   secrets/wp_user_password.txt
   secrets/redis_password.txt
   secrets/ftp_password.txt
   ```

   The script sets the directory to mode `700` and new files to `600`.
   Existing files keep their values and permissions; restrict their permissions
   as described in [USER_DOC](USER_DOC.md#credentials). Never add them to Git.

4. Map the VM IP address to the required domain on the machine running the web
   browser:

   ```text
   <VM_IP_ADDRESS> takawaka.42.fr
   ```

   For a browser inside the VM, use `127.0.0.1 takawaka.42.fr` in the VM's
   `/etc/hosts`. Once name resolution works, open Firefox, Chrome, or Chromium
   from the graphical session of the ordinary user:

   ```sh
   make browser
   ```

   The launcher checks normal name resolution; it does not change DNS.

5. Configure the VM's Docker data-root using [DEV_DOC.md](DEV_DOC.md).
   `srcs/docker/daemon.json` is an example to merge with existing daemon
   settings. Migrate existing data before changing a running installation.

### Build and run

From the repository root:

```sh
make
```

This checks configuration, secret permissions, and Docker storage, builds each
service image through Compose, and starts the stack in detached mode. The
Compose project is named `inception`. On the supplied VM, `takawaka` has Docker
access and can run all Make targets; the installed storage helper supplies
administrator permission for cleanup and recovery. Run the browser from
takawaka's graphical session.

If evaluation cleanup has removed the data-root of the empty, running system
Docker daemon, Make restarts that daemon before building. See
[storage recovery](srcs/docker/STORAGE_RECOVERY.md) for the conditions and
verification procedure. Deleted application data is initialized afresh.

Useful targets:

| Command | Action |
| --- | --- |
| `make` / `make up` | Check configuration, build active service images, and start the stack in the background. |
| `make check-config` | Validate local files, Compose, and Docker storage without changing them. |
| `make test` | Run isolated mocked tests for recovery, full cleanup, and the administrator helper without touching the live Docker daemon. |
| `make build` | Build or rebuild active service images without starting containers; restore images are built by `make restore-check`. |
| `make ps` | Show service status and published ports. |
| `make logs` | Follow logs from all services; press `Ctrl-C` to stop following. |
| `make stop` | Stop containers without removing them. |
| `make start` | Start previously stopped containers. |
| `make restart` | Restart existing containers without rebuilding them. |
| `make browser` | Open the site using the installed browser and normal DNS. |
| `make backup` | Create one backup generation immediately. |
| `make restore-check` | Create a fresh backup and verify its database and WordPress files by restoring them into temporary storage. |
| `make down` | Stop and remove containers and the project network. |
| `make clean` | Run Compose down and remove orphan containers while preserving persistent data. |
| `make fclean` | Delete every Docker container, image, volume, custom network and `/home/takawaka/data/*` in the evaluation VM, including site data and backups. |

After startup, open:

- Website: `https://takawaka.42.fr`
- WordPress administration: `https://takawaka.42.fr/wp-admin/`
- Bonus static site: `http://takawaka.42.fr:8080/`
- Adminer, inside the VM: `http://127.0.0.1:8081/` (server: `mariadb`)

FTP clients inside the VM connect to `127.0.0.1` on port 21 with the `FTP_USER` value
from `srcs/.env`. Passive mode uses ports 21100-21110, and
`FTP_PASV_ADDRESS` must be the address reachable by the FTP client.
`ADMIN_BIND_ADDRESS` defaults to `127.0.0.1` for both FTP and Adminer because
their protocols do not encrypt credentials. Use the SSH tunnels documented in
USER_DOC for remote access. Redis is internal and is not published to the host.

Backups are stored in `inception_backup_data`. Inspect its host path with
`docker volume inspect inception_backup_data`. `DATA_DIR` changes the expected
Docker storage path checked by Make; it does not reconfigure the daemon.

See [USER_DOC.md](USER_DOC.md) for routine operation and troubleshooting, and
[DEV_DOC.md](DEV_DOC.md) for setup and development details.

For the defense, start with the
[command checklist](USER_DOC.md#review-command-checklist), then follow the
[peer-review walkthrough](USER_DOC.md#peer-review-walkthrough) and the
[developer review guide](DEV_DOC.md#peer-review-preparation-and-technical-checks).
For database and reboot checks, use the
[minimal and extended commands for 6.9 / 6.10](USER_DOC.md#review-commands-for-69-and-610).
Together they cover the evaluation checklist: official source and credential
checks, initial cleanup, startup, TLS, users and database access, volumes,
browser edits, VM reboot, a service configuration change, and all five bonuses.
The cleanup commands permanently delete the VM's Docker resources and data;
run them only in the evaluation VM after preserving anything needed.

## Resources

- [Docker overview](https://docs.docker.com/get-started/docker-overview/)
- [Docker Compose application model](https://docs.docker.com/compose/intro/compose-application-model/)
- [Docker Compose file reference](https://docs.docker.com/compose/compose-file/)
- [Docker volumes](https://docs.docker.com/engine/storage/volumes/)
- [Docker bind mounts](https://docs.docker.com/engine/storage/bind-mounts/)
- [Docker bridge network driver](https://docs.docker.com/engine/network/drivers/bridge/)
- [Docker host network driver](https://docs.docker.com/engine/network/drivers/host/)
- [Docker Compose secrets](https://docs.docker.com/compose/how-tos/use-secrets/)
- [Docker volume backup and restore](https://docs.docker.com/engine/storage/volumes/)
- [NGINX HTTPS configuration](https://nginx.org/en/docs/http/configuring_https_servers.html)
- [WordPress with NGINX](https://developer.wordpress.org/advanced-administration/server/web-server/nginx/)
- [MariaDB Server documentation](https://mariadb.com/docs/server/)
- [MariaDB dump documentation](https://mariadb.com/docs/server/clients-and-utilities/backup-restore-and-import-clients/mariadb-dump)
- [Redis Object Cache plugin](https://wordpress.org/plugins/redis-cache/)
- [vsftpd documentation](https://security.appspot.com/vsftpd.html)
- [Adminer](https://www.adminer.org/)
- [Python HTTP server](https://docs.python.org/3/library/http.server.html)

### AI usage

AI assisted with requirement analysis, Dockerfiles and entrypoint scripts,
WordPress initialization, backup and restore verification, the Makefile,
documentation, reviews, and VM validation. It also assisted with the native
volume migration, startup checks, and storage recovery regression tests.
