CC = gcc
CFLAGS = -std=c99 -Wall -Wextra 
SRC_DIR = src
SOURCES = $(wildcard $(SRC_DIR)/*.c)
HEADERS = $(wildcard $(SRC_DIR)/*.h)
OUT = dshell

.PHONY: all bdebug clean run valgrind

all: $(OUT)

$(OUT): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(OUT) $(SOURCES)

bdebug: CFLAGS += -g
bdebug: $(OUT)

clean:
	rm -f $(OUT)

run: $(OUT)
	./$(OUT)

valgrind: $(OUT)
	valgrind --track-origins=yes --leak-check=full ./$(OUT)

