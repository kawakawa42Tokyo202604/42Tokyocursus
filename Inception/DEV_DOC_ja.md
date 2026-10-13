<a id="developer-documentation"></a>

# 開発者向けドキュメント

英語版：[DEV_DOC.md](DEV_DOC.md)

この文書では、Inceptionの開発環境を準備、ビルド、運用、検査する方法を説明します。

<a id="prerequisites"></a>

## 前提条件

次の条件を満たすLinux仮想マシンを使用してください。

- Docker Engine
- Docker Compose v2.17以降（提供されたVMではCompose v5）
- GNU Make
- Git
- 検証コマンドを実行するためのBash
- ソースファイル閲覧用の`less`
- HTTPS検証用の`curl`とOpenSSL
- `/home/takawaka/data`を作成し、Dockerデーモンにアクセスする権限

主要ツールを確認します。

```sh
docker --version
docker compose version
make --version
```

提供されたVMでは、`takawaka`はDockerグループのメンバーであり、すべてのMakeターゲットを実行できます。root所有のストレージ用ヘルパーには、`check`、`clean-data`、`recover`に限定されたパスワード不要のsudo権限が設定されています。Makeスクリプトは管理者権限が必要なときに自動的にヘルパーを呼び出します。`make fclean`とその直後の最初の`make`も`takawaka`として実行してください。`make browser`はグラフィカルセッションから使用します。

<a id="one-time-docker-group-setup-on-a-fresh-vm"></a>

### 新規VMでのDockerグループ設定（初回のみ）

Dockerインストール後、VM管理者が通常ユーザーを追加します。

```sh
su root -c '/usr/sbin/usermod -aG docker takawaka'
```

作業中のファイルを保存し、VMのグラフィカルデスクトップからログアウトした後、`takawaka`で再ログインします。端末を開き、次のコマンドで確認します。

```sh
id -nG
docker ps
```

現在のセッションの所属グループに`docker`が含まれ、デーモンにアクセスできることを確認してください。古いデスクトップセッション内で新しい端末を開くだけでは、グループ情報は更新されません。`usermod`を絶対パスで指定しているため、管理者シェルのPATHに`/usr/sbin`がなくても動きます。クリーンアップとデーモン復旧には、後述のストレージ用ヘルパーの設定も必要です。

<a id="one-time-storage-permission-setup-on-a-fresh-vm"></a>

### 新規VMでのストレージ権限設定（初回のみ）

後述のDocker data-rootを設定した後、VM管理者がリポジトリ内のヘルパーを一度インストールします。

```sh
su root -c 'sh srcs/tools/install-storage-admin.sh'
```

インストーラーには`sudo`と`visudo`が必要です。権限ポリシーを検証し、root所有のコピーを`/usr/local/sbin/inception-storage-admin`（755）と`/etc/sudoers.d/inception-storage`（440）へインストールします。ヘルパーはローカルシステムのDockerソケットと固定パス`/home/takawaka/data`を使い、Docker接続先の上書き設定を解除してシステムのPATHを使用します。任意のMake、シェル、編集可能なリポジトリスクリプトの実行を許可するものではありません。ヘルパーのソースを変更した場合は、VM管理者として新しいコードを再インストールしてください。通常のビルドではシステム権限をインストール・変更しません。

<a id="repository-layout"></a>

## リポジトリ構成

```text
.
├── Makefile
├── README.md
├── README_ja.md
├── USER_DOC.md
├── USER_DOC_ja.md
├── DEV_DOC.md
├── DEV_DOC_ja.md
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
    │   ├── test-service-regressions.sh
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

必須サービスはそれぞれ対応するDockerfileからビルドする必要があります。イメージ名とサービス名は一致させてください。ビルド済みのNGINX、WordPress、MariaDBサービスイメージは使用せず、許可されたDebianまたはAlpineのベースイメージのみをpullできます。

<a id="docker-storage-files"></a>

### Dockerストレージ関連ファイル

`srcs/docker/`には、VMのDockerデーモンのストレージ設定と管理用ユーティリティがあります。各サービスのDockerfileは`srcs/requirements/`以下にあります。

| ファイル | 用途 |
| --- | --- |
| `daemon.json` | `data-root`を`/home/takawaka/data/docker`に設定し、Dockerボリュームを指定の保存先に配置するための設定例。 |
| `inception-storage-admin.sh` | 権限を限定した管理操作を実行します。`check`（保存先の検証）、`clean-data`（Docker資源削除後のデータ削除）、`recover`（data-root消失時のDocker再起動）。 |
| `inception-storage.sudoers` | インストール済みヘルパーの上記3操作だけを、`takawaka`にパスワードなしで許可する設定。 |
| `STORAGE_RECOVERY.md` | 完全クリーンアップ後の復旧の前提条件、検証、制約を説明する文書。 |

[初期設定](#4-prepare-persistent-storage)では既存の設定を残したまま、`daemon.json`の内容を`/etc/docker/daemon.json`へ反映してください。サンプルファイルをリポジトリ内に置いただけではDockerの設定は変わりません。[管理者による初回設定](#one-time-storage-permission-setup-on-a-fresh-vm)ではヘルパーとsudoポリシーのroot所有のコピーを固定場所へインストールします。以後は`takawaka`からMake経由で許可された管理操作を呼び出せます。

呼び出しの流れは次のとおりです。

- `make fclean` → `srcs/tools/fclean.sh` → ヘルパーの`check`と`clean-data`。
- 完全クリーンアップ後の`make` → `srcs/tools/prepare-storage.sh` → 必要な場合のみヘルパーの`recover`。

`clean-data`はコンテナ、イメージ、ボリューム、カスタムネットワークが残っていると実行を拒否します。`recover`は、Dockerを再起動する前にストレージが消失していることとコンテナが残っていないことを確認します。削除済みのWebサイトやDBの内容をバックアップから復元することはありません。

<a id="set-up-the-environment-from-scratch"></a>

## 環境を一から構築する

<a id="1-configure-local-name-resolution"></a>

### 1. ローカル名前解決を設定する

サイトへのアクセス元マシンで、VMのIPアドレスと必要なドメインを対応付けます。

```text
<VM_IP_ADDRESS> takawaka.42.fr
```

Linuxでは通常`/etc/hosts`に追加します。これはホスト側の設定であり、リポジトリには含まれません。

VM内部のブラウザーを使用する場合は、VMの`/etc/hosts`に`127.0.0.1 takawaka.42.fr`を追加します。`getent ahostsv4 takawaka.42.fr`で確認し、グラフィカルユーザーとして次を実行します。

```sh
make browser
```

起動スクリプトは`DOMAIN_NAME`を読み取り、通常の名前解決を確認して、Firefox（優先）、Chrome、Chromiumのいずれかを開きます。システムやブラウザーのDNS設定は変更しません。VM外のクライアントでは、それぞれの名前解決設定とネットワークアクセスが必要です。

<a id="2-create-the-environment-file"></a>

### 2. 環境ファイルを作成する

ローカルファイルが存在しない場合だけ作成し、編集します。

```sh
test -e srcs/.env || cp srcs/.env.example srcs/.env
vim srcs/.env
```

現在の`srcs/.env.example`では、次のWordPressのユーザー名・メールアドレスの4項目が空欄です。

```dotenv
WORDPRESS_ADMIN_USER=
WORDPRESS_ADMIN_EMAIL=
WORDPRESS_USER=
WORDPRESS_USER_EMAIL=
```

コピーしたら、Makeの実行**前**に`srcs/.env`の4項目すべてを入力してください。ローカルファイルに空欄があるとWordPressの起動は失敗します。その他の必須設定も確認します。

ローカル開発や動作テストでは、以下のような有効なサンプル値を使えます。

```dotenv
WORDPRESS_ADMIN_USER=taka_owner
WORDPRESS_ADMIN_EMAIL=owner@example.com
WORDPRESS_USER=taka_editor
WORDPRESS_USER_EMAIL=editor@example.com
```

アカウントの作成やパスワードによるログインにメールの受信機能は必要ありません。entrypointは[`wp core install --skip-email`](https://developer.wordpress.org/cli/commands/core/install/)を使用し、一般ユーザーを作成するときも[`--send-email`](https://developer.wordpress.org/cli/commands/user/create/)を指定しません。メール通知やパスワード復旧が必要なら、受信可能なアドレスを使用してください。このVMでは、指定のログインドメインとループバックアドレスが正しい場合もあります。ファイルをコピーするだけではsecretの生成、DNS設定、Dockerストレージの準備は行われません。

`srcs/docker-compose.yml`およびentrypointスクリプトが必要とする、機密以外の値をすべて入力します。一般に以下が必要です。

- ドメイン名
- データベース名
- データベースユーザー名
- WordPress管理者のユーザー名とメールアドレス
- WordPress一般ユーザーのユーザー名とメールアドレス
- FTPユーザー名と、FTPクライアントから到達できるパッシブモード用アドレス
- 任意のバックアップスケジュールと保持数（`BACKUP_SCHEDULE`と`BACKUP_RETENTION`）

WordPressのユーザー名はASCII英字、数字、`.`、`_`、`-`のみを使用でき、60文字以内でなければなりません。2つのユーザー名は大文字小文字を無視しても異なる値にしてください。管理者ユーザー名には、大文字小文字に関係なく`admin`や`administrator`を含められません。メールアドレスは両方とも有効で、[互いに異なる](https://developer.wordpress.org/reference/functions/wp_insert_user/)必要があります。パスワードを`.env`に記載しないでください。

メールアドレスには`owner@example.com`のようにローカル部、`@`、ドメインを持つ形式を使います。WordPressはアカウント作成時に追加の[メールアドレス形式検証](https://developer.wordpress.org/reference/functions/is_email/)を行います。プロジェクトのentrypointは空欄、`@`の欠落、空白文字を拒否しますが、`make check-config`はWordPressアカウントの項目を検査しません。アカウント作成に失敗したら`make logs`を調べ、`Ctrl-C`でログ追跡を停止してください。

起動時、entrypointは設定されたログイン名がまだ存在しない場合にアカウントを作成します。`.env`を編集しても、既存アカウントの名前やメールアドレスは変わりません。ユーザー名を新しいログイン名に変更すると、古いアカウントは残したまま新しいアカウントが作成されます。既存ユーザーは別途更新してください。

<a id="3-create-local-secrets"></a>

### 3. ローカルsecretを作成する

Compose最上位の`secrets`セクションで要求するパスワードファイルを生成します。

```sh
./srcs/tools/generate-secrets.sh
```

このスクリプトはsecretディレクトリをモード`700`に設定し、以下のファイルをランダムな値とモード`600`で作成します。既存のファイルは変更しません。

```text
secrets/db_root_password.txt
secrets/db_password.txt
secrets/wp_admin_password.txt
secrets/wp_user_password.txt
secrets/redis_password.txt
secrets/ftp_password.txt
```

`.gitignore`は`srcs/.env`とsecretの内容を除外します。コミット前にsecretがGitで追跡されていないことを確認してください。

```sh
git ls-files srcs/.env secrets
```

想定される出力は空ですが、`secrets/.gitkeep`だけは表示されることがあります。

<a id="4-prepare-persistent-storage"></a>

### 4. 永続ストレージを準備する

VMのDockerデーモンはdata-rootとして`/home/takawaka/data/docker`を使用する必要があります。通常のlocalドライバーによる名前付きボリュームは次に保存されます。

```text
/home/takawaka/data/docker/volumes/inception_mariadb_data/_data
/home/takawaka/data/docker/volumes/inception_wordpress_data/_data
/home/takawaka/data/docker/volumes/inception_backup_data/_data
```

保持するDockerデータが存在しない新しいVMの場合は、rootとして`srcs/docker/daemon.json`の内容を`/etc/docker/daemon.json`に反映し、ほかの設定は保持します。`dockerd --validate --config-file=/etc/docker/daemon.json`で検証し、Dockerを再起動して`docker info --format '{{.DockerRootDir}}'`で確認してください。データがすでに存在する環境では、後述の移行手順なしに変更してはいけません。data-rootを変えただけでは既存データは移動しません。Dockerのcontainerdイメージストアは`/var/lib/containerd`に残っていても構いません。指定パス以下に置く必要があるのはボリュームの保存先です。

読み取り専用の事前チェックを実行します。

```sh
make check-config
```

`make check-config`は環境ファイル、空でない所有者専用のsecretファイル、Composeモデル、Dockerへのアクセス、data-root、既存ボリュームのオプションを確認します。ファイルの作成や変更は行いません。アプリケーションの起動時にComposeがボリュームを作成します。永続アプリケーションデータには`driver_opts`もbindマウントも使用しません。

`make prepare`は最初に完全クリーンアップ後の空のシステムDockerデーモンで復旧が必要か調べ、その後`make check-config`相当の検査を行います。データ削除や認証情報の生成は行いません。後述の初期化手順を参照してください。

プロジェクト名はclone先ディレクトリとは無関係に`inception`です。`LOGIN`の初期値はrootとして実行しても`takawaka`です。`DATA_DIR`は期待するdata-root（`DATA_DIR/docker`）を変更しますが、デーモン設定は変更しません。テスト用に`COMPOSE_PROJECT_NAME`を明示的に上書きすると、別のコンテナとボリュームを使用します。その場合、ホスト側のポートも本番アプリケーションから分離する必要があります。

<a id="build-and-launch"></a>

## ビルドと起動

ビルド前に、解決済みのCompose設定を検証します。

```sh
docker compose -f srcs/docker-compose.yml --env-file srcs/.env config --quiet
```

スタック全体をビルドして起動します。

```sh
make
```

`make`は`make up`と同等です。

```sh
make up
```

イメージだけをビルドしたい場合は`make build`を使ってください。VMのディスクやメモリの負荷を抑えるため、稼働対象のサービスを1つずつビルドします。`make up`はまずこのビルドを実行し、その後`up -d --no-build`でスタックをデタッチモードで起動します。再実行時は変更のないDockerビルドレイヤーと既存の名前付きボリュームが再利用されます。インストール済みのサイトは初期化されません。

<a id="manage-containers-and-logs"></a>

## コンテナとログの管理

| コマンド | 用途 |
| --- | --- |
| `make` / `make up` | 設定確認、サービスのビルド、スタックの起動。 |
| `make build` | 設定確認と稼働対象サービスのイメージビルド。コンテナは起動しません。 |
| `make check-config` | ローカルファイル、Compose、Dockerストレージを変更せずに検証。 |
| `make test` | 分離された復旧、クリーンアップ、管理ヘルパー、サービスの回帰テスト。 |
| `make ps` | サービスの状態と公開ポートを表示。 |
| `make logs` | 全サービスのログを追跡。`Ctrl-C`で追跡を停止してもコンテナは稼働し続けます。 |
| `make browser` | VMのグラフィカルセッションからデフォルトのHTTPSサイトを開く。 |
| `make stop` | コンテナを削除せず停止。 |
| `make start` | 停止中の既存コンテナを起動。 |
| `make restart` | イメージを再ビルドせずコンテナを再起動。 |
| `make backup` | 完了済みバックアップ世代を直ちに1つ作成。 |
| `make restore-check` | 新しいバックアップを作り、分離された一時ストレージに復元して検証。 |
| `make down` | コンテナとプロジェクトネットワークを削除。 |
| `make clean` | 永続データを保持して、コンテナと孤立コンテナを削除。 |
| `make fclean` | インストール済みヘルパーを利用し、`takawaka`としてDB、WordPress、バックアップを含むすべてのDockerリソースと`/home/takawaka/data/*`を削除。 |

`make restore-check`は1つの固有な世代名を`backup.sh now [generation]`と`restore-check.sh [generation]`に渡します。ロック競合を含むバックアップ作成エラーが発生すると中止し、指定した世代を正確に検証します。世代名なしで復元スクリプトを直接実行した場合は、随時確認用に最新の完了済みバックアップが選ばれます。

Composeを直接実行する場合は、Makefileと同じComposeファイルと環境ファイルを指定してください。例：

```sh
docker compose -f srcs/docker-compose.yml --env-file srcs/.env ps
docker compose -f srcs/docker-compose.yml --env-file srcs/.env logs --tail=100
```

<a id="service-implementation-rules"></a>

## サービス実装のルール

- 各サービスに独自のDockerfileを1つ使用します。
- ベースにはDebianまたはAlpineの、最新の安定版より1つ前の安定版を使用します。
- ベースイメージのタグは明示的に固定し、`latest`は使用しません。
- Dockerfile、ビルド引数、Composeファイル、Gitにパスワードを保存しません。
- 本来のサービスプロセスをPID 1のフォアグラウンドプロセスとして実行します。
- `tail -f`、対話シェル、`sleep infinity`、無限ループでコンテナを動かし続けてはいけません。
- 必須サービスすべてに再起動ポリシーを設定します。
- 必須スタックが公開するのはNGINXの443番ポートだけです。追加サービスが公開できるのは文書で明示したFTPの21番と21100～21110番、静的サイトの8080番、Adminerの8081番のみです。
- Backupサービスはポートを公開せず、cronをフォアグラウンドで実行します。Restore-checkは`ops`プロファイルの単発サービスであり、デフォルトのスタックでは起動しません。
- Restore-checkは内部の`restore`ネットワーク（`inception_restore`）上で専用の`restore-db`サービスを使用します。両サービスは`ops`プロファイルに所属し、検証後に一時DBコンテナは削除されます。
- NGINXではTLS 1.2とTLS 1.3だけを有効にします。
- host networking、`links`、`--link`は使用しません。
- WordPressの管理者と、別の一般ユーザーを作成します。

<a id="networking"></a>

## ネットワーク

すべての必須サービスは、明示的に定義したユーザー定義Dockerネットワークへ接続する必要があります。内部通信にはComposeのサービス名を使います。

- NGINXはWordPress/PHP-FPMサービスへ接続します。
- WordPressはMariaDBへ接続します。
- WordPressはオブジェクトキャッシュ用のRedisへ接続します。
- FTPはWordPressボリュームを共有します。静的サイトとAdminerはネットワークをサービス境界として使用します。
- BackupはWordPressボリュームを読み取り専用で参照し、DBのsecretを使ってMariaDBに接続します。Restore-checkはバックアップボリュームのみを使用し、内部のプライベートネットワーク経由で一時MariaDBサービスへ接続します。このサービスには公開ホストポートも本番データのボリュームもありません。
- MariaDBはホストに公開しません。

コンテナの再作成でIPアドレスは変わる場合があるため、コンテナIPをハードコードしないでください。

NGINXは、依存サービスを含むCompose操作で`restart: true`を指定して、正常なWordPressに依存します。ただし、`up wordpress`だけを実行した場合にはNGINXが稼働し続ける可能性があります。このため、NGINXは変更可能なFastCGIアップストリームをDocker DNS（`127.0.0.11`、キャッシュは5秒）からも名前解決します。これにより、再作成されたWordPressのIPが変わってもプロキシを再起動せずに対応できます。この設定は、クラッシュ後の再起動ポリシーとは別です。

WordPressが`healthy`になるのは、PHP-FPMが9000番ポートで接続を受け付け、データベースのインストールが完了してからです。PHP-FPMの準備完了まで待つことで、依存サービスが起動する前にentrypointによる2ユーザーの作成とRedisの有効化も完了します。AdminerとFTPは既定で`ADMIN_BIND_ADDRESS=127.0.0.1`にバインドされます。FTPパッシブポートを含むSSH転送はUSER_DOCを参照してください。静的サイトはホストの8080番ポートで公開され、認証情報を入力するフォームはありません。

<a id="data-persistence"></a>

## データ永続性

永続的に保存するデータは次のとおりです。

| データ | ホストの保存先 | 利用サービス |
| --- | --- | --- |
| MariaDBデータベース | `/home/takawaka/data/docker/volumes/inception_mariadb_data/_data` | MariaDB |
| WordPressファイル | `/home/takawaka/data/docker/volumes/inception_wordpress_data/_data` | WordPress、NGINX、FTP、Backup |
| バックアップ世代 | `/home/takawaka/data/docker/volumes/inception_backup_data/_data` | Backup、Restore-check |

コンテナの書き込み可能レイヤーは破棄される可能性があります。永続的な状態は名前付きボリュームに保存する必要があります。`make down`、`make clean`、コンテナ再作成でこのデータが削除されてはいけません。

`wordpress_data`のすべてのマウントでは`volume.nocopy: true`を使用します。WordPressのentrypointは、空のボリュームを`/usr/src/wordpress`から初期化します。これにより、WordPressの起動前にComposeがNGINXコンテナを作成した際、NGINXに同梱されたデフォルトHTMLが共有ボリュームへコピーされることを防ぎます。

次のコマンドでボリュームを確認します。

```sh
docker volume ls
docker volume inspect inception_mariadb_data inception_wordpress_data
```

意図的にデータを削除し、検証済みのバックアップがある場合を除き、`docker compose down --volumes`、`docker volume rm`、`docker volume prune`は実行しないでください。

<a id="migrate-the-previous-srcs-project-without-discarding-data"></a>

### データを破棄せず、以前の`srcs`プロジェクトを移行する

VM管理者としてメンテナンス時間帯に作業してください。検証済みバックアップ、以前のソースツリーとComposeファイル、イメージID、デーモン設定を`/home/takawaka/data`の外に保持します。コピー前に空き容量を確認してください。

1. 元のComposeファイルとプロジェクト名`srcs`で古いアプリケーションを停止します。ロールバックのため、コンテナと古い3つのボリュームは保持します。
2. 停止済みDB、WordPressファイル、バックアップをアーカイブ化し、数値UID/GID、権限モード、ACL、拡張属性を維持します。アーカイブのチェックサムを確認してください。
3. `docker.service`と`docker.socket`を停止し、全コンテナの停止と古いボリュームのマウント解除を確認します。提供されたVMでは`/var/lib/docker`と`/home/takawaka/data`が同じファイルシステム上にあります。大量のイメージやキャッシュをコピーする代わりに、Dockerディレクトリを`/home/takawaka/data/docker`へ移動します。
4. 新しいdata-root設定を反映・検証してDockerを起動します。containerdのストレージは変更せず、古い`srcs`コンテナは停止したままにします。
5. `inception_mariadb_data`、`inception_wordpress_data`、`inception_backup_data`という通常のlocalボリュームを作成し、`com.docker.compose.project=inception`と対応する`com.docker.compose.volume`キーをラベルに設定します。空であることを確認し、停止済みの対応データを確認済みマウントポイントへコピーして、所有権とACLを維持します。
6. `make check-config`を実行し、新しいプロジェクトをビルド・起動します。DBユーザー、投稿、コメント、ファイルハッシュ、権限、バックアップのチェックサムを事前の記録と比較します。TLS、再起動、復元も確認してください。

この移行処理の途中では古いディレクトリやボリュームを削除しないでください。新しい環境がユーザーの書き込みを受け付ける前なら、ロールバックは次の順で行います。新プロジェクトの停止、古いComposeソースの復元、Docker停止、data-rootを元の場所へ移動、デーモン設定の復元、Docker起動、古い`srcs`プロジェクトの起動。新しいサイトですでに書き込みが行われた場合は、まず変更内容を保全してください。その時点では元のボリュームは最新ではありません。

<a id="complete-cleanup-and-storage-recovery"></a>

### 完全クリーンアップとストレージ復旧

完全クリーンアップでは、Dockerを起動したまますべてのDockerリソースを削除し、`/home/takawaka/data/*`を削除します。これによりDocker自身のメタデータも失われます。Makeは設定検査やComposeビルドの前に`srcs/tools/prepare-storage.sh`を呼び出します。data-rootが存在しない場合、スクリプトはローカルシステムのデーモンとコンテナが一切残っていないことを確認し、インストール済みヘルパーを使って`docker.service`を一度再起動します。Dockerが保存先を再作成するため、Composeは新しい環境をビルドできます。

通常のビルドではデーモンを再起動しません。接続先がリモート、コンテナが残存、設定パスが不一致、復旧失敗のいずれかの場合、Makeはビルド前に中止します。Makeが自動的にクリーンアップコマンドを実行することはありません。削除済みのアプリケーションデータは自動復元されません。必要なバックアップと移行アーカイブは削除対象外に保持し、`.env`とsecretはリポジトリのGit無視対象のローカルファイルに保存してください。イメージが削除されたか確認する際は、別に存在するcontainerdストアにも注意が必要です。

全削除のテストには破棄可能なVMを使用してください。正確な手順は[STORAGE_RECOVERY.md](srcs/docker/STORAGE_RECOVERY.md)にあります。`make test`はシステムコマンドのモックを使い、復旧判定、全削除の安全策、Makeの実行順序を検証します。Dockerや管理者権限は不要です。サービスの回帰テストでは、認証情報の失敗、FTPのUID制限、バックアップの失敗、指定された復元世代の選択も検査します。利用可能な場合、`my_print_defaults`は実際のMariaDBオプションファイルパーサーを介してパスワードの往復処理を検証し、`redis-server`はすべてのリスナーを無効にして生成済み設定を検証します。ツールがない場合、このパーサー検査は明示的にスキップされます。

<a id="source-inspection-and-functional-verification"></a>

## ソースの検査と機能検証

これらの手順は、[USER_DOCのサービス検証手順](USER_DOC_ja.md#service-verification-procedures)および[操作コマンドのチェックリスト](USER_DOC_ja.md#operation-command-checklist)と併用してください。ソースの出所、設定、安全なストレージ削除、コンテナの動作、サービス設定の変更、診断を扱います。既知のソースリビジョンを使用し、実行前にシェルヘルパーを確認します。検査が失敗したら、依存サービスへ進む前に原因を調べてください。

<a id="1-clone-and-identify-the-source-repository"></a>

### 1. ソースリポジトリをcloneして識別する

再現可能なソースリビジョンを調べるには、別の作業コピーではなく、現在の42 IntraプロジェクトページにあるSSHリポジトリURLを使用してください。作業用PCで次の接続を行うと、すでにロック解除済みのSSHエージェントをVMへ転送できます。

```sh
ssh-add -l
ssh -A -p 2222 takawaka@127.0.0.1
```

そのSSHセッション内で、通常のVMユーザーとしてBashで実行します。

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

cloneする前に引用符内のリポジトリURLを置き換えてください。新しい親ディレクトリは空で、clone先はまだ存在しない必要があります。リポジトリ所有者、プロジェクト、意図したコミット、追跡対象の作業ツリーがクリーンであることを確認します。コマンドの配置場所とエイリアスの確認は、想定外のラッパーやツールの置き換えを発見するためのものです。疑わしい場合は中身を調べてください。リポジトリが空なら、正しいリモートURLを使用したか確かめます。

エージェント転送が機能するのは、作業用PCのエージェントがアカウント登録済みの鍵で署名できる間の`ssh -A`セッション内のみです。別に起動したVMデスクトップの端末には、転送されたエージェントは引き継がれません。VMデスクトップから直接cloneするには、42 Intraに登録した公開鍵に対応する秘密鍵をSSHで使用できるようにしてください。`ssh -i`で指定するものを含むローカル鍵ファイルや、VM自身のSSHエージェントでロック解除済みの鍵を使用できます。`ssh-add -l`はエージェント内の鍵だけを調べるため、エージェントが空または使用不能でもローカル鍵ファイルによる認証は可能です。標準パス以外の鍵では、`~/.ssh/config`にリポジトリホストの`IdentityFile`を設定します。cloneコマンドは上記と同じです。`Enter passphrase for key`という表示はSSH秘密鍵のパスフレーズを要求しており、IntraのWebパスワードでは解除できません。通常ユーザーとしてcloneしてください。ブラウザー確認にはVMのグラフィカル端末を使用します。

<a id="2-prepare-the-shell-and-inspect-the-project-files"></a>

### 2. シェルを準備し、プロジェクトファイルを確認する

上記の環境ファイル、secret、名前解決、ストレージの設定を完了してください。既存の環境ファイルは上書きしないでください。機密以外の設定内容を確認し、ローカルのsecretを用意します。有効なサンプルアカウントの値はそのまま使用できます。cloneしたリポジトリから`takawaka`としてコマンドを実行してください。インストール済みストレージヘルパーが、必要に応じてクリーンアップと復旧の権限を提供します。

```sh
unset COMPOSE_PROJECT_NAME
dc() {
  command docker compose -f srcs/docker-compose.yml --env-file srcs/.env "$@"
}
wp() {
  dc exec -T wordpress wp --allow-root --path=/var/www/html "$@"
}
pwd
id -u
docker context show
docker info --format '{{.DockerRootDir}}'
dc config --quiet
```

`takawaka`アカウント、意図したソースディレクトリ、VM内のローカルDockerデーモン、data-rootが`/home/takawaka/data/docker`であることを確認します。`docker context inspect`で使用中の接続先を調べ、`DOCKER_HOST`と`DOCKER_CONTEXT`が設定されていればその値も確認してください。作業用PCやリモートデーモンに対して破壊的なクリーンアップを絶対に実行しないでください。提供されたVMでは`~/.bashrc`からヘルパー関数が読み込まれます。`type dc wp`で関数定義を確認できます。すでに開いているBashでは`source ~/.bashrc`で再読み込みしてください。新しいVMでは、再起動後も含め必要に応じて新しいシェルごとに関数を定義します。

必要なプロジェクトファイル、READMEの構造、誤ってGitで追跡されている可能性がある認証情報を確認します。

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

READMEの1行目には、斜体で記述したカリキュラム指定文とログイン名が必要です。ドキュメントファイルは空であってはならず、Resources節にはAIの利用について記載してください。認証情報ファイル一覧に表示されてよいのは`secrets/.gitkeep`だけです。最後のコマンドが出力するのはファイル名であり、secretの値ではありません。secretファイル名や環境変数への正当な参照でも検索に一致する場合があります。必要に応じて候補ファイルとGit履歴を確認してください。これらの検索だけでは、過去に認証情報がコミットされなかったことの証明にはなりません。実際の認証情報が追跡されている場合はリポジトリから除外し、認証情報を変更してください。ローカルのsecretファイル、`wp-config.php`、Redisの生成済みパスワード設定、SQLダンプを端末ログに表示してはいけません。

ビルド定義と実行時スクリプトを調べます。

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

`less`は`q`で終了、`:n`で次のファイルへ移動します。`find`は空のDockerfileを一切表示してはいけません。禁止パターンへの一致がないことを確認してください（`git grep`の終了コード`1`は一致なしを表します）。Composeファイル全体、すべてのDockerfile、すべてのentrypointスクリプトを読んでください。パターン検索ですべてのバックグラウンドプロセスや無限ループを検出できるわけではありません。回数が制限された依存サービスへの再試行や、有限な入力に対するループは維持目的の無限ループではありません。シェルの`&&`によるコマンド連結はバックグラウンド実行ではありません。

必要時のみ使用する2つの復元サービスを含め、空でないDockerfileが10個存在します。それぞれが`FROM debian:12`で始まり、WordPressとMariaDBにはNGINXをインストールしません。ビルド検証時には、[Debianのリリース一覧](https://www.debian.org/releases/)で1つ前の安定版であることを確認してください。2026年10月10日時点ではDebian 13がstable、Debian 12がoldstableです。各サービスにはビルドコンテキストがあり、イメージ名は`<service>:inception`です。完成済みのサービスイメージは使用していません。`make`のドライランでは、ストレージと設定の検査に続き、稼働対象サービスを1つずつビルドするループ、その後のComposeの`up -d --no-build`が表示される必要があります。`make test`は分離されたストレージ、クリーンアップ、サービスの検査を実行し、本番デーモンのビルドや操作は行いません。

<a id="3-perform-a-complete-cleanup-and-fresh-startup"></a>

### 3. 完全クリーンアップと新規起動を実行する

**破壊的操作：以下の手順では、このVMのすべてのDockerコンテナ、イメージ、ボリューム、削除可能なネットワークを削除した後、`/home/takawaka/data/*`以下のすべてのデータを削除します。バックアップ用ボリュームも含まれます。** 所有者の許可を得た専用の破棄可能なVMでのみ実行してください。ソースリポジトリ、環境ファイル、secret、必要なアーカイブは削除対象外に配置します。保持する必要があるデータを削除する前に、検証済みのVMスナップショットまたは外部バックアップを保存してください。

現在のスタックに保持すべきデータがある場合、完了したバックアップを退避する方法の一例を以下に示します。稼働中のスタックで、デフォルトのプロジェクト名を使用している場合にのみ実行してください。

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

次に進む前に各コマンドが成功したことを確認します。アーカイブにはDBの内容と`wp-config.php`が含まれるため、退避先ディレクトリへのアクセスを制限してください。クリーンアップを取りやめる場合は`dc start backup`でスケジューラーを再開します。`inception_backup_data`にしか存在しないバックアップは、完全クリーンアップ後には残りません。

cloneしたリポジトリ内の`takawaka`シェルから、完全クリーンアップを実行します。

```sh
make fclean
docker ps -aq
docker images -qa
docker volume ls -q
docker network ls
```

`fclean`は`srcs/tools/fclean.sh`を実行します。削除前に、管理者ヘルパーの権限、正規化された`/home/<login>/data`パス、ソースと設定が削除対象の外にあること、ローカルシステムのDockerソケット、対応するdata-rootを検査します。全コンテナを停止・削除し、全イメージと全ボリューム、すべてのカスタムネットワークを削除します。空の一覧はスキップします。[`type=custom`フィルター](https://docs.docker.com/reference/cli/docker/network/ls/#type)により組み込みの`bridge`、`host`、`none`ネットワークは保持されます。リソースや入れ子のマウントが残っていないことを確かめた後、`DATA_DIR`以下の隠しファイル以外の項目を削除します（`data/*`の削除に相当します）。失敗すると手順が停止するため、原因を解決してから続行してください。インストール済みヘルパーは限定的な管理者権限を提供し、データ削除前に固定パス、Dockerが空であること、残存マウントを再確認します。sudoポリシーでパスワード不要なのは固定された3操作だけです。

その後、同じ`takawaka`シェルから新しい環境を起動します。

```sh
make
make check-config
make ps
docker info --format '{{.DockerRootDir}}'
```

クリーンアップ中もDockerを稼働させておきます。data-rootが削除されていた場合、`make`は[STORAGE_RECOVERY.md](srcs/docker/STORAGE_RECOVERY.md)に記述された安全条件付きのデーモン再起動を実行し、新規環境をビルド・起動します。削除済みの内容やバックアップは復元できません。この破壊的初期化は必要な場合だけに実行し、**永続性テスト中には実行しないでください**。続いて[USER_DOCのサービス起動手順](USER_DOC_ja.md#1-start-the-stack-and-define-checking-commands)へ進みます。

<a id="4-inspect-the-running-infrastructure"></a>

### 4. 稼働中のインフラストラクチャを確認する

起動後、手順2の補助関数を使います。イメージ名、再起動ポリシー、PID 1で実行されるフォアグラウンドプロセスを確認します。

```sh
dc ps --all
docker inspect $(dc ps -q) --format \
  '{{.Name}} image={{.Config.Image}} restart={{.HostConfig.RestartPolicy.Name}} state={{.State.Status}}{{if .State.Health}} health={{.State.Health.Status}}{{end}}'
for review_service in mariadb wordpress nginx redis ftp static-site adminer backup; do
  printf '%s PID 1: ' "$review_service"
  dc exec -T "$review_service" sh -c 'tr "\000" " " < /proc/1/cmdline; printf "\n"'
done
```

8つの常時稼働サービスすべてについて、イメージ名がサービス名に対応し、再起動ポリシーが`unless-stopped`であることを確認します。PID 1はMariaDB、PHP-FPM、NGINX、Redis、vsftpd、Python HTTPサーバー、PHP HTTPサーバー、フォアグラウンドcronなどの本来のサービスプロセスでなければなりません。コンテナを維持するだけのシェルや`tail`であってはいけません。必要時に動くopsサービスは、通常の稼働一覧には表示されません。

明示的なbridgeネットワーク、サービス名DNS、外部公開されない内部ポート、実際のデータマウントを確認します。

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

`inception_inception`のドライバーが`bridge`で、Docker DNSによってサービス名が名前解決され、MariaDB、WordPress、Redisにホストのポートバインドがないことを確認します。Dockerfileの`EXPOSE`はホストポートを公開するものではありません。MariaDBは`inception_mariadb_data`を`/var/lib/mysql`へマウントします。WordPress、NGINX、FTP、Backupは`inception_wordpress_data`を共有し、NGINXとBackupは読み取り専用でマウントします。Backupは`inception_backup_data`もマウントします。分離された復元ネットワークは、opsチェックの実行時だけ作成されます。

<a id="5-change-a-service-configuration-and-restore-it"></a>

### 5. サービス設定を変更し、元に戻す

一時的な設定変更テストでは、サービスと使用可能なポートを選びます。以下の例では、NGINXコンテナ自身は443番ポートで待ち受けたまま、**ホスト側**のHTTPSポートを443から8443へ変更します。別のポートを選ぶ場合は、以降の指定も統一してください。完了後は必須の443番ポートに戻します。ポートを選ぶ前に、公開済みのポートと待受ソケットを調べます。

```sh
ss -ltn '( sport = :8443 )'
docker ps --format 'table {{.Names}}\t{{.Ports}}'
review_port_backup=$(mktemp -d /home/takawaka/inception-port-change.XXXXXX)
cp -p srcs/docker-compose.yml "$review_port_backup/docker-compose.yml"
cp -p srcs/requirements/nginx/conf/nginx.conf "$review_port_backup/nginx.conf"
wp option get home > "$review_port_backup/home.txt"
wp option get siteurl > "$review_port_backup/siteurl.txt"
review_domain=$(dc exec -T wordpress printenv DOMAIN_NAME)
: "${review_domain:?Could not read DOMAIN_NAME from WordPress}"
vim srcs/docker-compose.yml
```

NGINXのポート対応を`"443:443"`から`"8443:443"`に変更します。`DOMAIN_NAME`にポートを含めてはいけません。entrypointはホスト名だけを要求します。さらに`srcs/requirements/nginx/conf/nginx.conf`を編集し、PHPのlocation内の`fastcgi_param HTTPS on;`の直後に次の行を追加します。

```nginx
fastcgi_param HTTP_HOST $host:8443;
```

同梱の`fastcgi_params`はポートなしのホスト名をPHPへ渡す場合があります。その場合、WordPressは8443を含むURLへ何度もリダイレクトします。明示したパラメーターによって、選択した公開ポートを含む正規化済みのホスト名を渡します。既存のWordPressのURL設定値はMariaDBに保存され、コンテナの再作成だけでは書き換わらないため、明示的に更新します。

```sh
git diff -- srcs/docker-compose.yml srcs/requirements/nginx/conf/nginx.conf
dc config --quiet
wp option update home "https://$review_domain:8443"
wp option update siteurl "https://$review_domain:8443"
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

8443番ポート、HTTPSによるHTTPステータス`200`、TLSハンドシェイクの成功を確認します。VMのブラウザーで`https://takawaka.42.fr:8443/`と`/wp-admin/`を手動で開いてください。`make browser`は元のデフォルトポートのURLを開きます。ログインと保存済みページ・コメントの内容も確認します。`make up`はイメージを再ビルドし、設定が変わったコンテナを再作成します。`make restart`だけでは新しいポートマッピングを適用できず、イメージも再ビルドされません。

Compose設定、NGINX設定、WordPressのURLオプションを記録済みの値へ戻します。

```sh
cp -p "$review_port_backup/docker-compose.yml" srcs/docker-compose.yml
cp -p "$review_port_backup/nginx.conf" srcs/requirements/nginx/conf/nginx.conf
wp option update home "$(cat "$review_port_backup/home.txt")"
wp option update siteurl "$(cat "$review_port_backup/siteurl.txt")"
make up
make ps
dc port nginx 443
curl --noproxy '*' --insecure --fail --silent --show-error \
  --resolve "$review_domain:443:127.0.0.1" \
  --output /dev/null --write-out 'HTTPS status: %{http_code}\n' \
  "https://$review_domain/"
git diff -- srcs/docker-compose.yml srcs/requirements/nginx/conf/nginx.conf
```

元のポート対応と正常なWordPress動作が復旧し、ポート変更による差分が残っていないことを確認します。変更後の設定が失敗した場合は、原因調査前に記録済みの値を使ってサービスを元に戻してください。ボリュームの削除や`git reset --hard`でロールバックしてはいけません。

<a id="alternative-service-configuration-changes"></a>

#### その他のサービス設定変更

NGINXの例と同じように、編集するソースや環境ファイルはすべて`/home/takawaka/data`の外へ保存してください。変更するポートを追跡対象ソースから検索し、待受側、接続するクライアント、TCPによるhealthcheckをまとめて更新します。

```sh
git grep -n -E '443|9000|3306|6379|8080|21100|21110' \
  -- srcs/docker-compose.yml srcs/requirements
```

次の表のパスは、特記がなければ`srcs/requirements/`からの相対パスです。内部専用サービスは外部公開しないでください。

| 対象サービス | 整合させるべき設定箇所 |
| --- | --- |
| NGINXコンテナの待受ポート | `nginx/conf/nginx.conf`の`listen`、Compose内の接続先ポート、変更する場合はホスト側ポート。8443で待ち受ける場合は`"8443:8443"`を使用し、公開ポートを含む`HTTP_HOST`を渡して前述のようにWordPressのURL設定を更新します。`dc port nginx 8443`で確認します。 |
| WordPress/PHP-FPM | `wordpress/conf/www.conf`の`listen`、`nginx/conf/nginx.conf`の`wordpress_backend`、ComposeのWordPress用`fsockopen` healthcheck。9000番ポートを使うすべての設定を揃えます。ブラウザーのHTTPSポートは引き続き443です。 |
| MariaDB | `mariadb/conf/mariadb.cnf`の`port`、`wordpress/tools/entrypoint.sh`の`DB_PORT`、`bonus/backup/tools/backup.sh`のクライアントポート、既存のWordPressの`DB_HOST`定数。Adminerのサーバー指定は`mariadb:NEW_PORT`に変更し、TCP接続のSQLログインコマンドに`--port=NEW_PORT`を追加します。MariaDBのhealthcheckはUnixソケットを使用します。 |
| Redis | `bonus/redis/conf/redis.conf`の`port`、`wordpress/tools/entrypoint.sh`の`REDIS_PORT`とPHP準備確認処理内のリテラル値`6379`、既存のWordPressの`WP_REDIS_PORT`定数。RedisのhealthcheckはUnixソケットを使用します。 |
| 静的サイト / Adminer | ホスト側ポートを変更する場合はComposeの左側のポートを編集し、新しいブラウザー/curlのURLを使います。コンテナ側ポートも変更する場合は、`bonus/static-site/Dockerfile`または`bonus/adminer/Dockerfile`内のサーバー`CMD`とComposeの接続先ポートも更新してください。Adminerのループバックへのバインドは維持します。 |
| FTP | 制御ポートの待受を変える場合は`bonus/ftp/conf/vsftpd.conf`に`listen_port=NEW_PORT`を追加し、Composeの制御ポート対応とクライアント・トンネル側ポートを更新します。パッシブポート範囲を変更する場合は、`pasv_min_port`、`pasv_max_port`、対応するすべての公開・転送ポートを更新します。ログインだけでなくファイル転送も検証してください。 |
| Backup | このサービスには待受ポートがありません。別の設定変更を試す場合は、`srcs/.env`の`BACKUP_SCHEDULE`を編集してMakeでコンテナを再作成し、`/etc/cron.d/inception-backup`を調べ、バックアップと復元のチェックを実行してください。 |

コンテナの待受ポートを変更した場合は、存在するならDockerfileの`EXPOSE`メタデータも更新します。ただし、`EXPOSE`だけでは待受ポートの変更もホストポートの公開も行われません。ダウンロードのチェックサムに含まれるポート番号に似た数字を置換しないでください。

既存の`wp-config.php`はentrypointによって保持されます。MariaDBを3307番ポートへ変更する場合、機密情報ではないホスト設定を記録し、古いWordPressコンテナがまだ実行中の間、再ビルドする**前**に更新します。

```sh
review_db_host_before=$(wp config get DB_HOST)
wp config set DB_HOST 'mariadb:3307' --type=constant
```

Redisを6380番ポートへ変更する場合は、次の別案を使用します。

```sh
review_redis_port_before=$(wp config get WP_REDIS_PORT)
wp config set WP_REDIS_PORT 6380 --type=constant --raw
```

選択したサービスのコマンドだけを実行し、ポート番号は選択した値に置き換えてください。作業シェルを変える場合は、元の設定値をデータディレクトリ外に記録しておきます。`dc config --quiet`、`make up`、`make ps`、USER_DOCの該当する検査を使って変更後のサービスとその依存先を検証します。MariaDBについては公開WordPressサイト、SQLログイン、Adminer、`make restore-check`を確認します。Redisについては、別プロセス間でのキャッシュテストを確認します。

最後の`make up`の前に、保存したソースファイルと対応する実行時の定数を復元します。次の単発コマンドは、WordPressサービスが`unhealthy`でも動作します。[`wp config set`](https://developer.wordpress.org/cli/commands/config/set/)はWordPressの読み込み前に設定を書き換え、`--no-deps`は依存サービスを起動しないためです。

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

その後、元のポートでサービス確認を再実行し、ソースの差分を保存済みの基準状態と比較します。ソースファイルを戻しただけでは、WordPressボリューム内に永続保存された定数は復元されません。

<a id="mariadb-and-data-persistence-commands"></a>

### MariaDBとデータ永続性の確認コマンド

[USER_DOCのデータベースと永続性に関する節](USER_DOC_ja.md#database-and-persistence-commands)には、簡易確認と詳細な検証手順の両方があります。

| 対象 | 簡易確認 | 詳細な検証 |
| --- | --- | --- |
| MariaDB | `make ps`、対話的なDBログイン、`SHOW TABLES;`、ユーザー数 | Dockerfileとボリュームのマウントポイントを調べ、現在のDB設定で接続し、投稿・コメントを問い合わせます。 |
| 永続性 | VMを再起動し、同じcloneで`make up ps browser`を実行してサイトとDBを確認 | 再起動前後のアカウント、ページ本文、コメント、ボリュームのマウントポイントを比較します。必要ならJSONスナップショットを保存し、`diff`で比較します。 |

短いDBログインコマンドは`.env.example`の初期値を使用します。

```sh
dc exec mariadb mariadb -h127.0.0.1 -u wp_user -p wordpress
```

DB名やユーザー名を変更している場合は、コンテナの現在の設定を使用します。

```sh
dc exec mariadb sh -c \
  'exec mariadb --protocol=TCP --host=127.0.0.1 --user="$MYSQL_USER" --password "$MYSQL_DATABASE"'
```

いずれのコマンドでも、プロンプトで`secrets/db_password.txt`のパスワードを入力します。`-h127.0.0.1`はMariaDBコンテナ内部のTCP接続先です。シェルコマンドはVM上で`takawaka`として実行し、データベースには`MYSQL_USER`として接続します。MariaDBの`root`とVMの`root`は別のアカウントです。再起動後もMakeは`takawaka`として実行してください。永続性テストにはデータを保持する通常の起動操作を使用し、破壊的な`make fclean`を実行しないでください。

<a id="verification-coverage-and-architecture-notes"></a>

### 検証範囲とアーキテクチャの補足

次の表では、一般的な運用確認を関連するコマンド・手順へ対応付けています。CLIの出力だけでなく、実際のブラウザー動作も確認してください。

| 対象 | 確認内容 |
| --- | --- |
| ソースと設定 | 上記の手順1～3：新規clone、コマンドとエイリアスの確認、認証情報と必須ファイルの検査、安全条件付きクリーンアップ。 |
| アーキテクチャ | 以下およびREADMEで説明するDocker/Compose、イメージとコンテナの違い、VMとの比較、リポジトリ構成。 |
| ドキュメント | READMEの構造、Description・Instructions・ResourcesとAIの利用、空ではないUSER_DOCとDEV_DOC。 |
| HTTPSとTLS | [USER_DOCのTLS検査](USER_DOC_ja.md#2-verify-https-and-tls)：443番ポート、利用不能な80番ポート、TLS 1.2/1.3、証明書、ブラウザーからのアクセス。 |
| イメージの構築 | 手順2のソース確認と手順4の実行時確認：独自Dockerfile、許可されたベースイメージ、サービスイメージ、Composeビルド、フォアグラウンドプロセス。 |
| Dockerネットワーク | 手順4：Composeのネットワーク宣言、bridge設定、サービス名DNS、内部専用のMariaDB/WordPress/Redis。 |
| WordPressとPHP-FPM | [USER_DOCのボリューム確認](USER_DOC_ja.md#3-inspect-persistent-volumes)と[アプリケーション確認](USER_DOC_ja.md#4-check-wordpress-and-database-access)：ボリュームパス、WordPressのインストール、異なるユーザー、ログイン、コメント、ページ変更。 |
| MariaDB | [USER_DOCのDB確認](USER_DOC_ja.md#4-check-wordpress-and-database-access)：ボリュームパス、認証付きSQLアクセス、テーブル、ユーザー。 |
| 永続性 | [USER_DOCの再起動検証](USER_DOC_ja.md#5-verify-persistence-across-container-recreation-and-vm-reboot)：VM再起動、Makeによる起動、保持された内容・ユーザー・DB・ボリュームパス。 |
| 設定変更 | 上記の手順5：一時的に空きポートへ変更し、必要ならWordPressのURL設定も修正して、再ビルド・検証・復元。 |
| 追加サービス | [USER_DOCの追加サービス確認](USER_DOC_ja.md#7-check-additional-services)：Redisキャッシュ、FTP転送、静的サイト、AdminerのDBアクセス、バックアップスケジュール、一時領域への復元。 |

以下の概念が、この設計の選択理由を説明します。

- **DockerとCompose：** Dockerfileはイメージをビルドし、コンテナはイメージを実行します。Composeはサービス、ネットワーク、ボリューム、依存関係、secretファイルのマウントを記述・調整します。`docker run`でもComposeでもイメージ形式は同じですが、Composeは複数サービスをまとめた設定を提供します。
- **DockerとVM：** 必須のVMはゲストカーネルを持ち、コンテナはそのカーネルを共有しながら各サービスのプロセスとファイルシステムを分離します。コンテナの再ビルドはサービスごとに別のVMを構築するより軽量です。
- **ディレクトリ構成とネットワーク：** ルートのMakefileが操作の入口となり、`srcs/`にサービスの設定とビルド用ソースを置き、Gitで無視するローカル`secrets/`に認証情報を保持します。ユーザー定義bridgeネットワークがサービス名DNSを提供し、host networkingや古いlinksは不要です。
- **プロセスと永続性：** entrypointは初期化を行ってから`exec`で本来のサービスをフォアグラウンド実行します。コンテナを再作成すると書き込み可能レイヤーは置き換わりますが、名前付きボリュームはDBとWordPressファイルを保持します。Dockerのdata-rootを移動することで、標準のボリュームデータを指定のログインユーザー用データディレクトリに置けます。
- **TLSと認証情報：** TLS 1.2/1.3はネゴシエーションされるプロトコルのバージョンであり、証明書のバージョンではありません。自己署名証明書はログイン用ドメインを識別します。パスワードはイメージレイヤーやコマンドライン引数ではなく、ローカルのsecretファイルに保存します。
- **自由選択の追加機能：** フォアグラウンドのcronが、世代保持ポリシーとチェックサム付きでDBとファイルのバックアップをアトミックに定期作成します。別の一時MariaDBとWordPressアーカイブの展開を使い、バックアップを実際に復元できることを検証します。

<a id="documentation-maintenance"></a>

## ドキュメントの保守

`README.md`、`USER_DOC.md`、`DEV_DOC.md`と各日本語版、`srcs/.env.example`、secretファイル名、Composeのサービス名の整合性を維持してください。コマンド、パス、ポート、認証情報、永続化の動作を変更した場合は、必ずドキュメントも更新してください。
