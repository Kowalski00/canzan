PROGRAM = canzan

CC = gcc
PKGCONFIG = $(shell which pkg-config)
CFLAGS = -Wall -Wextra  -O2 `$(PKGCONFIG) --cflags gtk4`

LIBS = `$(PKGCONFIG) --libs gtk4`

GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources)

SRCS = $(wildcard *.c)
BUILT_SRC = src/resources.c

OBJS = $(BUILT_SRC:.c=.o) $(SRCS:.c=.o)

all:
	@if command -v bear > /dev/null; then bear -- make $(PROGRAM) --always-make; \
	else echo "Warning: 'bear' not install. Neovim paths won't auto update."; \
	fi

${PROGRAM}: ${OBJS}
	${CC} ${CFLAGS} -o $@ $(OBJS) ${LIBS}

src/recources.c src/resources.h: src/canzan.gresource.xml menu.ui
	$(GLIB_COMPILE_RESOURCES) src/canzan.gresource.xml --target=$@ --generate-source --generate-header --target=src/resources.h

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< 

gschemas.compiled: com.github.rkj.canzan.gschema.valid 
	$(GLIB_COMPILE_SCHEMAS)

clean:
	rm -f ${PROGRAM} compile_commands.json $(OBJS) src/resouces.c

.PHONY: all clean
