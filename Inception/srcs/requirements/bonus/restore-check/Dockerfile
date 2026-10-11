# バックアップを展開し、一時 DB への復元を検証するためのクライアント用イメージを構築する。

FROM debian:12

RUN apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
		findutils \
		gzip \
		mariadb-client \
		tar \
	&& rm -rf /var/lib/apt/lists/*

COPY tools/restore-check.sh /usr/local/bin/restore-check.sh

RUN chmod 755 /usr/local/bin/restore-check.sh

ENTRYPOINT ["/usr/local/bin/restore-check.sh"]
