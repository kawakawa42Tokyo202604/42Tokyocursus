FROM debian:12

RUN apt-get update \
	&& DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
		acl \
		vsftpd \
	&& rm -rf /var/lib/apt/lists/* \
	&& mkdir -p /var/run/vsftpd/empty \
	&& chmod 755 /var/run/vsftpd/empty \
	&& if ! grep -qxF /usr/sbin/nologin /etc/shells; then \
		echo /usr/sbin/nologin >> /etc/shells; \
	fi

COPY conf/vsftpd.conf /etc/vsftpd/vsftpd.conf.template
COPY tools/entrypoint.sh /usr/local/bin/entrypoint.sh

RUN chmod 755 /usr/local/bin/entrypoint.sh

EXPOSE 21 21100-21110

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["vsftpd", "/etc/vsftpd/vsftpd.conf"]
