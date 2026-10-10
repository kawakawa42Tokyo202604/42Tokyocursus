FROM debian:12

RUN apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
		mariadb-server \
	&& rm -rf /var/lib/apt/lists/* \
	&& rm -rf /var/lib/mysql/* \
	&& mariadb-install-db --user=mysql --datadir=/var/lib/mysql --skip-test-db >/dev/null \
	&& chown -R mysql:mysql /var/lib/mysql

HEALTHCHECK --interval=2s --timeout=3s --retries=15 \
	CMD mariadb-admin --protocol=tcp --host=127.0.0.1 --port=3306 \
		--user=root --skip-password ping --silent || exit 1

EXPOSE 3306

ENTRYPOINT ["mariadbd"]
CMD ["--user=mysql", "--datadir=/var/lib/mysql", "--socket=/tmp/restore-db.sock", "--pid-file=/tmp/restore-db.pid", "--bind-address=0.0.0.0", "--port=3306", "--skip-grant-tables", "--skip-name-resolve", "--console"]
