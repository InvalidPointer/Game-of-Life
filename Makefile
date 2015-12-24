CC = gcc
CFLAGS = -g -O2 -Wall -Werror -Wformat-security -Wignored-qualifiers -Winit-self -Wswitch-default -Wfloat-equal -Wpointer-arith -Wtype-limits -Wempty-body -Wno-logical-op -Wstrict-prototypes -Wold-style-declaration -Wold-style-definition -Wmissing-parameter-type -Wmissing-field-initializers -Wnested-externs -Wno-pointer-sign -std=gnu99
BIN = ./bin/
.PHONY: all clean

all: $(BIN)/life-server $(BIN)/life-client

$(BIN)/life-server: $(BIN)/life-server.o $(BIN)/board.o $(BIN)/block.o $(BIN)/borders.o $(BIN)/output.o
		$(CC) -o $(BIN)/life-server $(BIN)/life-server.o $(BIN)/board.o $(BIN)/block.o $(BIN)/borders.o $(BIN)/output.o

$(BIN)/life-server.o: life-server.c commands.h board.h output.h | $(BIN)
		$(CC) $(CFLAGS) -c -o $(BIN)/life-server.o life-server.c

$(BIN)/board.o: board.c board.h borders.h block.h | $(BIN)
		$(CC) $(CFLAGS) -c -o $(BIN)/board.o board.c

$(BIN)/block.o: block.c block.h borders.h | $(BIN)
		$(CC) $(CFLAGS) -c -o $(BIN)/block.o block.c

$(BIN)/borders.o: borders.c borders.h | $(BIN)
		$(CC) $(CFLAGS) -c -o $(BIN)/borders.o borders.c

$(BIN)/output.o: output.c output.h board.h | $(BIN)
		$(CC) $(CFLAGS) -c -o $(BIN)/output.o output.c

$(BIN)/life-client: $(BIN)/life-client.o
		$(CC) -o $(BIN)/life-client $(BIN)/life-client.o

$(BIN)/life-client.o: life-client.c commands.h | $(BIN)
		$(CC) $(CFLAGS) -c -o $(BIN)/life-client.o life-client.c

$(BIN):
	mkdir -p $(BIN)

clean:
		rm $(BIN)/*
