PROGRAM = canzan
SRCS = main
CC = gcc
CFLAGS = -Wall -Wextra  -O2 `pkg-config --cflags gtk4`
LIBS = `pkg-config --libs gtk4`

all: ${PROGRAM}
	@if command -v bear > /dev/null; then bear -- make ${PROGRAM} --always-make; \
	else echo "Warning: 'bear' not install. Neovim paths won't auto update."; \
	fi

${PROGRAM}: ${SRCS}.c
	${CC} ${CFLAGS} -o ${PROGRAM} ${SRCS}.c ${LIBS}

clean:
	rm ${PROGRAM} compile_commands.json
