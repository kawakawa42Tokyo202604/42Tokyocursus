# Developer Documentation

This document describes how to prepare, build, operate, and inspect the
Inception development environment.

## Prerequisites

Use a Linux virtual machine with:

- Docker Engine
- Docker Compose v2
- GNU Make
- Git
- `curl` and OpenSSL for HTTPS verification
- Permission to create `/home/takawaka/data` and access the Docker daemon

Verify the main tools:

```sh
docker --version
docker compose version
make --version
```

## Repository layout

```text
.
├── Makefile
├── README.md
├── USER_DOC.md
├── DEV_DOC.md
├── secrets/
├── tools/
│   └── generate-secrets.sh
└── srcs/
    ├── .env
    ├── .env.example
    ├── docker-compose.yml
    └── requirements/
        ├── nginx/
        │   ├── Dockerfile
        │   ├── conf/
        │   └── tools/
        ├── wordpress/
        │   ├── Dockerfile
        │   ├── conf/
        │   └── tools/
        ├── mariadb/
        │   ├── Dockerfile
        │   ├── conf/
        │   └── tools/
        └── bonus/
            ├── redis/
            ├── ftp/
            ├── static-site/
            ├── adminer/
            ├── backup/
            └── restore-check/
```

Each mandatory service must be built from its matching Dockerfile. The image
name and service name must match. Do not use prebuilt NGINX, WordPress, or
MariaDB service images; only the permitted Debian or Alpine base image may be
pulled.

## Set up the environment from scratch

### 1. Configure local name resolution

Map the VM IP address to the required domain on the machine that will access the
site:

```text
<VM_IP_ADDRESS> takawaka.42.fr
```

On Linux this is normally added to `/etc/hosts`. This is a host-level setting,
not part of the repository.

For a browser running inside the VM, a sudo-free, browser-scoped alternative is
available:

```sh
make browser
```

The launcher reads `DOMAIN_NAME` from `srcs/.env`, determines the IPv4 address
used by the default route, and starts a dedicated Chrome or Chromium profile
with a temporary host mapping. Override IP detection when necessary:

```sh
VM_IP=<VM_IP_ADDRESS> make browser
```

This does not configure DNS for browsers running on another machine.

### 2. Create the environment file

Copy the example:

```sh
cp srcs/.env.example srcs/.env
```

Fill in every non-secret value required by `srcs/docker-compose.yml` and the
entrypoint scripts. These normally include:

- Domain name
- Database name
- Database username
- WordPress administrator username and email
- WordPress regular username and email
- FTP username and the passive-mode address reachable by FTP clients
- Optional backup schedule and retention (`BACKUP_SCHEDULE` and
  `BACKUP_RETENTION`)

The WordPress administrator username must not contain `admin` or
`administrator`, regardless of capitalization. Keep passwords out of `.env`.

### 3. Create local secrets

Generate the password files expected by the Compose top-level `secrets` section:

```sh
./tools/generate-secrets.sh
```

The script creates the following files with random values and leaves existing
files untouched:

```text
secrets/db_root_password.txt
secrets/db_password.txt
secrets/wp_admin_password.txt
secrets/wp_user_password.txt
secrets/redis_password.txt
secrets/ftp_password.txt
```

The `.gitignore` excludes `srcs/.env` and secret contents. Verify that no secret
is tracked before committing:

```sh
git ls-files srcs/.env secrets
```

The expected output is empty, except that `secrets/.gitkeep` may be listed.

### 4. Prepare persistent storage

The Makefile creates:

```text
/home/takawaka/data/mariadb
/home/takawaka/data/wordpress
/home/takawaka/data/backups
```

Run the preparation target directly if needed:

```sh
make prepare
```

The Compose file declares three named volumes whose local-driver configuration
stores data in these host directories. The services consume named volumes rather
than declaring direct bind mounts for the application data stores. `DATA_DIR`
can be overridden for both Make targets and Compose interpolation.

## Build and launch

Validate the resolved Compose model before building:

```sh
docker compose -f srcs/docker-compose.yml --env-file srcs/.env config
```

Build and start the complete stack:

```sh
make
```

Equivalent explicit operations are available as separate targets:

```sh
make build
make up
```

`make up` runs Compose with `up -d --build`, so changed images are rebuilt and
the stack starts in detached mode.

## Manage containers and logs

| Command | Purpose |
| --- | --- |
| `make ps` | Display the current service state and published ports. |
| `make logs` | Follow logs from all services. |
| `make stop` | Stop containers without removing them. |
| `make start` | Start existing stopped containers. |
| `make restart` | Restart containers without rebuilding images. |
| `make backup` | Create one completed backup generation immediately. |
| `make restore-check` | Create a fresh backup and verify restoration in isolated temporary storage. |
| `make down` | Remove containers and the project network. |
| `make clean` | Remove containers and orphan containers while preserving data. |

Direct Compose commands must use the same file and environment file as the
Makefile. For example:

```sh
docker compose -f srcs/docker-compose.yml --env-file srcs/.env ps
docker compose -f srcs/docker-compose.yml --env-file srcs/.env logs --tail=100
```

## Service implementation rules

- Use one custom Dockerfile per service.
- Use the penultimate stable release of Debian or Alpine as the base.
- Pin an explicit base-image tag; never use `latest`.
- Do not store passwords in Dockerfiles, build arguments, Compose files, or Git.
- Run the real service process in the foreground as PID 1.
- Do not keep containers alive with `tail -f`, an interactive shell,
  `sleep infinity`, or an infinite loop.
- Configure a restart policy for every mandatory service.
- Publish only NGINX port 443 for the mandatory stack. Bonus services may
  publish only their explicitly documented ports: FTP 21 and 21100-21110,
  static-site 8080, and Adminer 8081.
- The Backup service publishes no port and runs cron in the foreground. The
  Restore-check service is a one-shot `ops` profile and is not started by the
  default stack.
- Restore-check may start a temporary local MariaDB child for the duration of
  its import test; it stops and removes that child before exiting and is not a
  keepalive process.
- Enable only TLS 1.2 and TLS 1.3 in NGINX.
- Do not use host networking, `links`, or `--link`.
- Create the WordPress administrator and a separate regular WordPress user.

## Networking

All mandatory services must join an explicitly declared user-defined Docker
network. Use Compose service names for internal connections:

- NGINX connects to the WordPress/PHP-FPM service.
- WordPress connects to MariaDB.
- WordPress connects to Redis for object caching.
- FTP shares the WordPress volume; static-site and Adminer use the network as
  their service boundary.
- Backup reads the WordPress volume read-only and connects to MariaDB with the
  database secret. Restore-check uses only the backup volume and
  `network_mode: none`.
- MariaDB is not published to the host.

Do not hard-code container IP addresses because they can change when containers
are recreated.

## Data persistence

The intended persistent stores are:

| Data | Host location | Consumer |
| --- | --- | --- |
| MariaDB database | `/home/takawaka/data/mariadb` | MariaDB |
| WordPress files | `/home/takawaka/data/wordpress` | WordPress and NGINX as required |
| Backup generations | `/home/takawaka/data/backups` | Backup and Restore-check |

Container writable layers are disposable. Persistent state must live in the
named volumes. `make down`, `make clean`, and container recreation should not
delete this data.

Inspect volumes with:

```sh
docker volume ls
docker volume inspect <VOLUME_NAME>
```

Do not run `docker compose down --volumes`, `docker volume rm`, or
`docker volume prune` unless data deletion is intentional and a verified backup
exists.

## Verification checklist

After implementation, run these checks from the repository root:

1. Validate Compose interpolation and schema:

   ```sh
   docker compose -f srcs/docker-compose.yml --env-file srcs/.env config
   ```

2. Build and start:

   ```sh
   make up
   ```

3. Check status and ports:

   ```sh
   make ps
   ```

4. Check the HTTPS response:

   ```sh
   curl --insecure --head https://takawaka.42.fr
   ```

5. Confirm TLS 1.2 or TLS 1.3 negotiation:

   ```sh
   openssl s_client -connect takawaka.42.fr:443 -servername takawaka.42.fr -tls1_2
   ```

6. Recreate containers and confirm persistence:

   ```sh
   make down
   make up
   ```

   Verify that WordPress content and database users are unchanged.

7. Create and verify a backup without changing live data:

   ```sh
   make restore-check
   ```

   Confirm that the command reports checksum success, restored database table
   and options counts, and that the backup generation exists under the backup
   data directory.

8. Confirm that no credentials are tracked by Git and that no password appears
   in Dockerfiles or the Compose file.

Bonus checks:

```sh
docker compose -f srcs/docker-compose.yml --env-file srcs/.env exec -T redis \
  /usr/local/bin/healthcheck.sh
docker compose -f srcs/docker-compose.yml --env-file srcs/.env exec -T wordpress \
  wp --allow-root --path=/var/www/html redis status
curl --fail http://127.0.0.1:8080/
curl --fail http://127.0.0.1:8081/
```

Verify FTP separately with a passive-mode client. The client must reach port 21
and ports 21100-21110, and `FTP_PASV_ADDRESS` must advertise the address that
the client can reach. Redis is deliberately not published to the host.

## Documentation maintenance

Keep `README.md`, `USER_DOC.md`, `DEV_DOC.md`, `srcs/.env.example`, the secret
file names, and the Compose service names synchronized. Update the documentation
whenever commands, paths, ports, credentials, or persistence behavior changes.
