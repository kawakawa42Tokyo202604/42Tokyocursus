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
default, the volume data is stored under `/home/takawaka/data` on the host.

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
  WordPress files -> named volume -> /home/takawaka/data/wordpress
  MariaDB data    -> named volume -> /home/takawaka/data/mariadb
  Backup archives -> named volume -> /home/takawaka/data/backups

Bonus host ports:
  FTP -> 21 and 21100-21110
  Static site -> 8080
  Adminer -> 8081
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
| `srcs/requirements/nginx/` | NGINX Dockerfile, TLS configuration, and tools. |
| `srcs/requirements/wordpress/` | WordPress/PHP-FPM Dockerfile, configuration, and tools. |
| `srcs/requirements/mariadb/` | MariaDB Dockerfile, configuration, and tools. |
| `srcs/requirements/bonus/` | Redis, FTP, static-site, Adminer, Backup, and Restore-check services. |
| `srcs/requirements/bonus/backup/` | Scheduled backup image and backup scripts. |
| `srcs/requirements/bonus/restore-check/` | Isolated temporary MariaDB restore verifier. |
| `secrets/` | Local secret files; its contents are ignored by Git. |
| `USER_DOC.md` | End-user and administrator instructions. |
| `DEV_DOC.md` | Developer setup, operation, and persistence details. |

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
  restore check never mounts the live application volumes and uses
  `network_mode: none`.
- Containers run their actual foreground process as PID 1 and use a restart
  policy. Infinite-loop keepalive commands are not used.

### Technology comparisons

| Topic | Comparison | Choice in this project |
| --- | --- | --- |
| Virtual machines vs Docker | A VM virtualizes hardware and runs a complete guest kernel. A container shares the host kernel and isolates processes and filesystems, making it lighter and faster to recreate. | The Docker stack runs inside the required VM. Each application component is isolated in its own container rather than its own VM. |
| Secrets vs environment variables | Environment variables are convenient for ordinary configuration but may be exposed through process inspection, configuration output, or logs. Secrets are mounted as files only into authorized services. | Non-sensitive settings use `srcs/.env`; passwords use ignored files under `secrets/` and Docker Compose secrets. |
| Docker network vs host network | A user-defined bridge network isolates container traffic and provides DNS by service name. Host networking shares the host network namespace and reduces isolation. | A dedicated Docker network connects the services. Host networking and legacy links are not used. |
| Docker volumes vs bind mounts | Docker volumes have a lifecycle managed by Docker and are portable at the Compose level. Bind mounts directly couple a container path to an arbitrary host path. | Named volumes are declared in Compose and configured to persist WordPress, MariaDB, and backup data under `/home/takawaka/data`; direct service bind mounts are not used by application containers. |

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
   cp srcs/.env.example srcs/.env
   ```

2. Fill in every required non-secret value in `srcs/.env`. The WordPress
   administrator username must not contain `admin` or `administrator`, with any
   capitalization. `BACKUP_SCHEDULE` uses five cron fields and
   `BACKUP_RETENTION` controls how many completed generations are kept.

3. Generate the local secret files expected by the final Compose configuration:

   ```sh
   ./tools/generate-secrets.sh
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

   The script restricts access to the files. Never add them to Git.

4. Map the VM IP address to the required domain on the machine running the web
   browser:

   ```text
   <VM_IP_ADDRESS> takawaka.42.fr
   ```

   When the browser runs inside the VM and system DNS or `/etc/hosts` cannot be
   changed, launch a dedicated Chrome or Chromium session with a temporary host
   mapping:

   ```sh
   make browser
   ```

   This mapping applies only to that browser profile. If automatic VM IP
   detection is not appropriate, run `VM_IP=<VM_IP_ADDRESS> make browser`.

### Build and run

From the repository root:

```sh
make
```

This creates the host data directories and runs Docker Compose with
`--build` in detached mode.

Useful targets:

| Command | Action |
| --- | --- |
| `make build` | Build or rebuild all project images. |
| `make up` | Build and start the stack in the background. |
| `make ps` | Show service status and published ports. |
| `make logs` | Follow logs from all services; press `Ctrl-C` to stop following. |
| `make stop` | Stop containers without removing them. |
| `make start` | Start previously stopped containers. |
| `make restart` | Restart existing containers without rebuilding them. |
| `make browser` | Open the site in a dedicated Chrome session with a temporary domain mapping. |
| `make backup` | Create one backup generation immediately. |
| `make restore-check` | Create a fresh backup and verify its database and WordPress files by restoring them into temporary storage. |
| `make down` | Stop and remove containers and the project network. |
| `make clean` | Run Compose down and remove orphan containers while preserving persistent data. |

After startup, open:

- Website: `https://takawaka.42.fr`
- WordPress administration: `https://takawaka.42.fr/wp-admin/`
- Bonus static site: `http://takawaka.42.fr:8080/`
- Adminer: `http://takawaka.42.fr:8081/` (use `mariadb` as the server name)

FTP clients connect to `takawaka.42.fr` on port 21 with the `FTP_USER` value
from `srcs/.env`. Passive mode uses ports 21100-21110, and
`FTP_PASV_ADDRESS` must be the address reachable by the FTP client. Redis is an
internal service and is not published to the host.

Backups are stored under `/home/takawaka/data/backups` by default. To use a
different host data directory, pass the same `DATA_DIR` to Make targets, for
example `DATA_DIR=/path/to/data make up`.

See [USER_DOC.md](USER_DOC.md) for routine operation and troubleshooting, and
[DEV_DOC.md](DEV_DOC.md) for setup and development details.

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

AI was used to help extract requirements from the subject, draft the initial
Makefile and directory scaffold, organize the documentation, and identify
relevant official references. The generated material was checked against the
subject and the current repository structure. AI was not used to generate or
store credentials. All generated content must be reviewed, tested, and
understood by the project author before submission.
