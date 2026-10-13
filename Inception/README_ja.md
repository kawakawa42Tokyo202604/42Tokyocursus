*このプロジェクトは42カリキュラムの一環として、\<takawaka\>が作成しました。*

# Inception

英語版：[README.md](README.md)

<a id="description"></a>

## 概要

Inceptionは、仮想マシン内でDocker Composeを使って小規模なWebインフラストラクチャを構築する、システム管理の課題です。イメージの作成、サービスの分離、TLSの設定、コンテナ間のネットワーク、秘密情報の扱い、データの永続化を学びます。

必須構成は次の3サービスです。それぞれ専用のコンテナで動作し、個別のDockerfileからビルドします。

- **NGINX**：必須構成の唯一の公開入口です。443番ポートでTLS 1.2またはTLS 1.3によるHTTPS接続を受け付け、PHPリクエストをWordPressへ転送します。
- **WordPress + PHP-FPM**：アプリケーションを実行します。NGINXは含みません。
- **MariaDB**：WordPressのデータを保存します。NGINXは含みません。

NGINX、WordPress、MariaDBは専用のDockerネットワークで通信します。MariaDBのデータ、WordPressのファイル、バックアップの各世代は名前付きボリュームに保存するため、コンテナを再作成しても利用できます。Dockerのdata-rootは既定で`/home/takawaka/data/docker`です。ボリュームはbind用のオプションを指定せずlocalドライバーを使用し、このdata-rootの`volumes/`以下に保存されます。

追加のボーナスサービスも含まれています。

- **Redis**：WordPressのオブジェクトキャッシュを提供します。Dockerネットワーク内からのみアクセスできます。
- **FTP**：21番ポートとパッシブモード用の21100～21110番ポートを使って、WordPressボリュームへアクセスできます。
- **静的サイト**：PHPを使わないページを8080番ポートで配信します。
- **Adminer**：MariaDBを操作するWebインターフェースを8081番ポートで提供します。
- **Backup**：MariaDBのdumpとWordPressのファイルを定期的に保存します。ホスト側にポートを公開しません。
- **Restore-check**：必要なときに`ops`プロファイルで実行し、Makeが作成したバックアップを一時ストレージへ復元して検証します。

<a id="architecture"></a>

### 構成

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

必須構成では、NGINXだけがホストの443番ポートを公開します。WordPress、MariaDB、Redisには、プロジェクトのネットワークに接続したコンテナからのみアクセスできます。追加サービスが公開するポートは、上記で明示したものだけです。

<a id="included-sources"></a>

### 含まれるソース

| パス | 用途 |
| --- | --- |
| `Makefile` | Composeプロジェクトのビルド、起動、停止、状態確認。 |
| `srcs/docker-compose.yml` | サービス、ネットワーク、ボリューム、secretの定義。 |
| `srcs/.env` | ローカルの機密以外の設定値。Gitでは除外します。 |
| `srcs/.env.example` | 機密以外の設定値のひな形。 |
| `srcs/docker/` | VMのDockerストレージ設定、管理用ヘルパー、復旧ガイド。 |
| `srcs/tools/` | secretの生成、起動前の検査、ブラウザーの起動。 |
| `srcs/requirements/nginx/` | NGINXのDockerfile、TLS設定、ツール。 |
| `srcs/requirements/wordpress/` | WordPress/PHP-FPMのDockerfile、設定、ツール。 |
| `srcs/requirements/mariadb/` | MariaDBのDockerfile、設定、ツール。 |
| `srcs/requirements/bonus/` | Redis、FTP、静的サイト、Adminer、Backup、Restore-check。 |
| `srcs/requirements/bonus/backup/` | 定期バックアップ用のイメージとスクリプト。 |
| `srcs/requirements/bonus/restore-check/` | バックアップのアーカイブ検査と復元検証を行うクライアント。 |
| `srcs/requirements/bonus/restore-db/` | Restore-checkだけが使用する一時的なMariaDBサービス。 |
| `secrets/` | ローカルの秘密ファイル。内容はGitで除外します。 |
| `README_ja.md` | プロジェクトの概要と操作方法を説明する日本語の補助資料。 |
| `USER_DOC.md` | 利用者・管理者向けの英語版ドキュメント。 |
| `USER_DOC_ja.md` | 利用者向けドキュメントの日本語版。 |
| `DEV_DOC.md` | 開発環境の準備、操作、永続化を説明する英語版ドキュメント。 |
| `DEV_DOC_ja.md` | 開発者向けドキュメントの日本語版。 |

<a id="docker-storage-files"></a>

### Dockerストレージ関連ファイル

`srcs/docker/`には、VMのDockerストレージ設定例と、ストレージ管理用のヘルパーがあります。

| ファイル | 用途 |
| --- | --- |
| [daemon.json](srcs/docker/daemon.json) | Dockerの`data-root`を`/home/takawaka/data/docker`に設定する例。 |
| [inception-storage-admin.sh](srcs/docker/inception-storage-admin.sh) | 保存先の検査、Dockerリソース削除後のデータ削除、ストレージ消失時のDocker再起動。 |
| [inception-storage.sudoers](srcs/docker/inception-storage.sudoers) | インストール済みヘルパーの`check`、`clean-data`、`recover`だけを、`takawaka`がsudoでパスワードなしに実行できるようにする設定。 |
| [STORAGE_RECOVERY.md](srcs/docker/STORAGE_RECOVERY.md) | `make fclean`後のストレージ復旧、その条件、検証手順。 |

デーモン設定とヘルパーは、最初に管理者として設定する必要があります。その後は`takawaka`がクリーンアップと復旧を含むMakeコマンドを実行できます。設定へのリンクと、Makeからヘルパーへの呼び出しは[DEV_DOCのストレージ関連ファイル](DEV_DOC_ja.md#docker-storage-files)を参照してください。ストレージ復旧はDockerの保存先ディレクトリを再作成する操作です。削除されたサイトのデータを戻すには、別途バックアップからの復元が必要です。

<a id="main-design-choices"></a>

### 主な設計方針

- 各サービスの役割を分け、専用のコンテナで動かします。
- サービスイメージはプロジェクトのDockerfileからローカルでビルドします。pullできるのは、許可されたDebianまたはAlpineのベースイメージだけです。
- イメージには明示的なバージョンタグを使用し、`latest`は使用しません。
- 必須のWordPressサイトの公開入口はNGINXだけで、443番ポートだけを使用します。
- 追加サービスは文書に記載した別のポートを公開し、WordPressやMariaDBを直接公開しません。
- ユーザー定義のDockerネットワークでサービス名によるDNSを提供し、内部通信をホストネットワークから分離します。
- パスワードはsecretファイルとして渡します。`.env`はドメイン名、データベース名、ユーザー名など、機密以外の値に使用します。
- Backupはcronをフォアグラウンドで動かします。完了した各世代はチェックサム付きで、所有者だけがアクセスできる権限のもとにアトミックに保存します。復元検証は稼働中のアプリケーションボリュームをマウントせず、内部専用のネットワークと一時的なMariaDBサービスを使用し、ポートを公開しません。
- コンテナは実際のサービスプロセスをフォアグラウンドのPID 1として動かし、再起動ポリシーを設定します。コンテナを生かすための無限ループは使用しません。

<a id="technology-comparisons"></a>

### 技術の比較

| 比較対象 | 違い | このプロジェクトでの選択 |
| --- | --- | --- |
| 仮想マシンとDocker | VMはハードウェアを仮想化し、独立したゲストカーネルを動かします。コンテナはホストのカーネルを共有し、プロセスとファイルシステムを分離するため、軽量で再作成も速くなります。 | 課題で指定されたVMの中でDocker構成を動かします。各アプリケーションの部品は、個別のコンテナに分離します。 |
| secretと環境変数 | 環境変数は通常の設定に便利ですが、プロセス情報、設定の出力、ログから見える場合があります。secretは必要なサービスだけにファイルとしてマウントします。 | 機密以外の設定には`srcs/.env`を使い、パスワードにはGitで除外した`secrets/`内のファイルとDocker Compose secretsを使います。 |
| Dockerネットワークとホストネットワーク | ユーザー定義のbridgeネットワークはコンテナ間の通信を分離し、サービス名によるDNSを提供します。ホストネットワークはホストのネットワーク名前空間を共有するため、分離が弱くなります。 | 専用のDockerネットワークでサービスを接続します。ホストネットワークと旧式のlinksは使用しません。 |
| Dockerボリュームとbindマウント | DockerボリュームのライフサイクルはDockerが管理します。bindマウントはコンテナ内のパスをホストのディレクトリに結び付けます。名前付きボリュームでも、`o: bind`を使うと内部ではbindマウントになります。 | ドライバーオプションのない通常のlocal名前付きボリュームを使います。VMのDocker data-rootを設定し、データを`/home/takawaka/data/docker/volumes/`以下に配置します。 |

<a id="instructions"></a>

## 操作手順

<a id="prerequisites"></a>

### 前提条件

- Linuxの仮想マシン
- Docker EngineとDocker Compose v2プラグイン
- GNU Make
- ローカルsecretの生成とHTTPS検証に使用するOpenSSL
- Dockerの実行権限と`/home/takawaka/data`の作成権限
- VMのアドレスを`takawaka.42.fr`に対応付けるローカルDNSまたは`/etc/hosts`の設定

<a id="configuration"></a>

### 設定

1. ローカルの環境ファイルを作成します。

   ```sh
   test -e srcs/.env || cp srcs/.env.example srcs/.env
   vim srcs/.env
   ```

2. `srcs/.env`の機密以外の必須設定を確認します。WordPressのユーザー名・メールアドレスの4項目は、`srcs/.env.example`では空欄でも構いませんが、起動前にコピー先の`srcs/.env`へ入力する必要があります。具体的な項目は[環境ファイルの作成手順](DEV_DOC_ja.md#2-create-the-environment-file)を参照してください。

   ローカルのテストや評価には、有効なサンプルのユーザー名とメールアドレスを使えます。WordPressの2アカウントには、互いに異なるユーザー名と、互いに異なる有効なメールアドレスを設定してください。セットアップやログインにメールの受信機能は不要ですが、通知やパスワード復旧には受信可能なアドレスを使います。

   既存の`srcs/.env`は上書きしないでください。管理者ユーザー名には、大文字小文字に関係なく`admin`や`administrator`を含められません。`BACKUP_SCHEDULE`は5フィールドのcron形式で、`BACKUP_RETENTION`は保持する完了済みバックアップの世代数です。

3. Compose設定が必要とするローカルのsecretファイルを生成します。

   ```sh
   ./srcs/tools/generate-secrets.sh
   ```

   スクリプトは`openssl`でパスワードごとのファイルを作成し、既存のファイルは変更しません。作成するファイルは次のとおりです。

   ```text
   secrets/db_root_password.txt
   secrets/db_password.txt
   secrets/wp_admin_password.txt
   secrets/wp_user_password.txt
   secrets/redis_password.txt
   secrets/ftp_password.txt
   ```

   ディレクトリをモード`700`、新規ファイルを`600`に設定します。既存ファイルの値と権限は維持されるため、[USER_DOCの認証情報](USER_DOC_ja.md#認証情報)に従って権限を制限してください。秘密ファイルをGitに追加してはいけません。

4. Webブラウザーを動かすマシンで、VMのIPアドレスと必要なドメインを対応付けます。

   ```text
   <VM_IP_ADDRESS> takawaka.42.fr
   ```

   VM内部のブラウザーを使う場合は、VMの`/etc/hosts`に`127.0.0.1 takawaka.42.fr`を設定します。名前解決ができたら、通常ユーザーのグラフィカルセッションからFirefox、Chrome、Chromiumのいずれかを開きます。

   ```sh
   make browser
   ```

   起動スクリプトは通常の名前解決を確認します。DNS設定は変更しません。

5. [DEV_DOC](DEV_DOC_ja.md)に従って、VMのDocker data-rootを設定します。`srcs/docker/daemon.json`は、既存のデーモン設定に統合するための設定例です。稼働中の環境を変更する場合は、先に既存データを移行してください。

<a id="build-and-run"></a>

### ビルドと起動

リポジトリのルートから実行します。

```sh
make
```

ローカル設定、secretの権限、Dockerストレージを検査し、Composeで各サービスのイメージをビルドして、バックグラウンドで起動します。Composeのプロジェクト名は`inception`です。提供されたVMでは`takawaka`がDockerへアクセスでき、すべてのMakeターゲットを実行できます。クリーンアップと復旧で必要な管理者権限は、インストール済みのストレージ用ヘルパーが提供します。ブラウザーは`takawaka`のグラフィカルセッションから実行してください。

評価時のクリーンアップで、コンテナが残っていない稼働中のシステムDockerデーモンのdata-rootが削除された場合、Makeはビルド前にそのデーモンを再起動します。条件と検証手順は[ストレージ復旧](srcs/docker/STORAGE_RECOVERY.md)を参照してください。削除済みのアプリケーションデータは、新しい環境として初期化されます。

主なターゲットは次のとおりです。

| コマンド | 操作 |
| --- | --- |
| `make` / `make up` | 設定確認、稼働対象サービスのビルド、バックグラウンドでの起動。 |
| `make check-config` | ローカルファイル、Compose、Dockerストレージを変更せずに検証。 |
| `make test` | 実Dockerデーモンを操作せず、分離した復旧、クリーンアップ、管理ヘルパー、サービスの回帰テストを実行。 |
| `make build` | コンテナを起動せず、稼働対象サービスのイメージをビルド・再ビルド。復元用イメージは`make restore-check`でビルドします。 |
| `make ps` | サービスの状態と公開ポートを表示。 |
| `make logs` | 全サービスのログを追跡。`Ctrl-C`で追跡を停止。 |
| `make stop` | コンテナを削除せずに停止。 |
| `make start` | 停止した既存コンテナを起動。 |
| `make restart` | イメージを再ビルドせず、既存コンテナを再起動。 |
| `make browser` | インストール済みブラウザーと通常のDNSでサイトを開く。 |
| `make backup` | バックアップを直ちに1世代作成。 |
| `make restore-check` | 新しいバックアップを作り、DBとWordPressのファイルを一時ストレージへ復元して検証。 |
| `make down` | コンテナとプロジェクトネットワークを停止・削除。 |
| `make clean` | Compose downで孤立コンテナも削除し、永続データを保持。 |
| `make fclean` | 評価用VMのすべてのDockerコンテナ、イメージ、ボリューム、カスタムネットワークと`/home/takawaka/data/*`を削除。サイトのデータとバックアップも含みます。 |

起動後のアクセス先は次のとおりです。

- Webサイト：`https://takawaka.42.fr`
- WordPress管理画面：`https://takawaka.42.fr/wp-admin/`
- 追加の静的サイト：`http://takawaka.42.fr:8080/`
- VM内部のAdminer：`http://127.0.0.1:8081/`（サーバー名：`mariadb`）

VM内部のFTPクライアントは、`srcs/.env`の`FTP_USER`を使って`127.0.0.1`の21番ポートへ接続します。パッシブモードでは21100～21110番ポートを使い、`FTP_PASV_ADDRESS`にはクライアントから到達できるアドレスを指定します。FTPとAdminerは認証情報を暗号化しないため、`ADMIN_BIND_ADDRESS`の既定値はどちらも`127.0.0.1`です。リモートからアクセスする場合は、USER_DOCで説明するSSHトンネルを使用してください。Redisは内部サービスで、ホストにポートを公開しません。

バックアップは`inception_backup_data`に保存します。ホスト側の保存先は`docker volume inspect inception_backup_data`で確認できます。`DATA_DIR`はMakeが検査するDockerストレージの期待パスを変更する設定で、デーモン設定そのものは変更しません。

日常の操作とトラブルシューティングは[USER_DOC_ja.md](USER_DOC_ja.md)、環境の準備と開発の詳細は[DEV_DOC_ja.md](DEV_DOC_ja.md)を参照してください。英語版は[USER_DOC.md](USER_DOC.md)と[DEV_DOC.md](DEV_DOC.md)です。

評価の準備では、最初に[操作コマンドのチェックリスト](USER_DOC_ja.md#operation-command-checklist)を確認し、[サービスの検証手順](USER_DOC_ja.md#service-verification-procedures)と[開発者向けのソース検査](DEV_DOC_ja.md#source-inspection-and-functional-verification)に進みます。データベースと再起動の確認には、[データベースと永続性の確認コマンド](USER_DOC_ja.md#database-and-persistence-commands)を使ってください。

これらの文書は、提出先のソースと認証情報の確認、初期クリーンアップ、起動、TLS、ユーザーとDBへのアクセス、ボリューム、ブラウザーでの編集、VM再起動、サービス設定の変更、5つのボーナスを扱います。クリーンアップはVMのDockerリソースとデータを完全に削除するため、必要なデータを退避したうえで、評価用VMだけで実行してください。

<a id="resources"></a>

## 参考資料

- [Dockerの概要](https://docs.docker.com/get-started/docker-overview/)
- [Docker Composeのアプリケーションモデル](https://docs.docker.com/compose/intro/compose-application-model/)
- [Docker Composeファイルのリファレンス](https://docs.docker.com/compose/compose-file/)
- [Dockerボリューム](https://docs.docker.com/engine/storage/volumes/)
- [Dockerのbindマウント](https://docs.docker.com/engine/storage/bind-mounts/)
- [Dockerのbridgeネットワークドライバー](https://docs.docker.com/engine/network/drivers/bridge/)
- [Dockerのhostネットワークドライバー](https://docs.docker.com/engine/network/drivers/host/)
- [Docker Compose secrets](https://docs.docker.com/compose/how-tos/use-secrets/)
- [Dockerボリュームのバックアップと復元](https://docs.docker.com/engine/storage/volumes/)
- [NGINXのHTTPS設定](https://nginx.org/en/docs/http/configuring_https_servers.html)
- [WordPressとNGINX](https://developer.wordpress.org/advanced-administration/server/web-server/nginx/)
- [MariaDB Serverドキュメント](https://mariadb.com/docs/server/)
- [MariaDB dumpドキュメント](https://mariadb.com/docs/server/clients-and-utilities/backup-restore-and-import-clients/mariadb-dump)
- [Redis Object Cacheプラグイン](https://wordpress.org/plugins/redis-cache/)
- [vsftpdドキュメント](https://security.appspot.com/vsftpd.html)
- [Adminer](https://www.adminer.org/)
- [Python HTTP server](https://docs.python.org/3/library/http.server.html)

<a id="ai-usage"></a>

### AIの使用範囲

AIは、要件分析、Dockerfileとentrypointスクリプト、WordPressの初期化、バックアップと復元検証、Makefile、ドキュメント、レビュー、VM上の検証を支援しました。また、Docker標準ボリュームへの移行、起動前の検査、ストレージ復旧の回帰テストにも使用しました。
