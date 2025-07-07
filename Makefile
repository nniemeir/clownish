SRC_CORE = \
src/config.c \
src/context.c \
src/envs.c \
src/error.c \
src/main.c \
src/tease.c

SRC_IO = \
src/file.c \
src/history.c \
src/input.c

SRC_EXEC = \
src/builtins.c \
src/exec.c \
src/signals.c

SRC_PARSE = \
src/parse_envs.c \
src/parse_flags.c \
src/parse_lex.c \
src/parse_stream.c \
src/parse_utils.c

SRC = $(SRC_CORE) $(SRC_IO) $(SRC_EXEC) $(SRC_PARSE)

NAME = clownish

DESTDIR = /usr/bin/

SRCMAN = man/

MANPAGE = clownish.1

COMPMAN = clownish.1.gz 

COMPRESS = gzip < $(SRCMAN)$(MANPAGE) > $(SRCMAN)$(COMPMAN)

MANDIR = /usr/share/man/man1/

MANDB = mandb

CC = gcc

BIN_DIR = bin

BUILD_DIR = build

OBJS = $(SRC:src/%.c=$(BUILD_DIR)/%.o)

CFLAGS = -Wall -Wextra -pedantic -g -I include

LDFLAGS = -lreadline

all: bin $(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME): $(OBJS)
	$(CC) -o $(BIN_DIR)/$(NAME) $(OBJS) $(CFLAGS) $(LDFLAGS)

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -g $(CFLAGS) -c $< -o $@

bin:
	mkdir -p $(BIN_DIR)

clean:
	rm -f $(OBJS)
	rm -rf $(BUILD_DIR)

cleanMan:
	rm -f $(SRCMAN)$(COMPMAN)

fclean: clean cleanMan
	rm -rf $(BIN_DIR)

install: $(BIN_DIR)/$(NAME) 
	cp -f -r $(BIN_DIR)/$(NAME) $(DESTDIR)
	$(COMPRESS)
	cp -f -r $(SRCMAN)$(COMPMAN) $(MANDIR)
	$(MANDB)

re: fclean all

uninstall: $(NAME)
	rm -f $(DESTDIR)$(NAME)
	rm -f $(MANDIR)$(COMPMAN)
	$(MANDB)

.PHONY: all clean cleanMan fclean install re uninstall
