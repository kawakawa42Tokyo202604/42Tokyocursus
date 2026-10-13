# User Documentation

Japanese translation: [USER_DOC_ja.md](USER_DOC_ja.md).

This document explains how an end user or administrator can operate the
Inception stack.

For operational checks, start with the [operation command checklist](#operation-command-checklist).
See [database and persistence commands](#database-and-persistence-commands) for MariaDB
and data persistence after a VM reboot, and [service verification procedures](#service-verification-procedures)
for more detailed checks of individual services.

## Services provided

| Service | Purpose | Host access |
| --- | --- | --- |
| NGINX | Terminates TLS and serves the WordPress site. | HTTPS on port 443 only. |
| WordPress + PHP-FPM | Runs WordPress and processes PHP requests. | Internal Docker network only. |
| MariaDB | Stores WordPress posts, users, settings, and metadata. | Internal Docker network only. |
| Redis | Stores the WordPress object cache. | Internal Docker network only. |
| FTP | Provides file access to the WordPress volume. | VM localhost: port 21 and passive ports 21100-21110. |
| Static site | Serves a non-PHP bonus page. | HTTP on port 8080. |
| Adminer | Provides a MariaDB web interface. | VM localhost: HTTP on port 8081. |
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
- `srcs/.env` contains the required non-secret configuration, reviewed for this
  installation. The four WordPress username/email fields may be blank in the
  `srcs/.env.example` template, but must be filled in the copied `srcs/.env`
  before startup. See the [four required fields](DEV_DOC.md#2-create-the-environment-file).
  Valid example usernames and email addresses may be used for local testing.
  The two WordPress accounts need different usernames and different valid
  email addresses.
  Receiving email is not required for setup or login.
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

This checks local configuration and Docker's data-root, builds the images,
and starts the services in the background. On the supplied VM, `takawaka` is
in the Docker group and can run ordinary Docker/Make commands. Full cleanup and Docker storage recovery use the preinstalled administrator helper
automatically. Run all Make targets as `takawaka`, including `make fclean`
and the first `make` afterward. Use the graphical session for `make browser`.

`make` and `make up` perform the same operation. Running Make again checks
configuration and builds again, using Docker's cache for unchanged layers.
The existing named volumes retain the installed site and database.

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

### Full cleanup, including persistent data

**`make fclean` deletes every Docker container, image, volume and custom
network in the VM, then `/home/takawaka/data/*`. WordPress, database contents
and backups in that directory are permanently deleted.** Export any data to
keep outside that directory first; see the
[full cleanup procedure](DEV_DOC.md#3-perform-a-complete-cleanup-and-fresh-startup).

From the repository in the dedicated VM:

```sh
make fclean
make
make ps
```

The cleanup checks the installed administrator permission before deleting
anything and refuses a remote/rootless Docker endpoint, a different data-root,
unsafe paths, or a repository/configuration inside the deletion path.
It stops if removal fails; it deletes the data directory's
non-hidden entries only after checking no Docker resources or nested mounts
remain. The local `srcs/.env` and `secrets/` outside the data directory provide
the configuration for the next startup. Use this operation only when intentionally deleting all existing data and rebuilding from scratch. For persistence checks, use normal container shutdown, recreation, and reboot instead.

## Access WordPress

Open these URLs in a browser:

- Website: `https://takawaka.42.fr`
- Administration panel: `https://takawaka.42.fr/wp-admin/`

Once the VM's `/etc/hosts` maps `127.0.0.1` to `takawaka.42.fr`, open the site
using Firefox, Chrome, or Chromium from the graphical desktop:

```sh
make browser
```

The launcher uses normal name resolution and reports an error if it is missing.
Another client needs its own DNS/hosts mapping and a route to the VM.

Sign in with the WordPress administrator username configured in `srcs/.env`
and the corresponding local secret. The administrator username must not contain
`admin` or `administrator`, regardless of capitalization.

If the project uses a self-signed TLS certificate, the browser may show a trust
warning. Verify that the certificate belongs to this local project before
accepting it.

## Access the bonus services

Open the following bonus endpoints when they are enabled:

- Static site: `http://takawaka.42.fr:8080/`
- Adminer, inside the VM: `http://127.0.0.1:8081/`

In Adminer, select `MySQL` or `MariaDB`, enter `mariadb` as the server, and use
`MYSQL_USER` and `MYSQL_DATABASE` from `srcs/.env`. The database password is
in `secrets/db_password.txt`.
Adminer uses HTTP and is bound to localhost by default. For remote access, run
this command on the workstation and open `http://127.0.0.1:8081/` there:

```sh
ssh -N -p 2222 -L 127.0.0.1:8081:127.0.0.1:8081 takawaka@127.0.0.1
```

For FTP, use the `FTP_USER` value from `srcs/.env`, the password in
`secrets/ftp_password.txt`, host `127.0.0.1` inside the VM, and port `21`.
Use a dedicated FTP account. An existing account must have a UID different from
root and the PHP-FPM user (www-data); aliases sharing either UID are rejected.
Enable passive mode. FTP has no transport encryption, so it also binds to
localhost. Keep `FTP_PASV_ADDRESS=127.0.0.1` for local clients and SSH tunnels.
Remote FTP requires forwarding the control port (use local port 2121) and all
passive ports on the workstation:

```sh
ssh -N -p 2222 \
  -L 127.0.0.1:2121:127.0.0.1:21 \
  -L 127.0.0.1:21100:127.0.0.1:21100 \
  -L 127.0.0.1:21101:127.0.0.1:21101 \
  -L 127.0.0.1:21102:127.0.0.1:21102 \
  -L 127.0.0.1:21103:127.0.0.1:21103 \
  -L 127.0.0.1:21104:127.0.0.1:21104 \
  -L 127.0.0.1:21105:127.0.0.1:21105 \
  -L 127.0.0.1:21106:127.0.0.1:21106 \
  -L 127.0.0.1:21107:127.0.0.1:21107 \
  -L 127.0.0.1:21108:127.0.0.1:21108 \
  -L 127.0.0.1:21109:127.0.0.1:21109 \
  -L 127.0.0.1:21110:127.0.0.1:21110 takawaka@127.0.0.1
```

Connect the remote FTP client to `127.0.0.1:2121`. `ADMIN_BIND_ADDRESS` changes
the bind address for both administrative services; review exposure before
changing it. FTP accounts can modify executable site files and must be trusted.

## Backup and restore verification

Create a backup immediately while the stack is running:

```sh
make backup
```

The command stores a timestamped generation under
`/home/takawaka/data/docker/volumes/inception_backup_data/_data`.
The schedule uses UTC by default. Each generation contains a compressed MariaDB
dump, a compressed WordPress file archive, a manifest, and SHA-256 checksums.
Incomplete generations are not published as completed backups. Old completed
generations are removed according to `BACKUP_RETENTION` (seven by default).
The backup directory and generated archives use owner-only permissions because
the WordPress archive includes `wp-config.php`.
If another backup holds the lock, the command fails without creating a
generation. Wait for that backup to finish and retry.

Create a fresh backup and verify the complete restore path:

```sh
make restore-check
```

Make passes the new generation's name to the restore check, so that exact
generation is verified. Backup failure stops the verification.
The check validates the checksums, extracts the WordPress archive into a
temporary directory, imports the SQL dump into a temporary MariaDB service on
an internal private network, and checks that WordPress tables and options were
restored. It never mounts the live WordPress or MariaDB data volumes and does
not modify the running application data.

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

Mode `700` allows only the directory owner to list, enter, and modify the
directory; mode `600` allows only the file owner to read and write the files.
The generator applies these modes to the directory and newly generated files.
It preserves existing files. Make checks the permissions during startup and
`make check-config`, and stops if group/other access is enabled; it does not
repair permissions. Check modes without displaying passwords:

```sh
stat -c '%a %n' secrets secrets/*.txt
make check-config
```

The contents of `srcs/.env` and `secrets/` are ignored by Git. Confirm this
before committing changes. Do not paste credentials into logs, screenshots, issue
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
openssl s_client -connect takawaka.42.fr:443 -servername takawaka.42.fr \
  -tls1_2 -brief </dev/null
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

## Operation command checklist

Use this command index to inspect the repository, configuration, networking, TLS,
persistent data, and additional services. If a fresh initialization is unnecessary,
avoid `make fclean` and keep existing data intact.

Run the commands in Bash inside the VM, as `takawaka`, from the repository root.
Set up `dc`, `wp`, and `review_domain` in
[Step 1](#1-start-the-stack-and-define-checking-commands). The table links to
procedures; read their prerequisites and expected results as well.

| Area | Basic command or action | Detailed checks and instructions |
| --- | --- | --- |
| Source repository | `git clone`, `git remote get-url origin`, `git status --short` | [Check the repository URL, clone directory, commit, and aliases](DEV_DOC.md#1-clone-and-identify-the-source-repository) |
| Configuration and source | `dc config --quiet`, `git ls-files`, `less Makefile srcs/docker-compose.yml` | [Check required files, untracked secrets, Dockerfiles, and scripts](DEV_DOC.md#2-prepare-the-shell-and-inspect-the-project-files) |
| Fresh setup (deletes data) | `make fclean` → `make` → `make ps` | [Deletion scope, backup precautions, and empty Docker resources](DEV_DOC.md#3-perform-a-complete-cleanup-and-fresh-startup) |
| System architecture | Explain Docker, Compose, images, containers, VMs, and the stack topology | [Architecture overview](DEV_DOC.md#verification-coverage-and-architecture-notes) |
| README | `head -n 1 README.md`, `less README.md` | [File structure and documentation](DEV_DOC.md#2-prepare-the-shell-and-inspect-the-project-files) |
| Documentation | `less USER_DOC.md DEV_DOC.md` | [User and developer guides](DEV_DOC.md#2-prepare-the-shell-and-inspect-the-project-files) |
| Initial startup | `make ps`, `make browser`, open the configured WordPress site | [HTTPS response, disabled port 80, and WordPress initialization](#2-verify-https-and-tls) |
| Docker images | Inspect Dockerfiles, `dc config --images`, `make -n up` | [Image construction and base images](DEV_DOC.md#2-prepare-the-shell-and-inspect-the-project-files), [restart behavior and PID 1](DEV_DOC.md#4-inspect-the-running-infrastructure) |
| Docker network | `docker network ls`, `docker network inspect inception_inception` | [Bridge network, service DNS, and internal ports](DEV_DOC.md#4-inspect-the-running-infrastructure) |
| NGINX / TLS | `dc port nginx 443`, check port 80, test TLS 1.2 and 1.3 | [curl, OpenSSL, certificate, and NGINX configuration](#2-verify-https-and-tls) |
| WordPress / PHP-FPM / volumes | `wp user list`, inspect volumes, add a comment as a regular user and edit a page as administrator | [Volume locations](#3-inspect-persistent-volumes), [accounts and browser tests](#4-check-wordpress-and-database-access) |
| MariaDB / volumes | Log in to the database, `SHOW TABLES;`, `SELECT COUNT(*) FROM wp_users;` | [Basic and detailed database checks](#database-and-persistence-commands) |
| Data persistence | Reboot the VM → run `make up ps browser` in the same clone → check saved data and database | [Basic checks](#database-and-persistence-commands), [JSON snapshots and volumes](#5-verify-persistence-across-container-recreation-and-vm-reboot) |
| Configuration changes | Edit the configuration → `make up` → `make ps` → test the changed service | [NGINX port-change and rollback example](DEV_DOC.md#5-change-a-service-configuration-and-restore-it) |
| Additional service: Redis | `wp redis status`, set/get a cache key from separate processes | [Healthcheck, connection, cache reuse, and cleanup](#7-check-additional-services) |
| Additional service: FTP | Use curl to log in and upload, then retrieve the file through HTTPS | [Password prompt, passive transfer, shared volume, and cleanup](#7-check-additional-services) |
| Additional service: Static site | `curl http://127.0.0.1:8080/`, open it in the VM browser | [HTTP response and PHP-free source](#7-check-additional-services) |
| Additional service: Adminer | Log in to the database at `http://127.0.0.1:8081/` and inspect WordPress tables | [Connection, HTTP check, and SQL query](#7-check-additional-services) |
| Additional service: Backup | Check the cron schedule, `make restore-check` | [Schedule, generations, checksums, and temporary restoration](#7-check-additional-services) |
| Restore original settings | Restore saved configuration and WordPress URLs/constants → `make up` → verify operation | [Configuration rollback procedure](DEV_DOC.md#5-change-a-service-configuration-and-restore-it) |

**`make fclean` removes all Docker resources in the VM, including the site,
database, and backups.** Preserve necessary data elsewhere before using it.
Never run it during persistence testing. Command output alone does not replace
browser interaction or an actual VM reboot.

Follow logs with `make logs` and stop following them with `Ctrl-C`.
For a shorter output, use `dc logs --tail=100`. Resolve unexpected results
before proceeding.

## Database and persistence commands

This section checks MariaDB, its volume, and data persistence after a VM reboot.
**Run the commands in Bash inside the VM, as `takawaka`, from the clone's root.**
If the local clone is at `~/asd`, first run the command below; otherwise,
change to the corresponding directory.

```sh
cd ~/asd
```

`dc` and `wp` are [helper functions](#1-start-the-stack-and-define-checking-commands).
On the prepared VM, they are available in new Bash terminals. In an already
open terminal, reload them with `source ~/.bashrc`. On a fresh VM, define the
functions as shown in the linked section.

| Area | Basic checks | Detailed checks |
| --- | --- | --- |
| MariaDB | Service status, database login, tables and user count | Dockerfile, volume path, and SQL queries for posts and comments |
| Persistence | VM reboot, restart, browser and database verification | Compare pages, comments, users, and volumes before and after reboot |

### Check MariaDB and its volume

**Basic checks**

The following commands use the example database settings in `srcs/.env.example`
(`MYSQL_USER=wp_user`, `MYSQL_DATABASE=wordpress`). If these values have
changed, substitute the current names or use the detailed commands below.

```sh
make ps
dc exec mariadb mariadb -h127.0.0.1 -u wp_user -p wordpress
```

The password is stored in `secrets/db_password.txt`. Characters are not
shown as you enter it. At the MariaDB prompt, run the following SQL statements.
`EXIT;` returns to the normal shell.

```sql
SHOW TABLES;
SELECT COUNT(*) FROM wp_users;
EXIT;
```

Confirm that MariaDB is `Up` and `healthy`, and that WordPress tables and
users exist. An initial installation commonly contains about 12 tables and two
users; counts may change when users or plugins are added. Inspect the Dockerfile
and volume as needed.

**Detailed checks**

```sh
cat srcs/requirements/mariadb/Dockerfile
docker volume ls
docker volume inspect inception_mariadb_data --format '{{.Mountpoint}}'
dc exec mariadb sh -c \
  'exec mariadb --protocol=TCP --host=127.0.0.1 --user="$MYSQL_USER" --password "$MYSQL_DATABASE"'
```

Confirm that the Dockerfile installs `mariadb-server` and does not include
NGINX. The volume path is
`/home/takawaka/data/docker/volumes/inception_mariadb_data/_data`.
The final command uses the container's database settings to log in;
the password is the same as in the basic check.

```sql
SELECT DATABASE();
SHOW TABLES;
SELECT COUNT(*) AS wordpress_users FROM wp_users;
SELECT post_type, COUNT(*) AS rows_count FROM wp_posts GROUP BY post_type;
SELECT comment_ID, comment_post_ID, comment_approved FROM wp_comments;
EXIT;
```

Inspect the database name, user count, posts, pages, and any test comments.
The standard table prefix is `wp_`; use the actual prefix if it was changed.

### Data persistence after a VM reboot

**Basic checks**

1. Open the edited page and published comment in a browser. Record an identifying
   text string and URL, and note the database user count using the basic MariaDB checks.
2. Save your work and reboot Debian from the VM's desktop power menu.
3. Log back in as `takawaka` and run the commands from the same clone.

```sh
cd ~/asd
make up ps browser
```

`make up ps browser` builds and starts the stack, shows service status, and
opens the browser in sequence. Adjust the `cd` path for a differently named clone.
Confirm that WordPress does not show its installation wizard, that the same
page and comment remain, and that the administrator can still log in.
Repeat the basic MariaDB checks and compare table and user counts with their
pre-reboot values.

**Detailed checks**

Run these commands before the reboot and after the stack has restarted, then
compare results. Verify that users, page contents, comments, approval states,
and volume paths are unchanged.

```sh
wp core is-installed
wp user list --fields=ID,user_login,roles --format=table
wp post list --post_type=page --post_status=publish \
  --fields=ID,post_title,post_content --format=table
wp comment list \
  --fields=comment_ID,comment_post_ID,comment_content,comment_approved --format=table
docker volume inspect inception_wordpress_data inception_mariadb_data \
  --format '{{.Name}} path={{.Mountpoint}}'
```

To save output to files and compare it, use the
[before-and-after snapshot procedure](#5-verify-persistence-across-container-recreation-and-vm-reboot).
Verify the results in a browser as well.
**Do not run `make fclean` during persistence tests: it deletes the site,
database, and backups.**

## Service verification procedures

Use the following procedures to verify a running deployment. If you need a
completely fresh installation, first follow the
[full cleanup procedure](DEV_DOC.md#3-perform-a-complete-cleanup-and-fresh-startup), but do not
perform it on a stack whose data must be preserved.
These checks assume the default project name `inception` and run **inside the
VM, from the repository root**, as `takawaka`. Configure Docker access and
the storage administrator helper as described in DEV_DOC.
Browser actions require a graphical session.

The shell functions below use the same Compose and environment files as the
Makefile. Inspect their definitions using `type dc wp`. On the supplied
VM, they are defined in `takawaka`'s `~/.bashrc`; a new Bash terminal loads
them, and an existing terminal can run `source ~/.bashrc`. On a fresh VM,
define the functions in your working shell as shown below.
Do not source `srcs/.env` as a shell script: its cron value uses Compose's
environment-file syntax.

### 1. Start the stack and define checking commands

From the repository in the ordinary user's terminal, start the project:

```sh
unset COMPOSE_PROJECT_NAME
dc() {
  command docker compose -f srcs/docker-compose.yml --env-file srcs/.env "$@"
}
wp() {
  dc exec -T wordpress wp --allow-root --path=/var/www/html "$@"
}
make
dc ps --all
review_domain=$(dc exec -T wordpress printenv DOMAIN_NAME)
: "${review_domain:?Could not read DOMAIN_NAME from WordPress}"
review_url="https://$review_domain"
```

Keep this terminal open for the following checks.
Expect eight running services: `mariadb`, `redis`,
`wordpress`, `nginx`, `ftp`, `adminer`, `static-site`, and `backup`.
MariaDB, Redis, and WordPress must become `healthy`; the other services do not
define healthchecks. `restore-db` and `restore-check` run only on demand.
If a service exits or stays unhealthy, inspect it before continuing:

```sh
dc logs --tail=100 mariadb wordpress nginx redis
```

### 2. Verify HTTPS and TLS

Check normal DNS/hosts resolution, then probe the VM's own NGINX independently
of DNS and proxy settings:

```sh
getent ahostsv4 "$review_domain"
dc port nginx 443
curl --noproxy '*' --insecure --fail --silent --show-error \
  --resolve "$review_domain:443:127.0.0.1" \
  --output /dev/null --write-out 'HTTPS status: %{http_code}\n' "$review_url/"
curl --noproxy '*' --connect-timeout 3 --max-time 5 \
  --resolve "$review_domain:80:127.0.0.1" "http://$review_domain/"
```

Expect HTTPS status `200` and an NGINX host mapping for port `443`. The HTTP
probe must fail to connect, normally with curl exit code `7`; an HTTP response
or a redirect from port 80 does not satisfy the requirement. A failed DNS or
proxy request alone does not prove that port 80 is closed.

Demonstrate both allowed TLS versions and inspect the public certificate:

```sh
openssl s_client -connect 127.0.0.1:443 -servername "$review_domain" \
  -tls1_2 -brief </dev/null
openssl s_client -connect 127.0.0.1:443 -servername "$review_domain" \
  -tls1_3 -brief </dev/null
dc exec -T nginx openssl x509 -in /etc/nginx/ssl/server.crt \
  -noout -subject -issuer -dates -ext subjectAltName
dc exec -T nginx nginx -T 2>&1 \
  | grep -E 'listen |ssl_protocols |ssl_certificate'
```

Expect successful TLS 1.2 and TLS 1.3 handshakes, a certificate for the configured
domain, `listen 443 ssl`, and `ssl_protocols TLSv1.2 TLSv1.3`. A self-signed
verification warning is expected for this local certificate.

Check that older protocols cannot negotiate:

```sh
openssl s_client -connect 127.0.0.1:443 -servername "$review_domain" \
  -tls1 -cipher 'ALL:@SECLEVEL=0' -brief </dev/null
openssl s_client -connect 127.0.0.1:443 -servername "$review_domain" \
  -tls1_1 -cipher 'ALL:@SECLEVEL=0' -brief </dev/null
```

Expect a server protocol-version rejection and no successful handshake.
A client-side error such as `no protocols available` does not demonstrate a
server rejection; use the rendered NGINX configuration and a capable client.

In a **separate graphical terminal as the ordinary VM user**, run
`make browser` from the same clone. Open `https://takawaka.42.fr/` and
`https://takawaka.42.fr/wp-admin/`. Confirm the site is configured and no
WordPress installation wizard appears. The curl `--resolve` probes do not
configure browser DNS. SSH terminals do not provide a graphical display by
themselves. Bonus HTTP ports 8080 and 8081 are separate from the forbidden
WordPress HTTP entry point on port 80.

### 3. Inspect persistent volumes

```sh
docker info --format '{{.DockerRootDir}}'
docker volume ls
docker volume inspect inception_wordpress_data inception_mariadb_data \
  inception_backup_data \
  --format '{{.Name}} driver={{.Driver}} options={{json .Options}} path={{.Mountpoint}}'
```

Expect data-root `/home/takawaka/data/docker`, driver `local`, empty options
(`null` or `{}`), and these mandatory mountpoints:

- WordPress: `/home/takawaka/data/docker/volumes/inception_wordpress_data/_data`.
- MariaDB: `/home/takawaka/data/docker/volumes/inception_mariadb_data/_data`.

The backup volume is below the same data-root. These are native named volumes;
there are no bind-driver options. The [developer runtime checks](DEV_DOC.md#4-inspect-the-running-infrastructure)
show which containers share each volume and whether each mount is read-only.

### 4. Check WordPress and database access

Check installation, the two WordPress accounts, and the configured URLs:

```sh
wp core is-installed && printf 'WordPress is installed.\n'
wp user list --fields=ID,user_login,roles --format=table
wp option get home
wp option get siteurl
wp post list --post_type=page --post_status=publish \
  --fields=ID,post_title,post_status --format=table
wp comment list \
  --fields=comment_ID,comment_post_ID,comment_author,comment_approved --format=table
```

Expect an administrator and a different regular user (`editor` in this
implementation). The administrator login must not contain `admin` in any
capitalization. The URLs should be `https://takawaka.42.fr` at the default
port. These WordPress accounts are distinct from MariaDB's root and application
database accounts.

Perform the following browser checks as well:

1. Log in as the regular WordPress user using the matching local secret.
   Add a distinctive comment to a published post with comments enabled.
   If moderation is enabled, approve it from the administrator dashboard.
2. Log in as the administrator, edit and publish a page, and include a
   distinctive test string in the page content.
3. Open the public post and page in a logged-out/private browser window and
   verify the comment and edited page are visible. Record the post/page and
   comment IDs for the reboot check. CLI lists alone do not replace this test.

For a MariaDB login, the following command deliberately prompts for the
application database password. Read `secrets/db_password.txt` privately and
enter it at the prompt; do not put its value into the command or screen capture:

```sh
dc exec mariadb sh -c \
  'exec mariadb --protocol=TCP --host=127.0.0.1 --user="$MYSQL_USER" --password "$MYSQL_DATABASE"'
```

At the MariaDB prompt, run:

```sql
SELECT DATABASE();
SHOW TABLES;
SELECT COUNT(*) AS wordpress_users FROM wp_users;
SELECT post_type, COUNT(*) AS rows_count FROM wp_posts GROUP BY post_type;
SELECT comment_ID, comment_post_ID, comment_approved FROM wp_comments;
EXIT;
```

Expect the configured database, nonempty WordPress tables, both WordPress
users, and the test content. The default table prefix is `wp_`; use the
installation's prefix if it was deliberately changed. To demonstrate the
separate MariaDB administrator, use `secrets/db_root_password.txt` privately
at this prompt:

```sh
dc exec mariadb mariadb --user=root --password
```

Run `SELECT User, Host FROM mysql.user;`, then `EXIT;`. Root is local to
MariaDB; the configured application database user connects from the project
network.

### 5. Verify persistence across container recreation and VM reboot

Capture the browser changes and user accounts **after** the preceding edits.
Store comparison snapshots outside the Docker data directory:

```sh
review_evidence=$(mktemp -d /home/takawaka/inception-review.XXXXXX)
printf 'Keep this evidence path for after reboot: %s\n' "$review_evidence"
wp post list --post_type=page --post_status=publish --orderby=ID --order=ASC \
  --fields=ID,post_title,post_content,post_status --format=json \
  > "$review_evidence/pages.before.json"
wp comment list --orderby=comment_ID --order=ASC \
  --fields=comment_ID,comment_post_ID,comment_content,comment_approved --format=json \
  > "$review_evidence/comments.before.json"
wp user list --orderby=ID --order=ASC \
  --fields=ID,user_login,roles --format=json > "$review_evidence/users.before.json"
make down
make up
make ps
```

Reopen the public page and comment, and confirm that both test markers remain.
This tests container recreation, not an actual VM reboot. Do not use
Do not use `--volumes` or run `make fclean`.

Save open work, then reboot Debian using the power menu inside the VM's
graphical session. After it returns, log in as `takawaka`, open a Bash terminal,
and enter the same clone. If using SSH, reconnect with `ssh -A`.
Unset `COMPOSE_PROJECT_NAME` and check that `dc` and `wp` are available
as described in Step 1. Set `review_evidence` to the exact path printed before
reboot; replace the example path below:

```sh
unset COMPOSE_PROJECT_NAME
review_evidence=/home/takawaka/inception-review.REPLACE_WITH_SAVED_SUFFIX
test -d "$review_evidence"
make up
make ps
wp core is-installed && printf 'WordPress is still installed.\n'
wp post list --post_type=page --post_status=publish --orderby=ID --order=ASC \
  --fields=ID,post_title,post_content,post_status --format=json \
  > "$review_evidence/pages.after.json"
wp comment list --orderby=comment_ID --order=ASC \
  --fields=comment_ID,comment_post_ID,comment_content,comment_approved --format=json \
  > "$review_evidence/comments.after.json"
wp user list --orderby=ID --order=ASC \
  --fields=ID,user_login,roles --format=json > "$review_evidence/users.after.json"
diff -u "$review_evidence/pages.before.json" "$review_evidence/pages.after.json"
diff -u "$review_evidence/comments.before.json" "$review_evidence/comments.after.json"
diff -u "$review_evidence/users.before.json" "$review_evidence/users.after.json"
review_domain=$(dc exec -T wordpress printenv DOMAIN_NAME)
: "${review_domain:?Could not read DOMAIN_NAME from WordPress}"
review_url="https://$review_domain"
```

Every snapshot command must succeed; an empty file caused by a failed command
is not valid evidence. Each `diff` should print nothing and exit with code
`0` if no other user changed the site during the test. Repeat the database
queries and browser checks from Step 4. Recheck the volume mountpoints and
HTTPS response. A configured homepage alone does not prove that edited data
survived.

### 6. Rebuild after changing configuration

Follow the [NGINX port-change and rollback procedure](DEV_DOC.md#5-change-a-service-configuration-and-restore-it)
to verify that configuration changes take effect. Choose
a service and an available port. The NGINX example changes its port, verifies
WordPress display, login, and saved data, then restores port 443.

### 7. Check additional services

You can check the additional services individually as needed. First verify that MariaDB and WordPress are running correctly.

**Redis: connected cache and reuse across separate PHP processes**

```sh
dc exec -T redis /usr/local/bin/healthcheck.sh
wp redis status
wp cache set review-probe cache-ok inception-review 60
wp cache get review-probe inception-review
wp cache delete review-probe inception-review
```

Expect the healthcheck to exit `0`, Redis status to be connected with a valid
drop-in, and the separate `cache get` process to print `cache-ok` before the
60-second expiry. The final command removes only the test key.

**FTP: login, passive transfer, and the shared WordPress volume**

These commands run inside the VM. curl prompts for the FTP password when
`--user` contains only the username; use `secrets/ftp_password.txt` privately.
Keep `FTP_PASV_ADDRESS=127.0.0.1` for this local test.

```sh
review_ftp_user=$(dc exec -T ftp printenv FTP_USER)
curl --noproxy '*' --fail --ftp-pasv --user "$review_ftp_user" \
  --list-only ftp://127.0.0.1/
review_ftp_file=$(mktemp --suffix=.txt /tmp/inception-ftp-review.XXXXXX)
review_ftp_name=$(basename "$review_ftp_file")
printf 'Inception FTP review check\n' > "$review_ftp_file"
curl --noproxy '*' --fail --ftp-pasv --user "$review_ftp_user" \
  --upload-file "$review_ftp_file" "ftp://127.0.0.1/$review_ftp_name"
dc exec -T wordpress test -f "/var/www/html/$review_ftp_name"
curl --noproxy '*' --insecure --fail --silent --show-error \
  --resolve "$review_domain:443:127.0.0.1" "$review_url/$review_ftp_name"
curl --noproxy '*' --fail --ftp-pasv --user "$review_ftp_user" \
  --quote "DELE $review_ftp_name" --list-only ftp://127.0.0.1/
rm -- "$review_ftp_file"
```

Expect WordPress files in the listing, a successful upload, and the same test
text through HTTPS. Cleanup removes only the generated test file, not any
existing site file. Use the [FTP SSH tunnel](#access-the-bonus-services) for a
workstation client; forwarding port 21 alone is insufficient for passive FTP.

**Static site and Adminer: HTTP checks followed by browser use**

```sh
curl --noproxy '*' --fail --silent --show-error http://127.0.0.1:8080/
curl --noproxy '*' --fail --silent --show-error \
  --output /dev/null --write-out 'Adminer status: %{http_code}\n' http://127.0.0.1:8081/
dc exec -T static-site find /var/www/static -type f -name '*.php'
```

Expect the static page, Adminer status `200`, and no PHP files in the static
site. Open both in the VM browser. In Adminer, log in with server `mariadb`,
the configured database user, and the database secret, then browse the
WordPress tables and run `SELECT COUNT(*) FROM wp_users;`. Loading only
Adminer's login page does not demonstrate database access.

**Backup: scheduled storage and a verified restore**

```sh
dc exec -T backup cat /etc/cron.d/inception-backup
make restore-check
dc exec -T backup find /var/backups/inception \
  -mindepth 1 -maxdepth 1 -type d -name '20*' -printf '%f\n'
dc --profile ops ps --all
```

`make restore-check` already creates a fresh backup. Expect checksum lines
ending in `OK`, a completed generation, and
`Restore verification succeeded` with positive table and options counts.
The first run also builds the two ops images, which increases the initial build workload.
The one-shot restore container and temporary database are removed afterward.
The scheduled Backup service must remain running. Backups are
useful because they preserve both the SQL data and WordPress files, and that
the restore test uses temporary storage rather than the live data volumes.
An ordinary reboot preserves backups; `make fclean` removes their volume
unless the backups were copied outside `/home/takawaka/data`.

## Troubleshooting

### Cannot access Docker / docker.sock permission denied

Check the daemon and the groups of the current terminal:

```sh
systemctl is-active docker
id -nG
getent group docker
docker ps
```

Expect an active daemon, `docker` in the current session's groups, and
successful Docker access. If the group database includes `takawaka` but
`id -nG` does not include `docker`, save work and log out of the VM desktop,
then log in again. A new terminal in the same old desktop session inherits
the old groups. For initial setup, follow the
[Docker group instructions](DEV_DOC.md#one-time-docker-group-setup-on-a-fresh-vm).
The `Cannot access Docker` message does not mean that changing `.env` is
required; verify daemon access first.

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
service is running without errors in `make ps` and `make logs`.

### A service repeatedly restarts

Run `make logs` and inspect the first error from that service. Common causes are
missing secret files, incorrect file permissions, invalid configuration, and an
unavailable dependency.

### The site starts but data is missing

Confirm that the named volumes exist and that their configured host directories
under `/home/takawaka/data` contain the expected data. Do not initialize a new
database over an existing installation without a verified backup.

Docker's own metadata also lives below that directory. A full cleanup
intentionally removes the data and all containers. After that cleanup, run
`make` as `takawaka`: the installed helper can restart the empty local daemon
when its data-root is missing, then initialize a fresh installation using
`srcs/.env` and `secrets/`. It does not restore deleted posts or backups.
Follow [the storage recovery procedure](srcs/docker/STORAGE_RECOVERY.md).

### A backup cannot be created

Confirm that MariaDB and WordPress are healthy, that the WordPress volume is
initialized, and that `secrets/db_password.txt` is readable by Docker. Inspect
the backup service log without printing secret files.

### Restore verification fails

Check the first reported failure. A checksum error means the backup generation
was changed or is incomplete; a database error means the SQL dump could not be
imported into the temporary MariaDB; a WordPress error means the file archive
is incomplete. The live application data is not changed by this check.
