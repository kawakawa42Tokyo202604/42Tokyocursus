# User Documentation

This document explains how an end user or administrator can operate the
Inception stack after its service definitions and Dockerfiles have been
implemented.

## Services provided

| Service | Purpose | Host access |
| --- | --- | --- |
| NGINX | Terminates TLS and serves the WordPress site. | HTTPS on port 443 only. |
| WordPress + PHP-FPM | Runs WordPress and processes PHP requests. | Internal Docker network only. |
| MariaDB | Stores WordPress posts, users, settings, and metadata. | Internal Docker network only. |
| Redis | Stores the WordPress object cache. | Internal Docker network only. |
| FTP | Provides file access to the WordPress volume. | Port 21 and passive ports 21100-21110. |
| Static site | Serves a non-PHP bonus page. | HTTP on port 8080. |
| Adminer | Provides a MariaDB web interface. | HTTP on port 8081. |
| Backup | Periodically saves MariaDB and WordPress data. | No host port. |
| Restore-check | Verifies a backup in temporary storage. | On demand through Make. |

The services restart after a crash according to the restart policy in
`srcs/docker-compose.yml`. WordPress files and MariaDB data are stored in named
volumes and survive normal container recreation. Backup generations are stored
under the backup volume. Redis and Backup are internal; the other bonus
services publish only the ports shown in the table.

## Before starting

Confirm that:

- The Linux virtual machine is running.
- Docker is running and the current user can access it.
- `srcs/.env` contains the required non-secret configuration.
- Optional backup settings in `srcs/.env` are valid: `BACKUP_SCHEDULE` uses five
  cron fields and `BACKUP_RETENTION` is between 1 and 1000.
- The password files required by `srcs/docker-compose.yml` exist under
  `secrets/`.
- The VM IP address resolves as `takawaka.42.fr` from the client machine.

Do not put passwords in `srcs/.env`, Dockerfiles, Compose command lines, or Git.

## Start the project

From the repository root, run:

```sh
make
```

This prepares `/home/takawaka/data`, builds the images, and starts the services
in the background.

Check the service state and published ports:

```sh
make ps
```

Every long-running mandatory and bonus service should be running. NGINX must
publish port `443`; the expected host bonus ports are `21`, `21100-21110`,
`8080`, and `8081`. The Backup container runs its scheduler without publishing
a port. Restore-check is intentionally not part of the default stack.

## Stop and resume the project

Stop the containers without removing them:

```sh
make stop
```

Start the same containers again:

```sh
make start
```

Restart running containers:

```sh
make restart
```

Stop and remove the containers and project network:

```sh
make down
```

`make down` and `make clean` preserve the named volumes and the host data under
`/home/takawaka/data`, including completed backups. Do not add `--volumes`
unless permanent data removal is intentional and a verified backup exists.

## Access WordPress

Open these URLs in a browser:

- Website: `https://takawaka.42.fr`
- Administration panel: `https://takawaka.42.fr/wp-admin/`

When using a browser inside the VM without permission to change system DNS or
`/etc/hosts`, open the site in a dedicated Chrome or Chromium profile:

```sh
make browser
```

The temporary domain mapping is limited to that browser profile. It does not
configure name resolution for another client machine.

Sign in with the WordPress administrator username configured in `srcs/.env`
and the corresponding local secret. The administrator username must not contain
`admin` or `administrator`, regardless of capitalization.

If the project uses a self-signed TLS certificate, the browser may show a trust
warning. Verify that the certificate belongs to this local project before
accepting it.

## Access the bonus services

Open the following bonus endpoints when they are enabled:

- Static site: `http://takawaka.42.fr:8080/`
- Adminer: `http://takawaka.42.fr:8081/`

In Adminer, select `MySQL` or `MariaDB`, enter `mariadb` as the server, and use
the database username/password from `srcs/.env` and `secrets/db_password.txt`.
Adminer is intentionally exposed over HTTP for this local bonus service; do not
publish it beyond the intended VM/client network.

For FTP, use the `FTP_USER` value from `srcs/.env`, the password in
`secrets/ftp_password.txt`, host `takawaka.42.fr`, and port `21`. Enable passive
mode and allow ports `21100-21110` through the VM firewall. Set
`FTP_PASV_ADDRESS` to the address reachable by the FTP client before starting
the stack.

## Backup and restore verification

Create a backup immediately while the stack is running:

```sh
make backup
```

The command stores a timestamped generation under
`/home/takawaka/data/backups`. Each generation contains a compressed MariaDB
dump, a compressed WordPress file archive, a manifest, and SHA-256 checksums.
Incomplete generations are not published as completed backups. Old completed
generations are removed according to `BACKUP_RETENTION` (seven by default).
The backup directory and generated archives use owner-only permissions because
the WordPress archive includes `wp-config.php`.

Create a fresh backup and verify the complete restore path:

```sh
make restore-check
```

The check validates the checksums, extracts the WordPress archive into a
temporary directory, starts a temporary MariaDB with a private Unix socket,
imports the SQL dump into a temporary database, and checks that WordPress
tables and options were restored. It never mounts the live WordPress or
MariaDB data volumes and does not modify the running application data.

## Credentials

Non-secret settings are stored in:

```text
srcs/.env
```

Passwords are stored as local files under:

```text
secrets/
```

The intended secret-file convention is:

| File | Purpose |
| --- | --- |
| `secrets/db_root_password.txt` | MariaDB root password. |
| `secrets/db_password.txt` | Password for the WordPress database user. |
| `secrets/wp_admin_password.txt` | WordPress administrator password. |
| `secrets/wp_user_password.txt` | Password for the regular WordPress user. |
| `secrets/redis_password.txt` | Password used by Redis and WordPress object cache. |
| `secrets/ftp_password.txt` | FTP user password. |

Keep secret files readable only by their owner. Before first startup, permissions
can be restricted with:

```sh
chmod 700 secrets
chmod 600 secrets/*.txt
```

The contents of `srcs/.env` and `secrets/` are ignored by Git. Confirm this
before every submission. Do not paste credentials into logs, screenshots, issue
reports, or chat messages.

For an existing installation, changing a secret file alone may not update the
credential already stored in MariaDB or WordPress. Rotate the application-side
credential and its secret together, then recreate the affected container. Back
up persistent data before changing database credentials.

## Check that the stack is healthy

Show container status:

```sh
make ps
```

Follow service logs:

```sh
make logs
```

Press `Ctrl-C` to stop following logs. This does not stop the containers.

Check the HTTPS response from the VM or client machine:

```sh
curl --insecure --head https://takawaka.42.fr
```

Inspect the negotiated TLS version:

```sh
openssl s_client -connect takawaka.42.fr:443 -servername takawaka.42.fr -tls1_2
```

Expected results:

- NGINX publishes port 443, and the bonus services publish only their documented ports.
- The website responds over HTTPS.
- WordPress can load content and reach MariaDB.
- WordPress reports the Redis object cache as connected.
- The static site and Adminer respond on ports 8080 and 8081.
- An FTP client can log in and list the WordPress volume in passive mode.
- `make backup` creates a completed generation.
- `make restore-check` reports successful database and WordPress restoration.
- Data remains available after `make down` followed by `make up`.

## Troubleshooting

### The domain does not resolve

Confirm that the client machine has an `/etc/hosts` or DNS entry in this form:

```text
<VM_IP_ADDRESS> takawaka.42.fr
```

### Port 443 is unreachable

Check `make ps`, the VM firewall, the VM network mode, and whether another
process already uses port 443.

### FTP cannot connect or list files

Check that port 21 and passive ports 21100-21110 are reachable, that
`FTP_PASV_ADDRESS` matches the address used by the client, and that the FTP
service is healthy in `make ps` and `make logs`.

### A service repeatedly restarts

Run `make logs` and inspect the first error from that service. Common causes are
missing secret files, incorrect file permissions, invalid configuration, and an
unavailable dependency.

### The site starts but data is missing

Confirm that the named volumes exist and that their configured host directories
under `/home/takawaka/data` contain the expected data. Do not initialize a new
database over an existing installation without a verified backup.

### A backup cannot be created

Confirm that MariaDB and WordPress are healthy, that the WordPress volume is
initialized, and that `secrets/db_password.txt` is readable by Docker. Inspect
the backup service log without printing secret files.

### Restore verification fails

Check the first reported failure. A checksum error means the backup generation
was changed or is incomplete; a database error means the SQL dump could not be
imported into the temporary MariaDB; a WordPress error means the file archive
is incomplete. The live application data is not changed by this check.
