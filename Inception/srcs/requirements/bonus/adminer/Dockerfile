FROM debian:12

RUN apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
		ca-certificates \
		curl \
		php8.2-cli \
		php8.2-mysql \
	&& rm -rf /var/lib/apt/lists/*

RUN mkdir -p /var/www/adminer \
	&& curl -fsSL --retry 3 --retry-all-errors \
		https://github.com/vrana/adminer/releases/download/v6.0.1/adminer-6.0.1-mysql.php \
		-o /var/www/adminer/index.php \
	&& printf '%s  %s\n' \
		52c6fe0f493fcffb531cbd465554f40abd528263892ee82702b9ad7d062b1739 \
		/var/www/adminer/index.php | sha256sum -c - \
	&& useradd --system --home-dir /nonexistent --no-create-home \
		--shell /usr/sbin/nologin adminer \
	&& chown -R adminer:adminer /var/www/adminer

USER adminer
WORKDIR /var/www/adminer

EXPOSE 8080

CMD ["php", "-S", "0.0.0.0:8080", "-t", "/var/www/adminer"]
