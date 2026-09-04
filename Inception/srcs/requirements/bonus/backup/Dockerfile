FROM debian:12

RUN apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
		cron \
		gzip \
		mariadb-client \
		tar \
		util-linux \
	&& rm -rf /var/lib/apt/lists/*

COPY tools/backup.sh /usr/local/bin/backup.sh
COPY tools/entrypoint.sh /usr/local/bin/entrypoint.sh

RUN chmod 755 /usr/local/bin/backup.sh /usr/local/bin/entrypoint.sh

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["cron", "-f"]
