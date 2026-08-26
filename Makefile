PROGRAM = canzan
SRCS = main
CC = gcc
PKGCONFIG = $(shell which pkg-config)

CFLAGS = -Wall -Wextra  -O2 `$(PKGCONFIG) --cflags gtk4`
LIBS = `$(PKGCONFIG) --libs gtk4`

GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources)
GLIB_COMPILE_SCHEMAS = $(shell $(PKGCONFIG) --variable=glib_compile_schema)

SRC = main.c
BUILT_SRC = src/resources.c

OBJS = $(BUILT_SRC:.c=.o) $(SRC:.c=.o)

all:
	@if command -v bear > /dev/null; then bear -- make canzan --always-make; \
	else echo "Warning: 'bear' not install. Neovim paths won't auto update."; \
	fi

${PROGRAM}: ${SRCS}.c
	${CC} ${CFLAGS} -o ${PROGRAM} ${SRCS}.c ${LIBS}

clean:
	rm ${PROGRAM} compile_commands.json

canzan:
	${CC} ${CFLAGS} -o ${PROGRAM} ${SRCS}.c ${LIBS}

gschemas.compiled: com.github.rkj.canzan.gschema.valid 
	$(GLIB_COMPILE_SCHEMAS)

test: $(OBJS) gschemas.compiled
	$(CC) -o $(@F) $(OBJS) $(LIBS)

comprsc:
	glib-compile-resources src/canzan.gresource.xml --target=src/resources.c --generate-source
