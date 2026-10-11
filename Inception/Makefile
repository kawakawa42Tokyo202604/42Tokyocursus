# サービスのビルド・起動・停止、テスト、バックアップと復元検証の操作をまとめる。

COMPOSE_FILE := srcs/docker-compose.yml
ENV_FILE := srcs/.env
LOGIN ?= takawaka
DATA_DIR ?= /home/$(LOGIN)/data
COMPOSE := docker compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE)

.PHONY: all check-config test prepare build up down start stop restart logs ps clean fclean browser backup restore-check

all: up

check-config:
	DATA_DIR="$(DATA_DIR)" ./srcs/tools/check-config.sh

test:
	./srcs/tests/test-storage-recovery.sh
	sh ./srcs/tests/test-fclean.sh
	sh ./srcs/tests/test-storage-admin.sh
	sh ./srcs/tests/test-service-regressions.sh

prepare:
	DATA_DIR="$(DATA_DIR)" ./srcs/tools/prepare-storage.sh
	DATA_DIR="$(DATA_DIR)" ./srcs/tools/check-config.sh

build: prepare
	@set -eu; \
	services="$$( $(COMPOSE) config --services )"; \
	for service in $$services; do \
		$(COMPOSE) build "$$service"; \
	done

up: build
	$(COMPOSE) up -d --no-build

down:
	$(COMPOSE) down

start: check-config
	$(COMPOSE) start

stop:
	$(COMPOSE) stop

restart: check-config
	$(COMPOSE) restart

logs:
	$(COMPOSE) logs -f

ps:
	$(COMPOSE) ps

clean:
	$(COMPOSE) down --remove-orphans

fclean:
	DATA_DIR="$(DATA_DIR)" sh ./srcs/tools/fclean.sh

browser:
	./srcs/tools/open-site.sh

backup: prepare
	$(COMPOSE) run --rm --no-deps --build backup /usr/local/bin/backup.sh now

restore-check: prepare
	@status=0; \
	backup_generation="$$(date -u '+%Y%m%dT%H%M%SZ')-$$(date -u '+%s%N')"; \
	$(COMPOSE) run --rm --no-deps --build backup \
		/usr/local/bin/backup.sh now "$$backup_generation" && \
	$(COMPOSE) --profile ops build restore-db restore-check && \
	$(COMPOSE) --profile ops run --rm restore-check "$$backup_generation" || status=$$?; \
	$(COMPOSE) --profile ops rm -sf restore-db >/dev/null 2>&1 || true; \
	exit $$status
