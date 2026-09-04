FROM debian:12

RUN apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
		redis-server \
	&& rm -rf /var/lib/apt/lists/*

COPY conf/redis.conf /etc/redis/redis.conf.template
COPY tools/entrypoint.sh /usr/local/bin/entrypoint.sh
COPY tools/healthcheck.sh /usr/local/bin/healthcheck.sh

RUN chmod 755 /usr/local/bin/entrypoint.sh /usr/local/bin/healthcheck.sh

EXPOSE 6379

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["redis-server", "/run/redis/redis.conf", "--daemonize", "no"]
