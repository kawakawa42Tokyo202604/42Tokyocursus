# User Documentation

This document explains how an end user or administrator can operate the
Inception stack.

For a defense, start with the [command checklist](#review-command-checklist),
then use the [peer-review walkthrough](#peer-review-walkthrough) below.
For MariaDB and reboot checks, see the
[minimal and extended commands for 6.9 / 6.10](#review-commands-for-69-and-610).
The [developer review guide](DEV_DOC.md#peer-review-preparation-and-technical-checks)
contains the official-clone checks, destructive evaluation reset, source
inspection, and configuration-change procedure.

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
  Valid demo usernames and email addresses may be used for a local review.
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
in the Docker group and can run ordinary Docker/Make commands. Full evaluation
cleanup and Docker storage recovery use the preinstalled administrator helper
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

### Full cleanup before evaluation

**`make fclean` deletes every Docker container, image, volume and custom
network in the VM, then `/home/takawaka/data/*`. WordPress, database contents
and backups in that directory are permanently deleted.** Export any data to
keep outside that directory first; see the
[evaluation reset](DEV_DOC.md#3-perform-the-evaluation-reset).

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
the configuration for the next startup. Do this reset before the review's
functional checks; use normal restart/recreation for the persistence test.

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

Create a fresh backup and verify the complete restore path:

```sh
make restore-check
```

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

## Review command checklist

提供された評価シートの準備・必須11項目・ボーナス5項目に対応する索引です。
6.1〜6.11は、必須項目を評価シートの順に数えた番号です。
レビューは **正式clone → 設定・ソース確認 → 初期リセット → 起動 → 必須項目 → ボーナス**
の順で進めます。各リンク先に、実行するコマンド・期待する結果・画面操作があります。

コマンドはVM内のBashで、正式cloneのルートから `takawaka` として実行します。
`dc`・`wp_review`・`review_domain` は[起動手順](#1-start-and-define-the-review-commands)で
準備します。表は手順への索引なので、詳細欄の前提と成功条件も確認してください。

| 項目 | 必要最小限のコマンド・操作 | 少し詳しい確認・手順 |
| --- | --- | --- |
| 評価前：正式clone | `git clone`、`git remote get-url origin`、`git status --short` | [公式URL・空のclone先・提出コミット・aliasの確認](DEV_DOC.md#1-clone-and-identify-the-submitted-repository) |
| 評価前：設定・認証情報・ソース | `dc config --quiet`、`git ls-files`、`less Makefile srcs/docker-compose.yml` | [必要ファイル・Git内の認証情報・全Dockerfileとスクリプトの確認](DEV_DOC.md#2-prepare-the-review-shell-and-inspect-the-submission) |
| 評価前：全削除・起動 | `make fclean` → `make` → `make ps` | [削除範囲・バックアップ退避・空のDocker資源の確認](DEV_DOC.md#3-perform-the-evaluation-reset) |
| 6.1 Activity overview | Docker・Compose・image/container・VMとの違い・構成を口頭説明 | [説明の要点](DEV_DOC.md#evaluation-coverage-and-explanations) |
| 6.2 README | `head -n 1 README.md`、`less README.md` | [先頭行・必須見出し・AI利用の記載](DEV_DOC.md#2-prepare-the-review-shell-and-inspect-the-submission) |
| 6.3 Documentation | `less USER_DOC.md DEV_DOC.md` | [利用・開発手順が存在し、空でないこと](DEV_DOC.md#2-prepare-the-review-shell-and-inspect-the-submission) |
| 6.4 Simple setup | `make ps`、`make browser`、設定済みWordPressを表示 | [HTTPS成功・HTTP接続不可・インストール画面が出ないこと](#2-verify-https-and-tls) |
| 6.5 Docker Basics | Dockerfile確認、`dc config --images`、`make -n up` | [自作image・ベース・ビルド手順](DEV_DOC.md#2-prepare-the-review-shell-and-inspect-the-submission)、[再起動方針・PID 1](DEV_DOC.md#4-inspect-the-running-infrastructure) |
| 6.6 Docker Network | `docker network ls`、`docker network inspect inception_inception` | [bridge・サービス名DNS・内部ポート](DEV_DOC.md#4-inspect-the-running-infrastructure) |
| 6.7 NGINX / SSL・TLS | `dc port nginx 443`、HTTP接続不可、TLS 1.2・1.3の接続確認 | [curl・OpenSSL・証明書・NGINX設定のコマンド](#2-verify-https-and-tls) |
| 6.8 WordPress / PHP-FPM / volume | `wp_review user list`、volume確認、一般ユーザーでコメント・管理者でページ編集 | [volumeの保存先](#3-inspect-persistent-volumes)、[アカウント・公開画面での確認](#4-demonstrate-wordpress-and-database-access) |
| 6.9 MariaDB / volume | DBログイン、`SHOW TABLES;`、`SELECT COUNT(*) FROM wp_users;` | [最小版・詳細版のDBコマンド](#review-commands-for-69-and-610) |
| 6.10 Persistence | VM自体を再起動 → 同じcloneで `make up ps browser` → 保存内容・DBを確認 | [最小版](#review-commands-for-69-and-610)、[前後のJSON比較・volume再確認](#5-verify-persistence-across-recreation-and-vm-reboot) |
| 6.11 Configuration modification | 設定を編集 → `make up` → `make ps` → 変更後のサービスを確認 | [NGINXの8443変更と戻すコマンド、他サービスの変更箇所](DEV_DOC.md#5-change-a-service-configuration-and-restore-it) |
| Bonus：Redis | `wp_review redis status`、別プロセスでcacheのset/get | [healthcheck・接続・cache再利用・テストキー削除](#7-demonstrate-every-bonus-service) |
| Bonus：FTP | curlでログイン・アップロード → HTTPSで同じファイルを確認 | [パスワード入力・passive転送・共有volume・テストファイル削除](#7-demonstrate-every-bonus-service) |
| Bonus：Static site | `curl http://127.0.0.1:8080/`、VMブラウザーで表示 | [HTTP・PHPを使わないソースの確認](#7-demonstrate-every-bonus-service) |
| Bonus：Adminer | `http://127.0.0.1:8081/` でDBログイン、WordPressテーブルを表示 | [接続先・HTTP確認・SQL実行](#7-demonstrate-every-bonus-service) |
| Bonus：Backup | cron設定確認、`make restore-check` | [スケジュール・保存世代・チェックサム・一時DBへの復元](#7-demonstrate-every-bonus-service) |
| 確認後：元の設定へ戻す | 保存した設定とWordPressのURL・定数を戻す → `make up` → 動作確認 | [設定変更の復旧手順](DEV_DOC.md#5-change-a-service-configuration-and-restore-it) |

**`make fclean` はVM全体のDocker資源とサイト・DB・バックアップを削除します。**
評価開始時に必要なデータを退避して使い、永続性確認中には実行しません。
6.1の説明、6.8のブラウザー操作、6.10の実際のVM再起動、6.11の指定された変更は
コマンドの出力だけでは完了しません。ボーナスは必須項目がすべて成功した後に確認します。

ログは `make logs` で追いかけ、`Ctrl-C` で終了します。短く読む場合は
`dc logs --tail=100` を使います。期待と違う結果が出たら、その項目を解決してから進めます。

## Review commands for 6.9 and 6.10

`message.txt` の必須項目の順番で、6.9はMariaDBとvolume、6.10はVM再起動後の
永続性の確認です。**VM内のBash端末で、`takawaka` としてcloneのルートから実行します。**
練習用cloneが `~/asd` の場合は、最初に次を入力します。別の名前でcloneした場合は
そのディレクトリへ移動してください。

```sh
cd ~/asd
```

`dc` と `wp_review` は[レビュー用の関数](#1-start-and-define-the-review-commands)です。
用意済みのVMでは新しいBash端末で使えます。既に開いていた端末では
`source ~/.bashrc` で読み直します。新しいVMではリンク先の関数を定義してください。

| 項目 | 必要最小限 | 少し詳しい確認 |
| --- | --- | --- |
| 6.9 MariaDB | 起動状態、DBログイン、テーブル・ユーザー数 | Dockerfile、volumeの保存先、投稿・コメントのSQL確認 |
| 6.10 永続性 | VM再起動、起動、ブラウザーとDBの確認 | 再起動前後のページ・コメント・ユーザー・volumeの比較 |

### 6.9 MariaDB and its volume

**必要最小限**

次は `srcs/.env.example` のDB設定（`MYSQL_USER=wp_user`、`MYSQL_DATABASE=wordpress`）
を使う短いコマンドです。変更した場合は実際の名前に置き換えるか、下の詳細版を使います。

```sh
make ps
dc exec mariadb mariadb -h127.0.0.1 -u wp_user -p wordpress
```

パスワードは `secrets/db_password.txt` の中身です。入力中は文字が表示されません。
MariaDBのプロンプトで次を入力し、最後の `EXIT;` で普通の端末へ戻ります。

```sql
SHOW TABLES;
SELECT COUNT(*) FROM wp_users;
EXIT;
```

MariaDBが `Up`・`healthy` で、WordPressのテーブルとユーザーが存在することを確認します。
初期構成の目安は12テーブル・ユーザー2人です。追加したユーザーやプラグインにより
件数は変わります。レビューでは、次のDockerfile・volumeの確認も合わせて行ってください。

**少し詳しい確認**

```sh
cat srcs/requirements/mariadb/Dockerfile
docker volume ls
docker volume inspect inception_mariadb_data --format '{{.Mountpoint}}'
dc exec mariadb sh -c \
  'exec mariadb --protocol=TCP --host=127.0.0.1 --user="$MYSQL_USER" --password "$MYSQL_DATABASE"'
```

Dockerfileで `mariadb-server` をインストールし、NGINXを含んでいないことを確認します。
volumeの保存先は `/home/takawaka/data/docker/volumes/inception_mariadb_data/_data` です。
最後のコマンドはコンテナのDB設定を使ってログインします。パスワードは最小版と同じです。

```sql
SELECT DATABASE();
SHOW TABLES;
SELECT COUNT(*) AS wordpress_users FROM wp_users;
SELECT post_type, COUNT(*) AS rows_count FROM wp_posts GROUP BY post_type;
SELECT comment_ID, comment_post_ID, comment_approved FROM wp_comments;
EXIT;
```

実際のDB名、ユーザー数、投稿・固定ページ、レビューで追加したコメントを確認します。
SQLの `wp_` は標準のテーブル接頭辞です。変更した場合は実際の接頭辞を使ってください。

### 6.10 Persistence after VM reboot

**必要最小限**

1. ブラウザーで編集済みページと公開コメントを確認し、目印の文字・URLを控えます。
   DBのユーザー数も6.9の最小版で確認します。
2. 編集中のファイルを保存し、VM内のDebianの右上の電源メニューから再起動します。
3. `takawaka` で再ログインし、同じcloneの端末で入力します。

```sh
cd ~/asd
make up ps browser
```

`make up ps browser` は、ビルド・起動、状態表示、ブラウザー起動を順に行います。
別名のcloneでは `cd` の移動先を置き換えます。サイトにインストール画面が出ず、
同じページ・コメントが残り、同じ管理者でログインできることを確認します。
6.9の最小版を再実行し、DBのテーブルとユーザー数も再起動前と比較します。

**少し詳しい確認**

次を再起動前と、再起動後の起動が完了してから実行して比較します。
同じユーザー、ページ本文、コメントと承認状態、volumeの保存先が残ることを確認します。

```sh
wp_review core is-installed
wp_review user list --fields=ID,user_login,roles --format=table
wp_review post list --post_type=page --post_status=publish \
  --fields=ID,post_title,post_content --format=table
wp_review comment list \
  --fields=comment_ID,comment_post_ID,comment_content,comment_approved --format=table
docker volume inspect inception_wordpress_data inception_mariadb_data \
  --format '{{.Name}} path={{.Mountpoint}}'
```

出力をファイルに保存して差分を比較したい場合は、
[再起動前後の記録を残す手順](#5-verify-persistence-across-recreation-and-vm-reboot)を使います。
ブラウザーでの確認も合わせて実施してください。
**永続性の確認中に `make fclean` を実行すると、サイト・DB・バックアップが消えます。**

## Peer-review walkthrough

Follow this order after the reviewer has checked the submitted repository and
completed the [evaluation reset](DEV_DOC.md#3-perform-the-evaluation-reset).
These checks assume the default project name `inception` and run **inside the
VM, from the official clone's root**, as `takawaka`. Docker access and the
storage administrator helper must be configured as described in DEV_DOC.
Browser actions need the user's graphical session.

The shell helpers below spell out the same Compose file and environment file
as the Makefile. Show their definitions to the reviewer with
`type dc wp_review`. On the supplied VM, they are already in takawaka's
`~/.bashrc`; a new Bash terminal loads them, and an existing terminal can use
`source ~/.bashrc`. On a fresh VM, define them as below in each review shell.
Do not source `srcs/.env` as a shell script: its cron value uses Compose's
environment-file syntax.

### 1. Start and define the review commands

From the repository in the ordinary user's terminal, start the project:

```sh
unset COMPOSE_PROJECT_NAME
dc() {
  command docker compose -f srcs/docker-compose.yml --env-file srcs/.env "$@"
}
wp_review() {
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

### 4. Demonstrate WordPress and database access

Check installation, the two WordPress accounts, and the configured URLs:

```sh
wp_review core is-installed && printf 'WordPress is installed.\n'
wp_review user list --fields=ID,user_login,roles --format=table
wp_review option get home
wp_review option get siteurl
wp_review post list --post_type=page --post_status=publish \
  --fields=ID,post_title,post_status --format=table
wp_review comment list \
  --fields=comment_ID,comment_post_ID,comment_author,comment_approved --format=table
```

Expect an administrator and a different regular user (`editor` in this
implementation). The administrator login must not contain `admin` in any
capitalization. The URLs should be `https://takawaka.42.fr` at the default
port. These WordPress accounts are distinct from MariaDB's root and application
database accounts.

Perform the actual browser actions with the reviewer:

1. Log in as the regular WordPress user using the matching local secret.
   Add a distinctive comment to a published post with comments enabled.
   If moderation is enabled, approve it from the administrator dashboard.
2. Log in as the administrator, edit and publish a page, and include a
   distinctive review marker in its content.
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
users, and the review content. The default table prefix is `wp_`; use the
installation's prefix if it was deliberately changed. To demonstrate the
separate MariaDB administrator, use `secrets/db_root_password.txt` privately
at this prompt:

```sh
dc exec mariadb mariadb --user=root --password
```

Run `SELECT User, Host FROM mysql.user;`, then `EXIT;`. Root is local to
MariaDB; the configured application database user connects from the project
network.

### 5. Verify persistence across recreation and VM reboot

Capture the browser changes and user accounts **after** the preceding edits.
Keep evidence outside the data directory that the evaluation deletes:

```sh
review_evidence=$(mktemp -d /home/takawaka/inception-review.XXXXXX)
printf 'Keep this evidence path for after reboot: %s\n' "$review_evidence"
wp_review post list --post_type=page --post_status=publish --orderby=ID --order=ASC \
  --fields=ID,post_title,post_content,post_status --format=json \
  > "$review_evidence/pages.before.json"
wp_review comment list --orderby=comment_ID --order=ASC \
  --fields=comment_ID,comment_post_ID,comment_content,comment_approved --format=json \
  > "$review_evidence/comments.before.json"
wp_review user list --orderby=ID --order=ASC \
  --fields=ID,user_login,roles --format=json > "$review_evidence/users.before.json"
make down
make up
make ps
```

Reopen the public page and comment and confirm both review markers remain.
This tests container recreation, not the required VM reboot. Do not use
`--volumes` or run the evaluation reset again here.

Save open work, then reboot Debian using the power menu inside the VM's
graphical session. After it returns, log in as `takawaka`, open a Bash terminal,
and enter the same clone. If using SSH, reconnect with `ssh -A`.
Unset `COMPOSE_PROJECT_NAME` and check that `dc` and `wp_review` are available
as described in Step 1. Set `review_evidence` to the exact path printed before
reboot; replace the example path below:

```sh
unset COMPOSE_PROJECT_NAME
review_evidence=/home/takawaka/inception-review.REPLACE_WITH_SAVED_SUFFIX
test -d "$review_evidence"
make up
make ps
wp_review core is-installed && printf 'WordPress is still installed.\n'
wp_review post list --post_type=page --post_status=publish --orderby=ID --order=ASC \
  --fields=ID,post_title,post_content,post_status --format=json \
  > "$review_evidence/pages.after.json"
wp_review comment list --orderby=comment_ID --order=ASC \
  --fields=comment_ID,comment_post_ID,comment_content,comment_approved --format=json \
  > "$review_evidence/comments.after.json"
wp_review user list --orderby=ID --order=ASC \
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

### 6. Rebuild after a configuration change

Follow the [NGINX port-change and rollback procedure](DEV_DOC.md#5-change-a-service-configuration-and-restore-it)
when the reviewer requests a configuration modification. The reviewer chooses
the service and an available port; the documented NGINX example changes a
mandatory service. Demonstrate the configured WordPress site, login, and saved
review content at the changed address, then restore the required port 443.

### 7. Demonstrate every bonus service

Evaluate bonuses only after all mandatory checks, including reboot persistence
and the requested configuration change, succeed.

**Redis: connected cache and reuse across separate PHP processes**

```sh
dc exec -T redis /usr/local/bin/healthcheck.sh
wp_review redis status
wp_review cache set review-probe cache-ok inception-review 60
wp_review cache get review-probe inception-review
wp_review cache delete review-probe inception-review
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
The first run also builds the two ops images, so allow extra time on the VM.
The one-shot restore container and temporary database are removed afterward.
The scheduled Backup service must remain running. Explain that backups are
useful because they preserve both the SQL data and WordPress files, and that
the restore test uses temporary storage rather than the live data volumes.
An ordinary reboot preserves these backups; the evaluation reset deletes
their volume unless they were copied outside `/home/takawaka/data`.

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

Docker's own metadata also lives below that directory. Evaluation cleanup
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
