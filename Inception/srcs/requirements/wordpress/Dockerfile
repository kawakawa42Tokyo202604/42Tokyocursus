FROM debian:12

RUN apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
		ca-certificates \
		curl \
		mariadb-client \
		tar \
		php8.2-cli \
		php8.2-curl \
		php8.2-fpm \
		php8.2-gd \
		php8.2-mbstring \
		php8.2-mysql \
		php8.2-redis \
		php8.2-xml \
		php8.2-zip \
		unzip \
		&& rm -rf /var/lib/apt/lists/*

RUN mkdir -p /usr/src/wordpress /var/www/html \
	&& curl -fsSL --retry 3 --retry-all-errors \
		https://wordpress.org/wordpress-6.8.2.tar.gz \
		-o /tmp/wordpress.tar.gz \
	&& printf '%s  %s\n' \
		03baad10b8f9a416a3e10b89010d811d9361e468 \
		/tmp/wordpress.tar.gz | sha1sum -c - \
	&& tar -xzf /tmp/wordpress.tar.gz \
		--strip-components=1 --directory=/usr/src/wordpress \
	&& rm -f /tmp/wordpress.tar.gz \
	&& chown -R www-data:www-data /usr/src/wordpress /var/www/html \
	&& find /usr/src/wordpress /var/www/html -type d -exec chmod 755 {} + \
	&& find /usr/src/wordpress /var/www/html -type f -exec chmod 644 {} +

RUN curl -fsSL --retry 3 --retry-all-errors \
		https://github.com/wp-cli/wp-cli/releases/download/v2.12.0/wp-cli-2.12.0.phar \
		-o /usr/local/bin/wp \
	&& printf '%s  %s\n' \
		be928f6b8ca1e8dfb9d2f4b75a13aa4aee0896f8a9a0a1c45cd5d2c98605e6172e6d014dda2e27f88c98befc16c040cbb2bd1bfa121510ea5cdf5f6a30fe8832 \
		/usr/local/bin/wp | sha512sum -c - \
		&& chmod 755 /usr/local/bin/wp

RUN curl -fsSL --retry 3 --retry-all-errors \
		https://downloads.wordpress.org/plugin/redis-cache.2.8.0.zip \
		-o /tmp/redis-cache.zip \
	&& printf '%s  %s\n' \
		f077ac8b9c154cee936d3872b0734d42f7fd7f350e68fe8b2b20a69e854980cd \
		/tmp/redis-cache.zip | sha256sum -c - \
	&& unzip -q /tmp/redis-cache.zip -d /tmp/redis-cache \
	&& test -f /tmp/redis-cache/redis-cache/redis-cache.php \
	&& mkdir -p /usr/src/wordpress/wp-content/plugins \
	&& mv /tmp/redis-cache/redis-cache /usr/src/wordpress/wp-content/plugins/redis-cache \
	&& rm -rf /tmp/redis-cache /tmp/redis-cache.zip

COPY conf/www.conf /etc/php/8.2/fpm/pool.d/www.conf
COPY tools/create-wp-config.php /usr/local/bin/create-wp-config.php
COPY tools/configure-wp-redis.php /usr/local/bin/configure-wp-redis.php
COPY tools/entrypoint.sh /usr/local/bin/entrypoint.sh

RUN chmod 644 /usr/local/bin/create-wp-config.php \
		&& chmod 644 /usr/local/bin/configure-wp-redis.php \
		&& chmod 755 /usr/local/bin/entrypoint.sh

WORKDIR /var/www/html

EXPOSE 9000

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["php-fpm8.2", "--nodaemonize"]
