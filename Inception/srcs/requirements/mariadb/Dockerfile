# MariaDB と設定・初期化・ヘルスチェック用スクリプトを導入するイメージを構築する。

FROM debian:12

RUN apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends mariadb-server \
	&& rm -rf /var/lib/apt/lists/*

COPY conf/mariadb.cnf /etc/mysql/mariadb.conf.d/99-inception.cnf
COPY tools/entrypoint.sh /usr/local/bin/entrypoint.sh
COPY tools/healthcheck.sh /usr/local/bin/healthcheck.sh

RUN chmod +x /usr/local/bin/entrypoint.sh /usr/local/bin/healthcheck.sh \
	&& mkdir -p /var/lib/mysql \
	&& chown -R mysql:mysql /var/lib/mysql

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
