<a id="user-documentation"></a>

# 利用者向けドキュメント

英語版：[USER_DOC.md](USER_DOC.md)

この文書では、エンドユーザーまたは管理者がInceptionスタックを操作する方法を説明します。

動作確認は、まず[操作コマンドのチェックリスト](#operation-command-checklist)を参照してください。MariaDBおよびVM再起動後のデータ永続性については[データベースと永続性の確認コマンド](#database-and-persistence-commands)、各サービスの詳細な確認方法については[サービスの検証手順](#service-verification-procedures)を参照してください。

<a id="services-provided"></a>

## 提供するサービス

| サービス | 役割 | ホストからのアクセス |
| --- | --- | --- |
| NGINX | TLS通信を終端し、WordPressサイトを配信します。 | 443番ポートのHTTPSのみ。 |
| WordPress + PHP-FPM | WordPressを実行し、PHPリクエストを処理します。 | Docker内部ネットワークからのみ。 |
| MariaDB | WordPressの投稿、ユーザー、設定、メタデータを保存します。 | Docker内部ネットワークからのみ。 |
| Redis | WordPressのオブジェクトキャッシュを保存します。 | Docker内部ネットワークからのみ。 |
| FTP | WordPressボリューム内のファイルにアクセスする機能を提供します。 | VMのlocalhost：21番ポートおよびパッシブ用21100～21110番ポート。 |
| 静的サイト | PHPを使わない追加のWebページを配信します。 | 8080番ポートのHTTP。 |
| Adminer | MariaDBを操作するWebインターフェースを提供します。 | VMのlocalhost：8081番ポートのHTTP。 |
| Backup | MariaDBとWordPressのデータを定期的に保存します。 | ホスト側にポートを公開しません。 |
| Restore-check | 一時ストレージを使用してバックアップを検証します。 | 必要なときにMakeから実行します。 |

各サービスは`srcs/docker-compose.yml`に定義された再起動ポリシーに従い、クラッシュ後に再起動します。WordPressファイルとMariaDBデータは名前付きボリュームに保存され、通常のコンテナ再作成では失われません。バックアップの各世代はバックアップ用ボリュームに保存されます。RedisとBackupは内部サービスです。その他の追加サービスが公開するポートは表に示したものだけです。

<a id="before-starting"></a>

## 起動前の確認

以下を確認してください。

- Linuxの仮想マシンが稼働していること。
- Dockerが稼働しており、現在のユーザーがアクセスできること。
- `srcs/.env`に、この環境で必要な機密情報以外の設定値が入力されていること。WordPress用のユーザー名とメールアドレスの4項目は、テンプレート`srcs/.env.example`では空欄でも構いませんが、起動前にコピー先の`srcs/.env`で必ず設定してください。[必須の4項目](DEV_DOC_ja.md#2-create-the-environment-file)を参照してください。ローカルのテストには有効なサンプルのユーザー名とメールアドレスを使用できます。WordPressの2つのアカウントには、互いに異なるユーザー名と、互いに異なる有効なメールアドレスが必要です。セットアップやログインのためにメールを受信できる必要はありません。
- `srcs/.env`の任意のバックアップ設定が有効であること。`BACKUP_SCHEDULE`は5フィールドのcron形式を使用し、`BACKUP_RETENTION`には1～1000の値を指定します。
- `srcs/docker-compose.yml`で要求されるパスワードファイルが`secrets/`に存在すること。
- クライアントマシンから`takawaka.42.fr`がVMのIPアドレスに名前解決されること。

パスワードを`srcs/.env`、Dockerfile、Composeのコマンドライン、Gitに保存しないでください。

<a id="start-the-project"></a>

## プロジェクトの起動

リポジトリのルートディレクトリで次を実行します。

```sh
make
```

この操作はローカル設定とDockerのdata-rootを検査し、イメージをビルドしてサービスをバックグラウンドで起動します。提供されたVMでは、`takawaka`はDockerグループに所属しており、通常のDocker/Makeコマンドを実行できます。完全クリーンアップとDockerストレージ復旧では、インストール済みの管理者用ヘルパーが自動的に使用されます。`make fclean`と、その後に最初に実行する`make`も含め、すべてのMakeターゲットは`takawaka`として実行してください。`make browser`はグラフィカルセッションから実行します。

`make`と`make up`は同じ操作です。Makeを再実行すると設定の再確認と再ビルドが行われますが、変更のないレイヤーにはDockerのキャッシュが使われます。既存の名前付きボリュームには、インストール済みのサイトとデータベースが保持されます。

サービスの状態と公開ポートを確認します。

```sh
make ps
```

常時稼働する必須サービスと追加サービスは、すべて実行中である必要があります。NGINXは`443`番ポートを公開し、追加サービスで想定するホスト側のポートは`21`、`21100-21110`、`8080`、`8081`です。Backupコンテナはポートを公開せずにスケジューラーを動かします。Restore-checkは意図的にデフォルトのスタックには含めていません。

<a id="stop-and-resume-the-project"></a>

## プロジェクトの停止と再開

コンテナを削除せずに停止します。

```sh
make stop
```

停止した同じコンテナを再度起動します。

```sh
make start
```

実行中のコンテナを再起動します。

```sh
make restart
```

コンテナとプロジェクトのネットワークを停止・削除します。

```sh
make down
```

`make down`と`make clean`では、名前付きボリュームと`/home/takawaka/data`以下のホストデータ（完了済みバックアップを含む）は保持されます。恒久的にデータを削除する意図があり、検証済みのバックアップが存在する場合を除き、`--volumes`を付けないでください。

<a id="full-cleanup-including-persistent-data"></a>

### 永続データを含む完全クリーンアップ

**`make fclean`は、VM内のすべてのDockerコンテナ、イメージ、ボリューム、カスタムネットワークを削除した後、`/home/takawaka/data/*`を削除します。このディレクトリ内のWordPress、データベースの内容、バックアップは完全に失われます。** 保存したいデータは、事前にこのディレクトリの外へ退避してください。[完全クリーンアップの手順](DEV_DOC_ja.md#3-perform-a-complete-cleanup-and-fresh-startup)も参照してください。

専用VM内のリポジトリから実行します。

```sh
make fclean
make
make ps
```

クリーンアップ処理は、削除前に管理者権限を検査します。リモートまたはrootless Dockerの接続先、異なるdata-root、安全でないパス、削除対象内のリポジトリや設定ファイルは拒否します。削除に失敗すれば中断し、Dockerリソースや入れ子のマウントが残っていないことを確認してから、データディレクトリ内の隠しファイル以外の項目を削除します。データディレクトリ外の`srcs/.env`と`secrets/`が、次回起動時の設定を提供します。この操作は、既存データを意図的にすべて削除し、一から再構築する場合にのみ使用してください。永続性の確認には通常の停止・再作成・再起動を使用します。

<a id="access-wordpress"></a>

## WordPressへのアクセス

ブラウザーで次のURLを開きます。

- Webサイト：`https://takawaka.42.fr`
- 管理画面：`https://takawaka.42.fr/wp-admin/`

VMの`/etc/hosts`で`127.0.0.1`を`takawaka.42.fr`に対応付けたら、グラフィカルデスクトップからFirefox、Chrome、Chromiumのいずれかでサイトを開きます。

```sh
make browser
```

起動スクリプトは通常の名前解決を利用し、名前を解決できない場合はエラーを出します。別のクライアントからアクセスするには、そのクライアント側のDNSまたはhostsの設定と、VMへのネットワーク経路が必要です。

`srcs/.env`で設定したWordPress管理者のユーザー名と、対応するローカルのsecretを使ってログインします。管理者ユーザー名には、大文字・小文字を問わず`admin`または`administrator`を含められません。

自己署名TLS証明書を使用している場合、ブラウザーに信頼性の警告が出ることがあります。警告を承認する前に、証明書がこのローカルプロジェクトのものであることを確認してください。

<a id="access-the-bonus-services"></a>

## 追加サービスへのアクセス

有効な場合は、次のエンドポイントを開きます。

- 静的サイト：`http://takawaka.42.fr:8080/`
- Adminer（VM内部）：`http://127.0.0.1:8081/`

Adminerでは`MySQL`または`MariaDB`を選択し、サーバー名に`mariadb`、ユーザー名とデータベース名には`srcs/.env`の`MYSQL_USER`と`MYSQL_DATABASE`を使用します。データベースのパスワードは`secrets/db_password.txt`にあります。

AdminerはHTTPを使用し、初期設定ではlocalhostにのみバインドされます。リモートアクセスする場合は、作業用PCで次を実行し、そのPC上で`http://127.0.0.1:8081/`を開いてください。

```sh
ssh -N -p 2222 -L 127.0.0.1:8081:127.0.0.1:8081 takawaka@127.0.0.1
```

FTPには`srcs/.env`の`FTP_USER`、`secrets/ftp_password.txt`のパスワード、VM内部でホスト`127.0.0.1`、ポート`21`を使用します。専用のFTPアカウントを使ってください。既存のアカウントを使う場合、そのUIDはrootおよびPHP-FPMユーザー（www-data）のUIDと異なる必要があります。どちらかとUIDを共有するエイリアスは拒否されます。

パッシブモードを有効にしてください。FTPの通信は暗号化されないため、こちらもlocalhostにバインドしています。ローカルクライアントやSSHトンネルでは`FTP_PASV_ADDRESS=127.0.0.1`を維持してください。外部からFTPを利用するには、作業用PC側で制御ポート（ローカル側は2121番）と、すべてのパッシブポートを転送します。

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

リモートのFTPクライアントは`127.0.0.1:2121`に接続します。`ADMIN_BIND_ADDRESS`を変更すると、両方の管理用サービスのバインド先が変わるため、変更前に公開範囲を検討してください。FTPアカウントは実行可能なサイトファイルを変更できるので、信頼できるアカウントだけを使用してください。

<a id="backup-and-restore-verification"></a>

## バックアップと復元の検証

スタックの稼働中に、直ちにバックアップを作成します。

```sh
make backup
```

このコマンドは、`/home/takawaka/data/docker/volumes/inception_backup_data/_data`以下に日時付きのバックアップ世代を保存します。スケジュールは既定でUTCを使用します。各世代には、圧縮したMariaDBダンプ、圧縮したWordPressファイルのアーカイブ、マニフェスト、SHA-256チェックサムが含まれます。作成途中の世代は完了済みバックアップとして公開されません。古い完了済み世代は`BACKUP_RETENTION`に従って削除されます（既定で7世代）。WordPressのアーカイブには`wp-config.php`が含まれるため、バックアップディレクトリと生成したアーカイブの権限は所有者のみがアクセスできるよう制限されます。別のバックアップがロックを保持している場合、世代を作らずに失敗します。そのバックアップの終了後に再実行してください。

新しいバックアップを作成し、復元処理全体を検証します。

```sh
make restore-check
```

Makeは新しい世代名を復元チェックへ渡すため、その世代を正確に検証します。バックアップ作成が失敗すれば復元チェックも中断します。チェックでは、チェックサムを検証し、WordPressアーカイブを一時ディレクトリへ展開し、SQLダンプを内部の専用ネットワーク上にある一時MariaDBサービスへインポートして、WordPressのテーブルと設定値が復元されたか確認します。稼働中のWordPressやMariaDBのデータボリュームはマウントせず、アプリケーションの稼働データも変更しません。

## 認証情報

機密ではない設定は次に保存します。

```text
srcs/.env
```

パスワードは次のディレクトリ内のローカルファイルとして保存します。

```text
secrets/
```

secretファイルの規約は次のとおりです。

| ファイル | 用途 |
| --- | --- |
| `secrets/db_root_password.txt` | MariaDBのrootパスワード。 |
| `secrets/db_password.txt` | WordPress用データベースユーザーのパスワード。 |
| `secrets/wp_admin_password.txt` | WordPress管理者のパスワード。 |
| `secrets/wp_user_password.txt` | WordPress一般ユーザーのパスワード。 |
| `secrets/redis_password.txt` | RedisとWordPressオブジェクトキャッシュ用のパスワード。 |
| `secrets/ftp_password.txt` | FTPユーザーのパスワード。 |

secretファイルは所有者のみが読み取れるようにします。初回起動前に、次の操作で権限を制限できます。

```sh
chmod 700 secrets
chmod 600 secrets/*.txt
```

モード`700`ではディレクトリ所有者だけが一覧表示、移動、変更できます。モード`600`ではファイル所有者だけが読み書きできます。生成スクリプトはディレクトリと新しく生成したファイルにこの権限を設定し、既存のファイルは保持します。Makeは起動時と`make check-config`の実行時に権限を検査し、グループやその他ユーザーにアクセス権がある場合は停止しますが、権限を自動修正することはありません。パスワードを表示せずに権限を確認します。

```sh
stat -c '%a %n' secrets secrets/*.txt
make check-config
```

`srcs/.env`と`secrets/`の内容はGitの追跡対象から除外されています。変更をコミットする前に確認してください。ログ、スクリーンショット、Issue、チャットに認証情報を貼り付けないでください。

既存のインストールでは、secretファイルを変更しただけではMariaDBやWordPressに保存済みの認証情報が更新されない場合があります。アプリケーション側の認証情報とsecretを合わせて変更し、影響を受けるコンテナを再作成してください。データベースの認証情報を変更する前には永続データをバックアップしてください。

<a id="check-that-the-stack-is-healthy"></a>

## スタックの正常性を確認する

コンテナの状態を表示します。

```sh
make ps
```

サービスのログを追跡します。

```sh
make logs
```

`Ctrl-C`を押すとログの追跡が終了します。コンテナ自体は停止しません。

VMまたはクライアントマシンからHTTPSの応答を確認します。

```sh
curl --insecure --head https://takawaka.42.fr
```

ネゴシエーションで選択されたTLSバージョンを調べます。

```sh
openssl s_client -connect takawaka.42.fr:443 -servername takawaka.42.fr \
  -tls1_2 -brief </dev/null
```

期待される結果は以下のとおりです。

- NGINXが443番ポートを公開し、追加サービスは文書で指定したポートのみを公開している。
- WebサイトがHTTPSで応答する。
- WordPressがコンテンツを読み込み、MariaDBへアクセスできる。
- WordPressがRedisのオブジェクトキャッシュを接続済みと報告する。
- 静的サイトとAdminerがそれぞれ8080番、8081番ポートで応答する。
- FTPクライアントがログインでき、パッシブモードでWordPressボリュームの内容を一覧表示できる。
- `make backup`によって完了済みバックアップ世代を作成できる。
- `make restore-check`によってデータベースとWordPressの復元成功が報告される。
- `make down`の後に`make up`を実行してもデータを利用できる。

<a id="operation-command-checklist"></a>

## 操作コマンドのチェックリスト

このコマンド一覧を使って、リポジトリ、設定、ネットワーク、TLS、永続データ、追加サービスを検査できます。新規初期化が不要な場合は`make fclean`を避け、既存データを保持してください。

VM内で`takawaka`としてリポジトリのルートからBashで実行します。`dc`、`wp_review`、`review_domain`は[手順1](#1-start-the-stack-and-define-checking-commands)に従って準備してください。表中のリンクは各手順へつながっています。前提条件と期待される結果も確認してください。

| 対象 | 基本コマンド・操作 | 詳細な確認と手順 |
| --- | --- | --- |
| ソースリポジトリ | `git clone`、`git remote get-url origin`、`git status --short` | [リポジトリURL、clone先、コミット、エイリアスを確認](DEV_DOC_ja.md#1-clone-and-identify-the-source-repository) |
| 設定とソース | `dc config --quiet`、`git ls-files`、`less Makefile srcs/docker-compose.yml` | [必要なファイル、Gitに登録されていないsecret、Dockerfile、スクリプトを確認](DEV_DOC_ja.md#2-prepare-the-shell-and-inspect-the-project-files) |
| 初期化（データ削除） | `make fclean` → `make` → `make ps` | [削除範囲、バックアップの注意点、Docker資源が空であること](DEV_DOC_ja.md#3-perform-a-complete-cleanup-and-fresh-startup) |
| システム構成 | Docker、Compose、イメージ、コンテナ、VM、スタックの構成を説明 | [アーキテクチャの概要](DEV_DOC_ja.md#verification-coverage-and-architecture-notes) |
| README | `head -n 1 README.md`、`less README.md` | [ファイル構成とドキュメント](DEV_DOC_ja.md#2-prepare-the-shell-and-inspect-the-project-files) |
| ドキュメント | `less USER_DOC.md DEV_DOC.md` | [利用者向け・開発者向けガイド](DEV_DOC_ja.md#2-prepare-the-shell-and-inspect-the-project-files) |
| 初回起動 | `make ps`、`make browser`、設定済みWordPressを開く | [HTTPS応答、80番ポートの無効化、WordPressの初期化](#2-verify-https-and-tls) |
| Dockerイメージ | Dockerfileを調査、`dc config --images`、`make -n up` | [イメージのビルドとベースイメージ](DEV_DOC_ja.md#2-prepare-the-shell-and-inspect-the-project-files)、[再起動動作とPID 1](DEV_DOC_ja.md#4-inspect-the-running-infrastructure) |
| Dockerネットワーク | `docker network ls`、`docker network inspect inception_inception` | [bridgeネットワーク、サービス名DNS、内部ポート](DEV_DOC_ja.md#4-inspect-the-running-infrastructure) |
| NGINX / TLS | `dc port nginx 443`、80番ポート、TLS 1.2・1.3を検査 | [curl、OpenSSL、証明書、NGINX設定](#2-verify-https-and-tls) |
| WordPress / PHP-FPM / ボリューム | `wp_review user list`、ボリューム確認、一般ユーザーでコメント追加、管理者でページ編集 | [ボリュームの保存先](#3-inspect-persistent-volumes)、[アカウントとブラウザーテスト](#4-check-wordpress-and-database-access) |
| MariaDB / ボリューム | DBへログイン、`SHOW TABLES;`、`SELECT COUNT(*) FROM wp_users;` | [基本・詳細DB確認](#database-and-persistence-commands) |
| データ永続性 | VM再起動 → 同じcloneで`make up ps browser` → 保存データとDBを確認 | [基本確認](#database-and-persistence-commands)、[JSONスナップショットとボリューム](#5-verify-persistence-across-container-recreation-and-vm-reboot) |
| 設定変更 | 設定を編集 → `make up` → `make ps` → 変更後のサービスを検査 | [NGINXポート変更と元に戻す例](DEV_DOC_ja.md#5-change-a-service-configuration-and-restore-it) |
| 追加サービス：Redis | `wp_review redis status`、別プロセス間でキャッシュキーをset/get | [healthcheck、接続、キャッシュの再利用、後片付け](#7-check-additional-services) |
| 追加サービス：FTP | curlでログイン・アップロードし、HTTPSから同じファイルを取得 | [パスワード入力、パッシブ転送、共有ボリューム、後片付け](#7-check-additional-services) |
| 追加サービス：静的サイト | `curl http://127.0.0.1:8080/`、VMのブラウザーで表示 | [HTTP応答とPHPを含まないソース](#7-check-additional-services) |
| 追加サービス：Adminer | `http://127.0.0.1:8081/`からDBにログインしWordPressテーブルを確認 | [接続、HTTP確認、SQLクエリ](#7-check-additional-services) |
| 追加サービス：Backup | cronスケジュール確認、`make restore-check` | [スケジュール、保存世代、チェックサム、一時領域への復元](#7-check-additional-services) |
| 設定を元に戻す | 保存した設定・WordPressのURLと定数を復元 → `make up` → 動作確認 | [設定のロールバック](DEV_DOC_ja.md#5-change-a-service-configuration-and-restore-it) |

**`make fclean`はサイト、データベース、バックアップを含むVM内のすべてのDockerリソースを削除します。** 必要なデータは使用前に別の場所へ保存してください。永続性のテスト中には絶対に実行しないでください。コマンドの出力だけでは、ブラウザー操作や実際のVM再起動の代わりにはなりません。

`make logs`でログを追跡し、`Ctrl-C`で終了します。短いログだけを見る場合は`dc logs --tail=100`を使用します。想定外の結果が出た場合は、解決してから次へ進んでください。

<a id="database-and-persistence-commands"></a>

## データベースと永続性の確認コマンド

この節ではMariaDB、そのボリューム、VM再起動後のデータ永続性を確認します。**VM内で`takawaka`としてcloneのルートからBashコマンドを実行してください。** ローカルのcloneが`~/asd`にある場合は次のコマンドを実行し、それ以外は該当ディレクトリに移動します。

```sh
cd ~/asd
```

`dc`と`wp_review`は[補助関数](#1-start-the-stack-and-define-checking-commands)です。準備済みVMでは新しいBash端末で使用できます。すでに開いている端末では`source ~/.bashrc`で読み込み直してください。新しいVMではリンク先の手順に従って関数を定義します。

| 対象 | 基本確認 | 詳細確認 |
| --- | --- | --- |
| MariaDB | サービス状態、DBログイン、テーブルとユーザー数 | Dockerfile、ボリュームパス、投稿・コメントのSQL |
| 永続性 | VM再起動、サービス再起動、ブラウザーとDBの確認 | 再起動前後のページ、コメント、ユーザー、ボリュームの比較 |

<a id="check-mariadb-and-its-volume"></a>

### MariaDBとそのボリュームの確認

**基本確認**

以下のコマンドでは`srcs/.env.example`のサンプルDB設定（`MYSQL_USER=wp_user`、`MYSQL_DATABASE=wordpress`）を使用します。設定を変更した場合は現在の値に置き換えるか、後述の詳細コマンドを使用してください。

```sh
make ps
dc exec mariadb mariadb -h127.0.0.1 -u wp_user -p wordpress
```

パスワードは`secrets/db_password.txt`に保存されています。入力中は文字が表示されません。MariaDBのプロンプトで次のSQLを実行してください。`EXIT;`で通常のシェルに戻ります。

```sql
SHOW TABLES;
SELECT COUNT(*) FROM wp_users;
EXIT;
```

MariaDBが`Up`および`healthy`で、WordPressのテーブルとユーザーが存在することを確認します。初期インストールでは通常12テーブル程度、2ユーザー程度です。ユーザーやプラグインの追加により件数は変わります。必要に応じてDockerfileとボリュームも確認してください。

**詳細確認**

```sh
cat srcs/requirements/mariadb/Dockerfile
docker volume ls
docker volume inspect inception_mariadb_data --format '{{.Mountpoint}}'
dc exec mariadb sh -c \
  'exec mariadb --protocol=TCP --host=127.0.0.1 --user="$MYSQL_USER" --password "$MYSQL_DATABASE"'
```

Dockerfileが`mariadb-server`をインストールし、NGINXを含まないことを確認してください。ボリュームのパスは`/home/takawaka/data/docker/volumes/inception_mariadb_data/_data`です。最後のコマンドはコンテナ内のDB設定を使ってログインします。パスワードは基本確認と同じです。

```sql
SELECT DATABASE();
SHOW TABLES;
SELECT COUNT(*) AS wordpress_users FROM wp_users;
SELECT post_type, COUNT(*) AS rows_count FROM wp_posts GROUP BY post_type;
SELECT comment_ID, comment_post_ID, comment_approved FROM wp_comments;
EXIT;
```

データベース名、ユーザー数、投稿、固定ページ、テスト用のコメントを確認します。標準のテーブル接頭辞は`wp_`です。変更している場合は実際の接頭辞を使ってください。

<a id="data-persistence-after-a-vm-reboot"></a>

### VM再起動後のデータ永続性

**基本確認**

1. ブラウザーで編集済みのページと公開済みコメントを開き、目印となるテキストとURLを記録します。MariaDBの基本確認でDBユーザー数も控えます。
2. 作業を保存し、VMのデスクトップの電源メニューからDebianを再起動します。
3. `takawaka`として再ログインし、同じcloneのディレクトリから次を実行します。

```sh
cd ~/asd
make up ps browser
```

`make up ps browser`はスタックのビルド・起動、サービス状態の表示、ブラウザー起動を順番に行います。cloneの名前が異なる場合は`cd`の移動先を変更してください。WordPressのインストールウィザードが表示されず、元のページとコメントが残り、管理者がログインできることを確認します。MariaDBの基本確認を再実行し、テーブル数とユーザー数を再起動前と比較します。

**詳細確認**

次のコマンドをVM再起動前と、再起動後にスタックが立ち上がってから実行し、結果を比較します。ユーザー、ページ本文、コメント、承認状態、ボリュームパスが変化していないことを確認してください。

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

結果をファイルに記録して比較する場合は[再起動前後のスナップショット手順](#5-verify-persistence-across-container-recreation-and-vm-reboot)を使用します。ブラウザーでも動作を確認してください。**永続性のテスト中には`make fclean`を実行しないでください。サイト、DB、バックアップが削除されます。**

<a id="service-verification-procedures"></a>

## サービスの検証手順

以下の手順では、実行中のデプロイ環境を検証します。完全に新しくインストールする必要がある場合は、最初に[完全クリーンアップ手順](DEV_DOC_ja.md#3-perform-a-complete-cleanup-and-fresh-startup)を実施しますが、データを保持する必要があるスタックでは実行しないでください。

以下は、デフォルトのプロジェクト名が`inception`であると想定します。**VM内部、リポジトリのルートから、`takawaka`として実行します。** Dockerへのアクセスとストレージ用管理者ヘルパーはDEV_DOCの説明どおりに設定してください。ブラウザー操作にはグラフィカルセッションが必要です。

以下のシェル関数はMakefileと同じComposeファイルと環境ファイルを使います。`type dc wp_review`で定義を確認できます。提供されたVMでは`takawaka`の`~/.bashrc`に定義済みであり、新しいBash端末では自動で読み込まれます。開いている端末では`source ~/.bashrc`を実行してください。新しいVMでは、以下のように作業用シェルで定義します。`srcs/.env`はcron値にCompose独自の環境ファイル構文を使用するため、シェルスクリプトとしてsourceしないでください。

<a id="1-start-the-stack-and-define-checking-commands"></a>

### 1. スタックを起動し、確認用コマンドを定義する

通常ユーザーの端末で、リポジトリからプロジェクトを起動します。

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

以降の確認では、この端末を開いたままにします。`mariadb`、`redis`、`wordpress`、`nginx`、`ftp`、`adminer`、`static-site`、`backup`の8サービスが実行中であることを想定します。MariaDB、Redis、WordPressは`healthy`になる必要があり、他のサービスにはhealthcheckが定義されていません。`restore-db`と`restore-check`は必要に応じて実行します。サービスが終了したり`unhealthy`のままだったりする場合は、次のログを確認してください。

```sh
dc logs --tail=100 mariadb wordpress nginx redis
```

<a id="2-verify-https-and-tls"></a>

### 2. HTTPSとTLSの検証

通常のDNS/hostsによる名前解決を確認した後、DNSやプロキシ設定に左右されない方法でVM内のNGINXを調べます。

```sh
getent ahostsv4 "$review_domain"
dc port nginx 443
curl --noproxy '*' --insecure --fail --silent --show-error \
  --resolve "$review_domain:443:127.0.0.1" \
  --output /dev/null --write-out 'HTTPS status: %{http_code}\n' "$review_url/"
curl --noproxy '*' --connect-timeout 3 --max-time 5 \
  --resolve "$review_domain:80:127.0.0.1" "http://$review_domain/"
```

HTTPSのステータス`200`と、NGINXの443番ポートに対するホスト側のポート対応が必要です。HTTPの接続確認は失敗する必要があり、通常はcurlの終了コード`7`になります。80番ポートからHTTP応答やリダイレクトが返る場合は要件を満たしません。DNSやプロキシへの接続が失敗しただけでは、80番ポートが閉じている証拠にはなりません。

許可した2種類のTLSバージョンを検証し、公開証明書を確認します。

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

TLS 1.2およびTLS 1.3のハンドシェイクが成功し、設定したドメインの証明書、`listen 443 ssl`、`ssl_protocols TLSv1.2 TLSv1.3`が確認できることを想定します。このローカル証明書が自己署名であれば、検証時の警告は想定内です。

古いプロトコルではネゴシエーションができないことを確認します。

```sh
openssl s_client -connect 127.0.0.1:443 -servername "$review_domain" \
  -tls1 -cipher 'ALL:@SECLEVEL=0' -brief </dev/null
openssl s_client -connect 127.0.0.1:443 -servername "$review_domain" \
  -tls1_1 -cipher 'ALL:@SECLEVEL=0' -brief </dev/null
```

サーバーがプロトコルバージョンを拒否し、ハンドシェイクに成功しないことが期待されます。クライアント側で`no protocols available`というエラーが出ても、サーバーが拒否した証拠にはなりません。NGINXの実際に展開された設定と、旧バージョンに対応したクライアントで確認してください。

**VMの通常ユーザーによる別のグラフィカル端末**から、同じcloneで`make browser`を実行します。`https://takawaka.42.fr/`と`https://takawaka.42.fr/wp-admin/`を開き、サイトが設定済みで、WordPressのインストールウィザードが表示されないことを確認します。curlの`--resolve`はブラウザーのDNSを設定しません。SSH端末だけではグラフィカル画面を提供しません。追加サービスの8080番・8081番のHTTPポートと、WordPressで禁止されている80番のHTTP入口は別です。

<a id="3-inspect-persistent-volumes"></a>

### 3. 永続ボリュームの確認

```sh
docker info --format '{{.DockerRootDir}}'
docker volume ls
docker volume inspect inception_wordpress_data inception_mariadb_data \
  inception_backup_data \
  --format '{{.Name}} driver={{.Driver}} options={{json .Options}} path={{.Mountpoint}}'
```

data-rootが`/home/takawaka/data/docker`、ドライバーが`local`、オプションが空（`null`または`{}`）であり、必須ボリュームのマウント先が次のとおりであることを確認します。

- WordPress：`/home/takawaka/data/docker/volumes/inception_wordpress_data/_data`。
- MariaDB：`/home/takawaka/data/docker/volumes/inception_mariadb_data/_data`。

バックアップ用ボリュームも同じdata-root以下に置かれます。これらはDocker標準の名前付きボリュームで、bind用ドライバーオプションは使用しません。[開発者向けの実行時確認](DEV_DOC_ja.md#4-inspect-the-running-infrastructure)には、各ボリュームを共有するコンテナと、読み取り専用マウントの有無が記載されています。

<a id="4-check-wordpress-and-database-access"></a>

### 4. WordPressとデータベースへのアクセス確認

インストール状態、WordPressの2アカウント、設定済みURLを確認します。

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

管理者と、それとは異なる一般ユーザー（この実装では`editor`）が存在することを確認します。管理者のログイン名には大文字小文字に関係なく`admin`を含められません。デフォルトポートでのURLは`https://takawaka.42.fr`である必要があります。これらのWordPressアカウントは、MariaDBのrootおよびアプリケーション用DBアカウントとは別です。

次のブラウザーテストも実施してください。

1. 対応するローカルsecretを使い、WordPressの一般ユーザーとしてログインします。コメントを許可した公開済み投稿に、識別できるコメントを追加します。承認制の場合は管理者ダッシュボードから承認します。
2. 管理者でログインしてページを編集・公開し、そのページの本文に識別できるテスト文字列を追加します。
3. ログアウトした状態またはプライベートウィンドウで公開投稿と固定ページを開き、コメントと編集後のページが表示されることを確かめます。再起動後の確認に備えて、投稿・ページ・コメントのIDを記録してください。CLIの一覧表示だけではこのテストの代わりになりません。

以下のMariaDBログインコマンドでは、アプリケーション用DBパスワードを対話的に入力するよう求められます。`secrets/db_password.txt`を他人に見えない場所で確認し、プロンプトに入力します。値をコマンドや画面キャプチャに含めないでください。

```sh
dc exec mariadb sh -c \
  'exec mariadb --protocol=TCP --host=127.0.0.1 --user="$MYSQL_USER" --password "$MYSQL_DATABASE"'
```

MariaDBのプロンプトでは次を実行します。

```sql
SELECT DATABASE();
SHOW TABLES;
SELECT COUNT(*) AS wordpress_users FROM wp_users;
SELECT post_type, COUNT(*) AS rows_count FROM wp_posts GROUP BY post_type;
SELECT comment_ID, comment_post_ID, comment_approved FROM wp_comments;
EXIT;
```

設定されたDB、空ではないWordPressのテーブル、2つのWordPressユーザー、テスト用コンテンツがあることを確認します。標準のテーブル接頭辞は`wp_`です。意図的に変更した場合はその接頭辞を使用します。MariaDB管理者が別に存在することを確認するには、`secrets/db_root_password.txt`の内容を他人に見せず、次のプロンプトで使用します。

```sh
dc exec mariadb mariadb --user=root --password
```

`SELECT User, Host FROM mysql.user;`を実行し、続いて`EXIT;`で終了します。MariaDBのrootはローカル用であり、アプリケーションDBユーザーはプロジェクトのネットワークから接続します。

<a id="5-verify-persistence-across-container-recreation-and-vm-reboot"></a>

### 5. コンテナ再作成とVM再起動後の永続性検証

前述の編集操作が終わった**後**に、ブラウザーで変更した内容とユーザーアカウントを記録します。比較用スナップショットはDockerデータディレクトリの外へ保存してください。

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

公開ページとコメントをもう一度開き、両方のテスト用目印が残っていることを確認します。この操作で確認できるのはコンテナの再作成であり、VMそのものの再起動ではありません。`--volumes`を使用したり、`make fclean`を実行したりしないでください。

開いている作業を保存した後、VM内のグラフィカルセッションの電源メニューからDebianを再起動します。再起動後に`takawaka`でログインし、Bash端末を開いて同じcloneへ移動します。SSHを使用する場合は`ssh -A`で再接続します。`COMPOSE_PROJECT_NAME`を解除し、手順1にあるように`dc`と`wp_review`が使用できることを確認します。`review_evidence`には再起動前に表示された正確なパスを設定してください。次の例のパスは書き換える必要があります。

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

各スナップショットコマンドが成功する必要があります。失敗したコマンドが生成した空のファイルは、有効な比較資料ではありません。テスト中に他のユーザーがサイトを変更していなければ、各`diff`は何も表示せず終了コード`0`を返すはずです。手順4のSQLクエリとブラウザー確認をもう一度実行し、ボリュームのマウント先とHTTPS応答も再確認してください。ホームページが設定されていることだけでは、編集済みデータが保持された証拠にはなりません。

<a id="6-rebuild-after-changing-configuration"></a>

### 6. 設定変更後の再ビルド

設定変更が反映されることを検証するには、[NGINXのポート変更と元に戻す手順](DEV_DOC_ja.md#5-change-a-service-configuration-and-restore-it)に従ってください。対象サービスと使用可能なポートを選択します。NGINXの例ではポートを変更し、WordPressの表示、ログイン、保存済みデータを確認してから、443番ポートに戻します。

<a id="7-check-additional-services"></a>

### 7. 追加サービスの確認

追加サービスは必要に応じて個別に確認できます。まずMariaDBとWordPressが正常に実行されていることを確かめてください。

**Redis：キャッシュへの接続と、別々のPHPプロセス間での再利用**

```sh
dc exec -T redis /usr/local/bin/healthcheck.sh
wp_review redis status
wp_review cache set review-probe cache-ok inception-review 60
wp_review cache get review-probe inception-review
wp_review cache delete review-probe inception-review
```

healthcheckが終了コード`0`を返し、Redisの状態が有効なdrop-inによる接続済みであり、別プロセスの`cache get`が60秒の有効期限内に`cache-ok`を表示することを確認します。最後のコマンドではテストキーだけを削除します。

**FTP：ログイン、パッシブ転送、共有WordPressボリューム**

これらのコマンドはVM内で実行します。`--user`にユーザー名だけを指定するとcurlがFTPパスワードを要求します。`secrets/ftp_password.txt`の内容を他人に見せずに使用してください。このローカルテストでは`FTP_PASV_ADDRESS=127.0.0.1`を維持します。

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

一覧にWordPressのファイルが表示され、アップロードが成功し、HTTPSからも同じテスト文字列を取得できることを確認します。後片付けでは新規作成したテストファイルのみを削除し、既存のサイトファイルは削除しません。作業用PCのクライアントには[FTPのSSHトンネル](#access-the-bonus-services)を使用します。パッシブFTPでは21番ポートだけの転送では不十分です。

**静的サイトとAdminer：HTTPの確認とブラウザーでの利用**

```sh
curl --noproxy '*' --fail --silent --show-error http://127.0.0.1:8080/
curl --noproxy '*' --fail --silent --show-error \
  --output /dev/null --write-out 'Adminer status: %{http_code}\n' http://127.0.0.1:8081/
dc exec -T static-site find /var/www/static -type f -name '*.php'
```

静的ページが表示され、Adminerのステータスが`200`であり、静的サイトにPHPファイルがないことを確認します。VMのブラウザーで両方を開いてください。Adminerではサーバー`mariadb`、設定済みDBユーザー、DBのsecretでログインし、WordPressのテーブルを参照して`SELECT COUNT(*) FROM wp_users;`を実行します。Adminerのログイン画面が表示されるだけでは、データベースにアクセスできる証拠になりません。

**Backup：定期保存と復元の検証**

```sh
dc exec -T backup cat /etc/cron.d/inception-backup
make restore-check
dc exec -T backup find /var/backups/inception \
  -mindepth 1 -maxdepth 1 -type d -name '20*' -printf '%f\n'
dc --profile ops ps --all
```

`make restore-check`は新しいバックアップの作成も行います。末尾が`OK`のチェックサム行、完了したバックアップ世代、テーブル数と設定項目数が正であることを示す`Restore verification succeeded`を確認してください。初回実行ではops用の2イメージもビルドするため、初回のビルド負荷が増えます。その後、単発実行の復元コンテナと一時DBは削除されます。定期バックアップ用サービスは稼働し続ける必要があります。バックアップはSQLデータとWordPressファイルの両方を保存し、復元テストでは稼働中のボリュームではなく一時領域を使います。通常の再起動ではバックアップが残りますが、`make fclean`はバックアップを`/home/takawaka/data`の外へコピーしていない限り、そのボリュームを削除します。

<a id="troubleshooting"></a>

## トラブルシューティング

<a id="cannot-access-docker-dockersock-permission-denied"></a>

### Dockerにアクセスできない / docker.sockの権限が拒否される

デーモンと現在の端末の所属グループを確認します。

```sh
systemctl is-active docker
id -nG
getent group docker
docker ps
```

Dockerデーモンが稼働中であり、現在のセッションのグループに`docker`が含まれ、Dockerへアクセスできることを確認します。グループ情報には`takawaka`が含まれているのに`id -nG`に`docker`が表示されない場合は、作業を保存してVMのデスクトップからログアウトし、再ログインしてください。以前のデスクトップセッション内で端末を新しく開いても、古いグループ情報が引き継がれます。初回設定については[Dockerグループの設定手順](DEV_DOC_ja.md#one-time-docker-group-setup-on-a-fresh-vm)を参照してください。`Cannot access Docker`と表示されても、`.env`の変更が必要とは限りません。最初にDockerデーモンへのアクセスを確認してください。

<a id="the-domain-does-not-resolve"></a>

### ドメインを名前解決できない

クライアントマシンの`/etc/hosts`またはDNSに、次の形式の登録があるか確認します。

```text
<VM_IP_ADDRESS> takawaka.42.fr
```

<a id="port-443-is-unreachable"></a>

### 443番ポートにアクセスできない

`make ps`、VMのファイアウォール、VMのネットワークモード、別プロセスによる443番ポートの使用を確認してください。

<a id="ftp-cannot-connect-or-list-files"></a>

### FTPに接続できない、ファイル一覧を表示できない

21番ポートとパッシブ用21100～21110番ポートに到達可能であること、`FTP_PASV_ADDRESS`がクライアントの使用するアドレスと一致すること、FTPサービスが`make ps`と`make logs`でエラーなく動いていることを確認してください。

<a id="a-service-repeatedly-restarts"></a>

### サービスが繰り返し再起動する

`make logs`を実行し、そのサービスで最初に発生したエラーを調べてください。主な原因として、secretファイルの不足、誤ったファイル権限、無効な設定、依存先サービスへの接続不能があります。

<a id="the-site-starts-but-data-is-missing"></a>

### サイトは起動するがデータが見つからない

名前付きボリュームが存在し、`/home/takawaka/data`以下に設定されたホストディレクトリに期待するデータがあることを確認してください。検証済みのバックアップがない状態で、既存のインストールを新しいデータベースで上書き初期化しないでください。

Docker自身のメタデータもこのディレクトリ以下にあります。完全クリーンアップでは意図的にデータとすべてのコンテナを削除します。その後`takawaka`として`make`を実行すると、data-rootが失われていれば、インストール済みヘルパーが空のローカルデーモンを再起動できます。その後、`srcs/.env`と`secrets/`を使用して新規環境を初期化します。削除済みの投稿やバックアップが復元されるわけではありません。[ストレージの復旧手順](srcs/docker/STORAGE_RECOVERY.md)を参照してください。

<a id="a-backup-cannot-be-created"></a>

### バックアップを作成できない

MariaDBとWordPressが`healthy`であること、WordPressボリュームが初期化済みであること、`secrets/db_password.txt`をDockerが読み取れることを確認してください。secretファイルの内容を表示せずに、バックアップサービスのログを調べてください。

<a id="restore-verification-fails"></a>

### 復元の検証が失敗する

最初に報告されたエラーを確認してください。チェックサムエラーはバックアップ世代が変更されたか不完全であること、DBエラーはSQLダンプを一時MariaDBへインポートできなかったこと、WordPressエラーはファイルアーカイブが不完全であることを意味します。このチェックは稼働中のアプリケーションデータを変更しません。
