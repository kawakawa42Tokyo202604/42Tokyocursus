FROM debian:12

RUN apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends python3 \
	&& rm -rf /var/lib/apt/lists/* \
	&& useradd --system --home-dir /nonexistent --no-create-home \
		--shell /usr/sbin/nologin static-site

COPY site /var/www/static

RUN chown -R static-site:static-site /var/www/static

USER static-site
WORKDIR /var/www/static

EXPOSE 8080

CMD ["python3", "-m", "http.server", "8080", "--bind", "0.0.0.0", "--directory", "/var/www/static"]
