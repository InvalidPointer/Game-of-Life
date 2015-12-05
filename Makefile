CC = gcc
CFLAGS = -O2 -Wall -Werror -Wformat-security -Wignored-qualifiers -Winit-self -Wswitch-default -Wfloat-equal -Wpointer-arith -Wtype-limits -Wempty-body -Wno-logical-op -Wstrict-prototypes -Wold-style-declaration -Wold-style-definition -Wmissing-parameter-type -Wmissing-field-initializers -Wnested-externs -Wno-pointer-sign -std=gnu99
BIN = ./bin/

all: $(BIN)/life-server $(BIN)/life-client

$(BIN)/life-server: $(BIN)/life-server.o $(BIN)/board.o
		$(CC) -o $(BIN)/life-server $(BIN)/life-server.o $(BIN)/board.o

$(BIN)/life-server.o: life-server.c
		$(CC) $(CFLAGS) -c -o $(BIN)/life-server.o life-server.c

$(BIN)/board.o: board.c board.h
		$(CC) $(CFLAGS) -c -o $(BIN)/board.o board.c

$(BIN)/life-client:

clean:
		rm $(BIN)/*
