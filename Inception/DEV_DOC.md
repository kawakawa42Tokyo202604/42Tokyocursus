# Developer Documentation

This document describes how to prepare, build, operate, and inspect the
Inception development environment.

## Prerequisites

Use a Linux virtual machine with:

- Docker Engine
- Docker Compose v2.17 or newer (the supplied VM uses Compose v5)
- GNU Make
- Git
- Bash for the review shell
- `less` for browsing source files
- `curl` and OpenSSL for HTTPS verification
- Permission to create `/home/takawaka/data` and access the Docker daemon

Verify the main tools:

```sh
docker --version
docker compose version
make --version
```

On the supplied VM, `takawaka` is a member of the Docker group and can run
all Make targets. A root-owned storage helper has narrowly scoped passwordless
sudo permission for its `check`, `clean-data` and `recover` operations.
The Make scripts call it automatically when administrator permission is needed;
run `make fclean` and the first `make` afterward as `takawaka`.
Use the graphical session for `make browser`.

### One-time Docker group setup on a fresh VM

After installing Docker, the VM administrator adds the ordinary user:

```sh
su root -c '/usr/sbin/usermod -aG docker takawaka'
```

Save open work, log out of the VM's graphical desktop, and log in again as
`takawaka`. Then open a terminal and verify:

```sh
id -nG
docker ps
```

Expect `docker` in the current session's groups and successful daemon access.
Opening another terminal within the old desktop session does not refresh its
groups. The absolute `usermod` path also works when `/usr/sbin` is absent from
the administrator shell's PATH. The storage helper setup below is still needed
for cleanup and daemon recovery.

### One-time storage permission setup on a fresh VM

After configuring Docker's data-root below, the VM administrator installs the
helper once from the repository:

```sh
su root -c 'sh srcs/tools/install-storage-admin.sh'
```

The installer requires `sudo` and `visudo`, validates the policy, and installs
root-owned copies at `/usr/local/sbin/inception-storage-admin` (755) and
`/etc/sudoers.d/inception-storage` (440). The helper uses the local system
Docker socket and the fixed path `/home/takawaka/data`; it clears Docker
endpoint overrides and uses a system PATH. It authorizes no arbitrary Make,
shell or editable repository script. Install updated helper code as the VM
administrator again after changing its source. Ordinary builds do not install
or modify system permissions.

## Repository layout

```text
.
├── Makefile
├── README.md
├── USER_DOC.md
├── DEV_DOC.md
├── secrets/
└── srcs/
    ├── .env
    ├── .env.example
    ├── docker-compose.yml
    ├── docker/
    │   ├── daemon.json
    │   ├── inception-storage-admin.sh
    │   ├── inception-storage.sudoers
    │   └── STORAGE_RECOVERY.md
    ├── tools/
    │   ├── check-config.sh
    │   ├── fclean.sh
    │   ├── prepare-storage.sh
    │   ├── install-storage-admin.sh
    │   ├── generate-secrets.sh
    │   └── open-site.sh
    ├── tests/
    │   ├── test-fclean.sh
    │   ├── test-storage-admin.sh
    │   └── test-storage-recovery.sh
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
            ├── restore-check/
            └── restore-db/
```

Each mandatory service must be built from its matching Dockerfile. The image
name and service name must match. Do not use prebuilt NGINX, WordPress, or
MariaDB service images; only the permitted Debian or Alpine base image may be
pulled.

### Docker storage files

`srcs/docker/` は、VMのDockerデーモンの保存先と管理操作に関するファイルです。
サービスをビルドするDockerfileは `srcs/requirements/` にあります。

| ファイル | 役割 |
| --- | --- |
| `daemon.json` | `data-root` を `/home/takawaka/data/docker` にする設定例。Dockerのvolumeを必要な保存先の下に置きます。 |
| `inception-storage-admin.sh` | 管理者権限で `check`（保存先の検査）、`clean-data`（Docker資源を削除した後のdata削除）、`recover`（保存先消失後のDocker再起動）を行います。 |
| `inception-storage.sudoers` | `takawaka` に、インストール済みヘルパーの上記3操作をパスワードなしで実行する権限を与える設定です。 |
| `STORAGE_RECOVERY.md` | 評価用の全削除後に保存先を作り直す流れ、確認方法、制限を説明します。 |

`daemon.json` は[初期設定](#4-prepare-persistent-storage)で `/etc/docker/daemon.json`
へ既存設定を保ちながら反映します。リポジトリに置いただけではDockerの設定は変わりません。
ヘルパーとsudo設定は[管理者による初回インストール](#one-time-storage-permission-setup-on-a-fresh-vm)
でroot所有の固定パスへ配置します。その後は `takawaka` のMakeから必要な管理操作を呼び出します。

呼び出しの流れは次のとおりです。

- `make fclean` → `srcs/tools/fclean.sh` → ヘルパーの `check`・`clean-data`。
- 全削除後の `make` → `srcs/tools/prepare-storage.sh` → 必要な場合にヘルパーの `recover`。

`clean-data` はコンテナ・イメージ・volume・カスタムネットワークが残っている場合に停止します。
`recover` は保存先が消失し、コンテナが残っていない等の条件を確認してからDockerを再起動します。
削除済みのサイトやDBをバックアップから戻す処理は含みません。

## Set up the environment from scratch

### 1. Configure local name resolution

Map the VM IP address to the required domain on the machine that will access the
site:

```text
<VM_IP_ADDRESS> takawaka.42.fr
```

On Linux this is normally added to `/etc/hosts`. This is a host-level setting,
not part of the repository.

For a browser inside the VM, add `127.0.0.1 takawaka.42.fr` to its `/etc/hosts`.
Verify with `getent ahostsv4 takawaka.42.fr`. Then, as the graphical user:

```sh
make browser
```

The launcher reads `DOMAIN_NAME`, checks normal name resolution, and opens
Firefox (preferred), Chrome, or Chromium. It does not change system or browser
DNS. Clients outside the VM need their own mapping and network access.

### 2. Create the environment file

Create the local file only when it does not already exist, then edit it:

```sh
test -e srcs/.env || cp srcs/.env.example srcs/.env
vim srcs/.env
```

The current `srcs/.env.example` template leaves the following four WordPress
username/email fields blank:

```dotenv
WORDPRESS_ADMIN_USER=
WORDPRESS_ADMIN_EMAIL=
WORDPRESS_USER=
WORDPRESS_USER_EMAIL=
```

After copying, fill all four fields in `srcs/.env` **before** running Make.
Blank values in the local file cause WordPress startup to fail. Review every
other required setting as well.

For a local test or defense, valid demo values are sufficient, for example:

```dotenv
WORDPRESS_ADMIN_USER=taka_owner
WORDPRESS_ADMIN_EMAIL=owner@example.com
WORDPRESS_USER=taka_editor
WORDPRESS_USER_EMAIL=editor@example.com
```

Receiving email is not required for account creation or password-based login:
the entrypoint uses
[`wp core install --skip-email`](https://developer.wordpress.org/cli/commands/core/install/)
and creates the regular user without
[`--send-email`](https://developer.wordpress.org/cli/commands/user/create/).
Use reachable addresses if you need email notifications or password recovery.
The required login domain and loopback addresses may also be correct for this
VM. Copying the file alone does not create secrets, configure DNS, or prepare
Docker storage.

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

WordPress usernames must use ASCII letters, digits, `.`, `_`, or `-` and
fit WordPress's 60-character limit. The two usernames must differ even when
case is ignored. The administrator username must not contain `admin` or
`administrator`, regardless of capitalization. The two email addresses must
be valid and [different](https://developer.wordpress.org/reference/functions/wp_insert_user/).
Keep passwords out of `.env`.

Use an address such as `owner@example.com`, with a local part, `@`, and a
domain. WordPress performs further
[email-format validation](https://developer.wordpress.org/reference/functions/is_email/)
during account creation. The project entrypoint rejects blank values, missing
`@`, and whitespace; `make check-config` does not validate WordPress account
fields. Inspect `make logs` if account creation fails; stop following with
`Ctrl-C`.

At startup, the entrypoint creates accounts whose configured login does not
already exist. Editing `.env` does not rename an existing account or change
its email. Changing a username to a new login creates another account and
keeps the old one. Update existing users separately.

### 3. Create local secrets

Generate the password files expected by the Compose top-level `secrets` section:

```sh
./srcs/tools/generate-secrets.sh
```

The script sets the secret directory to mode `700`, creates the following
files with random values and mode `600`, and leaves existing files untouched:

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

The VM's Docker daemon must use `/home/takawaka/data/docker` as its data-root.
Plain local named volumes are stored at:

```text
/home/takawaka/data/docker/volumes/inception_mariadb_data/_data
/home/takawaka/data/docker/volumes/inception_wordpress_data/_data
/home/takawaka/data/docker/volumes/inception_backup_data/_data
```

For a fresh VM with no Docker data to preserve, merge `srcs/docker/daemon.json`
into `/etc/docker/daemon.json` as root, preserving other settings. Validate with
`dockerd --validate --config-file=/etc/docker/daemon.json`, restart Docker,
and check `docker info --format '{{.DockerRootDir}}'`. Do not change a populated
installation without the migration procedure below. Changing data-root alone
does not move data. Docker's containerd image store may remain in
`/var/lib/containerd`; only the volume storage must live below the required path.

Run the read-only preparation checks:

```sh
make check-config
```

`make check-config` checks the environment
file, nonempty owner-only secret files, the Compose model, Docker access,
data-root, and existing volume options. It creates or changes nothing. Compose
creates the volumes when the application starts; no `driver_opts` or bind mounts
are used for persistent application data.

`make prepare` first checks whether the empty system Docker daemon needs
recovery after evaluation cleanup, then runs `make check-config`'s checks.
It does not delete data or generate credentials. See the reset procedure below.

The project name is `inception`, independent of the clone directory. `LOGIN`
defaults to `takawaka` even when running as root. `DATA_DIR` changes the expected
data-root (`DATA_DIR/docker`), not the daemon configuration. A deliberate
`COMPOSE_PROJECT_NAME` override uses separate containers and volumes for tests;
its host ports must also be isolated from the main application.

## Build and launch

Validate the resolved Compose model before building:

```sh
docker compose -f srcs/docker-compose.yml --env-file srcs/.env config --quiet
```

Build and start the complete stack:

```sh
make
```

`make` is equivalent to `make up`:

```sh
make up
```

Use `make build` when you only want to build images. It builds the active
services one at a time to limit disk and memory pressure in the VM.
`make up` runs that build first, then starts the stack with `up -d --no-build`
in detached mode. Running it again reuses unchanged Docker build layers and
the existing named volumes. It does not reset the installed site.

## Manage containers and logs

| Command | Purpose |
| --- | --- |
| `make` / `make up` | Check configuration, build active services and start the stack. |
| `make build` | Check configuration and build active service images without starting containers. |
| `make check-config` | Validate local files, Compose, and Docker storage without mutation. |
| `make test` | Run isolated mocked recovery, cleanup and administrator-helper checks. |
| `make ps` | Display the current service state and published ports. |
| `make logs` | Follow logs from all services; `Ctrl-C` stops following and leaves containers running. |
| `make browser` | Open the default HTTPS address from the VM graphical session. |
| `make stop` | Stop containers without removing them. |
| `make start` | Start existing stopped containers. |
| `make restart` | Restart containers without rebuilding images. |
| `make backup` | Create one completed backup generation immediately. |
| `make restore-check` | Create a fresh backup and verify restoration in isolated temporary storage. |
| `make down` | Remove containers and the project network. |
| `make clean` | Remove containers and orphan containers while preserving data. |
| `make fclean` | As takawaka with the installed helper, delete all Docker resources and `/home/takawaka/data/*`, including database, WordPress and backups. |

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
  Restore-check service is a one-shot service in the `ops` profile and is not started by the
  default stack.
- Restore-check uses the separate `restore-db` service on the internal
  `restore` network (`inception_restore`). Both services belong to the `ops`
  profile. The temporary database container is removed after verification.
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
  database secret. Restore-check uses only the backup volume and connects to an
  ephemeral MariaDB service through an internal private network. That service
  has no host port and no live application data volume.
- MariaDB is not published to the host.

Do not hard-code container IP addresses because they can change when containers
are recreated.

NGINX depends on healthy WordPress with `restart: true` for Compose operations
that include dependent services. A targeted `up wordpress` may leave NGINX
running, so NGINX also resolves its variable FastCGI upstream through Docker DNS
(`127.0.0.11`, cached for five seconds). This handles a replacement WordPress
container with a different IP without requiring a proxy restart. These settings
are separate from crash restart policies.
WordPress is healthy only after PHP-FPM accepts a connection on port 9000 and
its database is installed. Waiting for PHP-FPM also ensures the entrypoint has
finished creating both users and enabling Redis before dependent services start.
Adminer and FTP bind to `ADMIN_BIND_ADDRESS=127.0.0.1` by default. See USER_DOC
for SSH forwarding, including the FTP passive ports. The static site remains
on host port 8080 and contains no credential form.

## Data persistence

The intended persistent stores are:

| Data | Host location | Consumer |
| --- | --- | --- |
| MariaDB database | `/home/takawaka/data/docker/volumes/inception_mariadb_data/_data` | MariaDB |
| WordPress files | `/home/takawaka/data/docker/volumes/inception_wordpress_data/_data` | WordPress, NGINX, FTP and Backup |
| Backup generations | `/home/takawaka/data/docker/volumes/inception_backup_data/_data` | Backup and Restore-check |

Container writable layers are disposable. Persistent state must live in the
named volumes. `make down`, `make clean`, and container recreation should not
delete this data.

Every mount of `wordpress_data` uses `volume.nocopy: true`. WordPress's
entrypoint initializes an empty volume from `/usr/src/wordpress`. This prevents
NGINX's packaged default HTML from being copied into the shared volume when
Compose creates its container, before WordPress starts.

Inspect volumes with:

```sh
docker volume ls
docker volume inspect inception_mariadb_data inception_wordpress_data
```

Do not run `docker compose down --volumes`, `docker volume rm`, or
`docker volume prune` unless data deletion is intentional and a verified backup
exists.

### Migrate the previous `srcs` project without discarding data

Perform this as the VM administrator during a maintenance window. Keep a
verified backup, the previous source tree/Compose file, image IDs, and daemon
configuration outside `/home/takawaka/data`. Check free space before copying.

1. Stop the old application with its original Compose file and project name
   `srcs`. Keep its containers and three old volumes for rollback.
2. Archive the stopped database, WordPress files, and backups, preserving numeric
   ownership, modes, ACLs, and extended attributes. Verify the archive checksum.
3. Stop `docker.service` and `docker.socket`. Confirm all containers are stopped
   and the old volume mounts are released. On the supplied VM, `/var/lib/docker`
   and `/home/takawaka/data` share a filesystem: move the Docker directory to
   `/home/takawaka/data/docker` without copying its large image/cache contents.
4. Merge the new data-root setting, validate it, and start Docker. Keep the
   containerd storage unchanged. The old `srcs` containers must remain stopped.
5. Create plain local volumes named `inception_mariadb_data`,
   `inception_wordpress_data`, and `inception_backup_data`, labelled with
   `com.docker.compose.project=inception` and the matching
   `com.docker.compose.volume` key. Verify they are empty. Copy the corresponding
   stopped data into their inspected mountpoints, preserving ownership and ACLs.
6. Run `make check-config`, build, and start the new project. Compare database
   users, posts, comments, file hashes, permissions, and backup checksums with
   the recorded baseline. Verify TLS, restart, and restoration.

Do not delete the old directories or volumes as part of this migration. Before
accepting new user writes, rollback is: stop the new project, restore the old
Compose source, stop Docker, move its data-root back, restore daemon settings,
start Docker, and start the old `srcs` project. If the new site has accepted
writes, first preserve those changes; the original volumes are then stale.

### Evaluation reset

The evaluation removes all Docker resources, then deletes
`/home/takawaka/data/*` while Docker is still running. This also removes its
metadata. Make invokes `srcs/tools/prepare-storage.sh` before configuration
checks and Compose builds. When the data-root is missing, the script verifies
the local system daemon and the absence of all containers, then uses the
installed helper to restart `docker.service` once. Docker recreates its
storage and Compose can build a fresh installation.

Ordinary builds leave the daemon running. If the endpoint is remote, containers
remain, the configured path differs, or recovery fails, Make stops before
building. No cleanup command is run automatically by Make. Deleted application
data is not restored automatically. Retain any required backups and migration
archives outside the cleanup path, and keep `.env` and secrets in the repo's
ignored local files. A separate containerd store must also be considered when
checking that images have been removed.

Use a disposable VM for the complete deletion test. The exact procedure is in
[STORAGE_RECOVERY.md](srcs/docker/STORAGE_RECOVERY.md). `make test` covers
recovery decisions, full cleanup safeguards and Make's execution order with
mocked system commands; it does not require Docker or administrator access.

## Peer-review preparation and technical checks

This guide and the [USER_DOC walkthrough](USER_DOC.md#peer-review-walkthrough)
cover the supplied evaluation checklist. Use the
[item-by-item command checklist](USER_DOC.md#review-command-checklist) to
find the minimal commands and detailed demonstrations. Only the official
submitted source is evaluated, and the learner must be present. Review automation and the
transparent shell helpers with the evaluator before using them. If a required
check fails, investigate it before moving on to bonuses.

### 1. Clone and identify the submitted repository

Use the SSH repository URL shown on the current 42 Intra project page, not an
old working copy or an unsubmitted local tree. On the workstation, the following
connection forwards its already-unlocked SSH agent to the VM:

```sh
ssh-add -l
ssh -A -p 2222 takawaka@127.0.0.1
```

Inside that SSH session, as the ordinary VM user in Bash:

```sh
ssh-add -l
review_clone_parent=$(mktemp -d "$HOME/inception-review-clone.XXXXXX")
git clone 'REPLACE_WITH_OFFICIAL_INTRA_SSH_REPOSITORY_URL' "$review_clone_parent/inception"
cd "$review_clone_parent/inception"
git remote get-url origin
git rev-parse HEAD
git status --short
type -a git docker make
alias
```

Replace the quoted repository URL before executing the clone. The new parent
is empty and the clone directory does not exist yet. Confirm owner, project,
submitted commit, and a clean tracked worktree. The command-location and alias
checks help expose replacements for the real tools; read suspicious functions
or wrappers. If the repository is empty, stop and verify the submission.

Agent forwarding works only in the `ssh -A` session while the workstation's
agent can sign with a key registered to the account. An independent VM desktop
terminal does not inherit that forwarded agent. To clone directly from the VM
desktop, its own SSH agent must have an unlocked key registered with 42 Intra;
check with `ssh-add -l` and use the same clone commands above.
A prompt reading `Enter passphrase for key`
refers to the private SSH key; an Intra web password does not unlock that key.
Clone as the ordinary user. Use the VM's graphical terminal for
browser checks.

### 2. Prepare the review shell and inspect the submission

Complete the environment, secret, name-resolution, and storage setup above.
Do not overwrite an existing environment file. Review its non-secret settings
and prepare the local secrets; valid demo account values may remain.
Use the ordinary takawaka shell from the clone; the storage helper supplies
the reset/recovery permissions automatically:

```sh
unset COMPOSE_PROJECT_NAME
dc() {
  command docker compose -f srcs/docker-compose.yml --env-file srcs/.env "$@"
}
wp_review() {
  dc exec -T wordpress wp --allow-root --path=/var/www/html "$@"
}
pwd
id -u
docker context show
docker info --format '{{.DockerRootDir}}'
dc config --quiet
```

Expect the takawaka account, the official clone as the current directory, a
local VM Docker daemon, and data-root `/home/takawaka/data/docker`. Read the active endpoint
with `docker context inspect` and check `DOCKER_HOST`/`DOCKER_CONTEXT` if set.
Never perform the reset against a workstation or remote daemon. The helpers
are already loaded from takawaka's `~/.bashrc` on the supplied VM. Use
`type dc wp_review` to show their definitions; an existing Bash terminal can
reload them with `source ~/.bashrc`. On a fresh VM, define them again in a
new shell or after reboot.

Check required files, README format, and tracked credential candidates:

```sh
for review_file in Makefile README.md USER_DOC.md DEV_DOC.md srcs/docker-compose.yml; do
  if test -s "$review_file"; then
    printf 'OK: %s\n' "$review_file"
  else
    printf 'FAIL: missing or empty %s\n' "$review_file"
  fi
done
head -n 1 README.md
grep -nE '^## (Description|Instructions|Resources)$|^### AI usage$' README.md
git ls-files srcs/.env .env secrets '*.key' '*.pem' '*.p12' '*.pfx'
git check-ignore -v srcs/.env secrets/*.txt
git grep -Il -i -E 'PRIVATE KEY|API[_-]?KEY|TOKEN|PASSWORD|PASSWD' -- srcs Makefile
```

The first README line must be the italicized curriculum statement with the
actual login. Required documentation must be nonempty, and Resources must
explain AI use. Only `secrets/.gitkeep` may appear in the credential-file
listing. The final command prints filenames rather than credential values;
references to secret files and variable names are legitimate matches.
Manually inspect candidates and the submitted source, and check Git history
if needed. These searches alone cannot prove that no credential was committed.
Stop if actual credentials are in the submitted repository. Do not display
the contents of local secret files, `wp-config.php`, Redis's rendered
password configuration, or SQL dumps in review logs.

Inspect build definitions and runtime scripts:

```sh
less Makefile srcs/docker-compose.yml
git ls-files 'srcs/**/Dockerfile' 'srcs/**/*.sh'
find srcs/requirements -name Dockerfile -empty -print
less srcs/requirements/*/Dockerfile srcs/requirements/bonus/*/Dockerfile
less srcs/tools/*.sh srcs/docker/*.sh srcs/tests/*.sh \
  srcs/requirements/*/tools/*.sh srcs/requirements/bonus/*/tools/*.sh
git grep -n -E '^(FROM|ENTRYPOINT|CMD)[[:space:]]' -- 'srcs/**/Dockerfile'
git grep -n -E 'network_mode:[[:space:]]*host|^[[:space:]]*links:|--link([=[:space:]]|$)|sleep[[:space:]]+infinity|tail[[:space:]]+-f|while[[:space:]]+(true|:)([[:space:];]|$)' \
  -- Makefile srcs/docker-compose.yml 'srcs/**/*.sh' 'srcs/**/Dockerfile'
dc --profile ops config --services
dc --profile ops config --images
make -n up
make test
```

Use `q` to leave `less` and `:n` to move to its next file. The `find` command
must print no empty Dockerfile. Expect no forbidden-pattern matches
(git grep exit `1` means no match).
Read the full Compose file, every Dockerfile, and every entrypoint script:
pattern searches do not detect every form of background process or infinite
loop. The bounded dependency retries and loops over finite input are not
keepalive loops. Shell `&&` chains are not background execution.

There are ten nonempty Dockerfiles, including the two on-demand restore
services. Each starts with `FROM debian:12`; WordPress and MariaDB do not
install NGINX. Verify the penultimate stable release against
[Debian's release list](https://www.debian.org/releases/) on the evaluation
date: Debian 12 is oldstable while Debian 13 is stable as of 2026-10-10.
Every service has a build context and an image named `<service>:inception`;
ready-made service images are not used. The Make dry run must show a loop that
builds the active services one at a time after storage and configuration checks,
followed by Compose `up -d --no-build`. `make test` runs isolated mocked storage
and cleanup checks and does not build or touch the live daemon.

### 3. Perform the evaluation reset

**Destructive: the checklist deletes every Docker container, image, volume,
and removable network in this VM, then all data under
`/home/takawaka/data/*`. This includes the backup volume.** Obtain agreement
from the reviewer and the VM owner, and use the dedicated evaluation VM.
The source clone, environment file, secrets, and any required archive must
be outside the deletion path. Preserve a verified VM snapshot or external
backup before deleting data that must be kept.

If the current stack contains data to keep, one way to export a completed
backup is below. Run only with a working stack and the default project name:

```sh
make restore-check
review_backup_name=$(dc exec -T backup find /var/backups/inception \
  -mindepth 1 -maxdepth 1 -type d -name '20*' -printf '%f\n' | LC_ALL=C sort | tail -n 1)
dc stop backup
review_saved_backups=$(mktemp -d /home/takawaka/inception-saved-backups.XXXXXX)
test -n "$review_backup_name" \
  && docker cp "$(dc ps -aq backup):/var/backups/inception/$review_backup_name" "$review_saved_backups/"
(cd "$review_saved_backups/$review_backup_name" && sha256sum -c SHA256SUMS)
printf 'External backup directory: %s\n' "$review_saved_backups"
```

Every command must succeed before proceeding. Keep this directory private:
the archive includes database data and `wp-config.php`. If canceling the
reset, use `dc start backup` to resume its scheduler. A generation left only
in `inception_backup_data` does not survive the reset.

From the takawaka shell in the clone, perform the checklist's full reset:

```sh
make fclean
docker ps -aq
docker images -qa
docker volume ls -q
docker network ls
```

`fclean` runs `srcs/tools/fclean.sh`. It checks administrator helper permission,
a canonical `/home/<login>/data` path, source/configuration outside that path, the local
system Docker socket, and the matching Docker data-root before any deletion.
It stops and removes all containers, removes all images and volumes, and
removes all custom networks. Empty lists are skipped. The
[`type=custom` filter](https://docs.docker.com/reference/cli/docker/network/ls/#type)
preserves the built-in `bridge`, `host`, and `none` networks.
After verifying no resources or nested mounts remain, it deletes the
non-hidden entries under `DATA_DIR`, matching the checklist's final `data/*`
command. A failure stops the sequence; resolve it before continuing.
The installed helper supplies the administrator permissions requested by the
checklist's `sudo` command. It rechecks the fixed path, empty Docker state and
remaining mounts before deleting data. The sudo policy permits only its three
fixed operations without a password.

Then start the fresh installation from the same takawaka shell:

```sh
make
make check-config
make ps
docker info --format '{{.DockerRootDir}}'
```

Keep Docker running during the prescribed deletion sequence. If its data-root
was removed, `make` performs the guarded daemon restart documented in
[STORAGE_RECOVERY.md](srcs/docker/STORAGE_RECOVERY.md), then builds and starts
a fresh installation. It does not recover deleted content or backups.
Do this reset once before the functional review, **not** during the later
persistence test. Continue with
[USER_DOC Step 1](USER_DOC.md#1-start-and-define-the-review-commands).

### 4. Inspect the running infrastructure

Use the helpers from Step 2 after the startup. Confirm images, restart policies,
and the actual foreground PID 1 processes:

```sh
dc ps --all
docker inspect $(dc ps -q) --format \
  '{{.Name}} image={{.Config.Image}} restart={{.HostConfig.RestartPolicy.Name}} state={{.State.Status}}{{if .State.Health}} health={{.State.Health.Status}}{{end}}'
for review_service in mariadb wordpress nginx redis ftp static-site adminer backup; do
  printf '%s PID 1: ' "$review_service"
  dc exec -T "$review_service" sh -c 'tr "\000" " " < /proc/1/cmdline; printf "\n"'
done
```

Expect image names matching services and `unless-stopped` for all eight
long-running services. PID 1 must be the service (MariaDB, PHP-FPM, NGINX,
Redis, vsftpd, Python HTTP server, PHP HTTP server, or foreground cron),
not a keepalive shell or `tail`. The on-demand ops services are not expected
in the normal running list.

Inspect the explicitly declared bridge network, service-name DNS, unpublished
internal ports, and actual data mounts:

```sh
docker network ls
docker network inspect inception_inception --format \
  'driver={{.Driver}} containers={{range .Containers}}{{.Name}} {{end}}'
dc exec -T wordpress getent hosts mariadb redis
for review_service in mariadb wordpress redis; do
  docker inspect "$(dc ps -q "$review_service")" --format \
    '{{.Name}} published={{json .HostConfig.PortBindings}}'
done
for review_service in mariadb wordpress nginx ftp backup; do
  docker inspect "$(dc ps -q "$review_service")" --format \
    '{{.Name}}{{range .Mounts}}{{if eq .Type "volume"}}{{printf "\n  %s -> %s (rw=%v)" .Name .Destination .RW}}{{end}}{{end}}'
done
```

Expect `inception_inception` with driver `bridge`, names resolved through
Docker DNS, and no host port bindings for MariaDB, WordPress, or Redis.
`EXPOSE` in a Dockerfile does not publish a host port. MariaDB mounts
`inception_mariadb_data` at `/var/lib/mysql`. WordPress, NGINX, FTP, and
Backup share `inception_wordpress_data`; NGINX and Backup have read-only
mounts. Backup also mounts `inception_backup_data`. The isolated restore
network is created only for the ops check.

### 5. Change a service configuration and restore it

The reviewer chooses the service and an available new port. This example
changes NGINX's **host** HTTPS port from 443 to 8443 while its container
continues listening on 443. Use the chosen port consistently if it differs.
This is a temporary demonstration; restore the mandatory port 443 afterward.
Check all published ports as well as listeners before choosing the new port:

```sh
ss -ltn '( sport = :8443 )'
docker ps --format 'table {{.Names}}\t{{.Ports}}'
review_port_backup=$(mktemp -d /home/takawaka/inception-port-change.XXXXXX)
cp -p srcs/docker-compose.yml "$review_port_backup/docker-compose.yml"
cp -p srcs/requirements/nginx/conf/nginx.conf "$review_port_backup/nginx.conf"
wp_review option get home > "$review_port_backup/home.txt"
wp_review option get siteurl > "$review_port_backup/siteurl.txt"
review_domain=$(dc exec -T wordpress printenv DOMAIN_NAME)
: "${review_domain:?Could not read DOMAIN_NAME from WordPress}"
vim srcs/docker-compose.yml
```

Change the NGINX ports entry from `"443:443"` to `"8443:443"`.
Do not change `DOMAIN_NAME` to include a port: the entrypoints require a
hostname. Also edit `srcs/requirements/nginx/conf/nginx.conf` and add this
line after `fastcgi_param HTTPS on;` in the PHP location:

```nginx
fastcgi_param HTTP_HOST $host:8443;
```

The packaged `fastcgi_params` can pass the hostname to PHP without its port.
WordPress then repeatedly redirects to the URL containing 8443. This explicit
parameter passes the normalized hostname with the chosen public port.
Existing WordPress URL options are stored in MariaDB and are not rewritten
by merely recreating containers, so update them explicitly:

```sh
git diff -- srcs/docker-compose.yml srcs/requirements/nginx/conf/nginx.conf
dc config --quiet
wp_review option update home "https://$review_domain:8443"
wp_review option update siteurl "https://$review_domain:8443"
make up
make ps
dc exec -T nginx nginx -t
dc port nginx 443
curl --noproxy '*' --insecure --fail --silent --show-error --location \
  --resolve "$review_domain:8443:127.0.0.1" \
  --output /dev/null --write-out 'HTTPS status: %{http_code}\n' \
  "https://$review_domain:8443/"
openssl s_client -connect 127.0.0.1:8443 -servername "$review_domain" \
  -tls1_2 -brief </dev/null
```

Expect port 8443, HTTP status `200` over HTTPS, and a successful TLS handshake.
Open `https://takawaka.42.fr:8443/` and `/wp-admin/` manually in the VM
browser; `make browser` opens the original default-port URL. Demonstrate
login and the saved page/comment. `make up` rebuilds images and recreates
containers whose configuration changed. `make restart` alone does not apply
new port mappings or rebuild images.

Return Compose, the NGINX configuration, and WordPress URL options to their
recorded values:

```sh
cp -p "$review_port_backup/docker-compose.yml" srcs/docker-compose.yml
cp -p "$review_port_backup/nginx.conf" srcs/requirements/nginx/conf/nginx.conf
wp_review option update home "$(cat "$review_port_backup/home.txt")"
wp_review option update siteurl "$(cat "$review_port_backup/siteurl.txt")"
make up
make ps
dc port nginx 443
curl --noproxy '*' --insecure --fail --silent --show-error \
  --resolve "$review_domain:443:127.0.0.1" \
  --output /dev/null --write-out 'HTTPS status: %{http_code}\n' \
  "https://$review_domain/"
git diff -- srcs/docker-compose.yml srcs/requirements/nginx/conf/nginx.conf
```

Expect the original mapping, working WordPress, and no remaining port-change
diff. If the changed configuration fails, use these recorded values to restore
service before investigating. Do not roll back by deleting volumes or using
`git reset --hard`.

#### If the reviewer chooses another service

Save every source or environment file you will edit outside
`/home/takawaka/data`, as in the NGINX example. Search the tracked source for
the relevant port, then update the listener, its clients, and any TCP
healthcheck together:

```sh
git grep -n -E '443|9000|3306|6379|8080|21100|21110' \
  -- srcs/docker-compose.yml srcs/requirements
```

Paths in this table are relative to `srcs/requirements/` unless stated
otherwise. Keep internal-only services unpublished.

| Selected service | Places that must agree |
| --- | --- |
| NGINX container listener | `nginx/conf/nginx.conf` (`listen`), the target port in Compose, and the host port if changed. For a listener at 8443, use `"8443:8443"`, pass `HTTP_HOST` with the public port, and update WordPress URL options as above; inspect with `dc port nginx 8443`. |
| WordPress/PHP-FPM | `wordpress/conf/www.conf` (`listen`), `nginx/conf/nginx.conf` (`wordpress_backend`), and WordPress's `fsockopen` healthcheck in Compose. Update 9000 consistently; the browser still uses HTTPS 443. |
| MariaDB | `mariadb/conf/mariadb.cnf` (`port`), `wordpress/tools/entrypoint.sh` (`DB_PORT`), `bonus/backup/tools/backup.sh` (client port), and the existing WordPress `DB_HOST` constant. Adminer's server becomes `mariadb:NEW_PORT`; add `--port=NEW_PORT` to the TCP SQL-login command. MariaDB's healthcheck uses its Unix socket. |
| Redis | `bonus/redis/conf/redis.conf` (`port`), `wordpress/tools/entrypoint.sh` (both `REDIS_PORT` and the PHP readiness probe's literal 6379), and the existing WordPress `WP_REDIS_PORT` constant. Redis's healthcheck uses its Unix socket. |
| Static site / Adminer | For a host-port change, edit the left port in Compose and use the new browser/curl address. For a container-port change, also update the server's `CMD` in `bonus/static-site/Dockerfile` or `bonus/adminer/Dockerfile` and the Compose target port. Preserve Adminer's loopback binding. |
| FTP | For a control-listener change, add `listen_port=NEW_PORT` in `bonus/ftp/conf/vsftpd.conf` and update the Compose control mapping and client/tunnel port. For a passive-range change, update both `pasv_min_port`/`pasv_max_port` and every matching published/forwarded port. Verify a transfer, not just login. |
| Backup | This service has no listening port. If chosen for another configuration change, edit `BACKUP_SCHEDULE` in `srcs/.env`, recreate with Make, inspect `/etc/cron.d/inception-backup`, and demonstrate a backup/restore check. |

If a container listener changes, also update its Dockerfile's `EXPOSE`
metadata where present; `EXPOSE` alone does not change the listener or publish
a port. Do not replace port-like numbers inside download checksums.

An existing `wp-config.php` is preserved by the entrypoint. For a MariaDB
change to 3307, record its non-secret host setting and update it **before**
rebuilding, while the old WordPress container still runs:

```sh
review_db_host_before=$(wp_review config get DB_HOST)
wp_review config set DB_HOST 'mariadb:3307' --type=constant
```

For a Redis change to 6380, use this alternative:

```sh
review_redis_port_before=$(wp_review config get WP_REDIS_PORT)
wp_review config set WP_REDIS_PORT 6380 --type=constant --raw
```

Execute only the commands for the selected service and substitute the chosen
port. Record these original values outside the data directory if changing
shells. Use `dc config --quiet`, `make up`, `make ps`, and the relevant
USER_DOC demonstration to verify the modified service and its dependents.
For MariaDB, verify the public WordPress site, SQL login, Adminer, and
`make restore-check`; for Redis, verify the cross-process cache test.

Restore the saved source files and the matching runtime constant before the
final `make up`. These one-shot commands also work if the WordPress service
is unhealthy: [`wp config set`](https://developer.wordpress.org/cli/commands/config/set/)
edits configuration before WordPress loads, and `--no-deps` avoids starting
dependencies:

```sh
# MariaDB alternative only:
dc run --rm --no-deps --entrypoint wp wordpress \
  --allow-root --path=/var/www/html config set DB_HOST "$review_db_host_before" --type=constant
```

```sh
# Redis alternative only:
dc run --rm --no-deps --entrypoint wp wordpress \
  --allow-root --path=/var/www/html config set WP_REDIS_PORT "$review_redis_port_before" \
  --type=constant --raw
```

Then repeat the service checks at the original ports and compare the source
diff with the saved baseline. Restoring source files alone does not restore
constants already persisted in the WordPress volume.

### MariaDB and persistence commands (6.9 / 6.10)

[USER_DOCのコマンド比較](USER_DOC.md#review-commands-for-69-and-610)に、
6.9・6.10の「必要最小限」と「少し詳しい確認」を併記しています。

| 項目 | 必要最小限 | 少し詳しい確認 |
| --- | --- | --- |
| 6.9 MariaDB | `make ps`、短いDBログイン、`SHOW TABLES;`、ユーザー数 | Dockerfile・volumeの保存先、実行時のDB設定によるログイン、投稿・コメントのSQL確認 |
| 6.10 永続性 | VM再起動後に同じcloneで `make up ps browser`、同じページ・コメント・DBを確認 | 再起動前後のユーザー・本文・コメント・volumeを比較し、必要に応じてJSONとdiffで記録 |

短いDBログインは `.env.example` の初期値を使います。

```sh
dc exec mariadb mariadb -h127.0.0.1 -u wp_user -p wordpress
```

DB名やユーザー名を変えた構成では、コンテナの設定を使います。

```sh
dc exec mariadb sh -c \
  'exec mariadb --protocol=TCP --host=127.0.0.1 --user="$MYSQL_USER" --password "$MYSQL_DATABASE"'
```

どちらも `secrets/db_password.txt` の内容をパスワードのプロンプトへ入力します。
`-h127.0.0.1` はMariaDBコンテナ内のTCP接続先です。
VMでの実行ユーザーは `takawaka`、DBへのログインは `MYSQL_USER` です。
DBの `root` とVMの `root` も別のアカウントです。
再起動後のMakeは `takawaka` で実行できます。永続性を確認するときは、
データを保つ通常の起動を使い、全削除の `make fclean` は評価開始時のリセットで使います。

### Evaluation coverage and explanations

The following table maps the supplied checklist to demonstrations. A command
result does not replace an explanation or the requested browser action.

| Evaluation item | Demonstration |
| --- | --- |
| Preliminaries / General instructions | Official fresh clone, learner present, command/alias review, credential and required-file inspection; guarded VM-only reset in Steps 1-3 above. |
| Activity overview | Explain Docker/Compose, images and containers, VM tradeoffs, and repository layout using the points below and README. |
| README / Documentation | First-line format, Description/Instructions/Resources and AI use; nonempty USER_DOC and DEV_DOC in Step 2. |
| Simple setup / NGINX with SSL/TLS | [USER_DOC Step 2](USER_DOC.md#2-verify-https-and-tls): port 443, closed port 80, both allowed TLS versions, certificate, configured site in browser. |
| Docker Basics | Source inspection in Step 2 and runtime Step 4: ten custom Dockerfiles, allowed base, matching images, Make-driven Compose build, foreground service processes. |
| Docker Network | Compose network definition, bridge network inspection, service-name DNS, and internal-only MariaDB/WordPress/Redis in Step 4. |
| WordPress with php-fpm and its volume | [USER_DOC Steps 3-4](USER_DOC.md#3-inspect-persistent-volumes): actual volume path, installed WordPress, two users, valid admin login, browser comment and page edit. |
| MariaDB and its volume | [USER_DOC Steps 3-4](USER_DOC.md#4-demonstrate-wordpress-and-database-access): actual volume path, authenticated SQL session, nonempty tables and users. |
| Persistence! | [USER_DOC Step 5](USER_DOC.md#5-verify-persistence-across-recreation-and-vm-reboot): actual VM reboot, Make startup, unchanged review content, users, database, and volume paths. |
| Configuration modification | Step 5 above: available new port, WordPress URL options, rebuild/recreate, browser verification, and return to port 443. |
| Bonus | [USER_DOC Step 7](USER_DOC.md#7-demonstrate-every-bonus-service): Redis cross-process cache, FTP transfer/shared volume, non-PHP static site, Adminer database login, scheduled backup and verified temporary restoration. |

Prepare these short explanations in your own words:

- **Docker and Compose:** a Dockerfile builds an image; a container runs that
  image. Compose describes and coordinates services, networks, volumes,
  dependencies, and secret-file mounts. The image format is the same whether
  started by `docker run` or Compose; Compose supplies the multi-service
  configuration.
- **Docker and VMs:** the required VM has its own guest kernel; these containers
  share that kernel while isolating service processes and filesystems. Rebuilding
  a container is lighter than provisioning a separate VM per service.
- **Layout and networking:** the root Makefile is the entry point, `srcs/`
  contains service configuration/build sources, and ignored local `secrets/`
  holds credentials. A user-defined bridge provides service-name DNS; host
  networking and legacy links are unnecessary.
- **Processes and persistence:** entrypoints initialize once and use `exec`
  for the real foreground service. Recreating a container replaces its writable
  layer; named volumes preserve the database and WordPress files. Moving Docker's
  data-root places native volume data below the required login data directory.
- **TLS and credentials:** TLS 1.2/1.3 are negotiated protocol versions, not
  certificate versions. The self-signed certificate identifies the login domain;
  passwords are local secret files, not image layers or command-line arguments.
- **Free-choice bonus:** foreground cron schedules atomic, checksum-protected
  database/file backups with retention. A separate temporary MariaDB and
  WordPress extraction demonstrate that a backup can actually be restored.

## Documentation maintenance

Keep `README.md`, `USER_DOC.md`, `DEV_DOC.md`, `srcs/.env.example`, the secret
file names, and the Compose service names synchronized. Update the documentation
whenever commands, paths, ports, credentials, or persistence behavior changes.
