COMPOSE_FILE := srcs/docker-compose.yml
ENV_FILE := srcs/.env
LOGIN ?= $(or $(LOGNAME),$(USER),$(shell id -un))
DATA_DIR ?= /home/$(LOGIN)/data
COMPOSE := DATA_DIR=$(DATA_DIR) docker compose -f $(COMPOSE_FILE) --env-file $(ENV_FILE)

.PHONY: all prepare build up down start stop restart logs ps clean browser backup restore-check

all: up

prepare:
	mkdir -p $(DATA_DIR)/mariadb $(DATA_DIR)/wordpress $(DATA_DIR)/backups
	chmod 700 $(DATA_DIR)/backups

build: prepare
	$(COMPOSE) build

up: prepare
	$(COMPOSE) up -d --build

down:
	$(COMPOSE) down

start:
	$(COMPOSE) start

stop:
	$(COMPOSE) stop

restart:
	$(COMPOSE) restart

logs:
	$(COMPOSE) logs -f

ps:
	$(COMPOSE) ps

clean:
	$(COMPOSE) down --remove-orphans

browser:
	./tools/open-site.sh

backup: prepare
	$(COMPOSE) run --rm --no-deps --build backup /usr/local/bin/backup.sh now

restore-check: backup
	$(COMPOSE) --profile ops run --rm --build restore-check
