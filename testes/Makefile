CC=gcc

INC_DIR=../include
LIB_DIR=../lib

CFLAGS := -std=gnu99 -Wall -Wextra

SRCS := $(wildcard *.c)

.PHONY: all clean

all: $(SRCS:.c=)

%: %.c $(LIB_DIR)/libcthread.a
	$(CC) $(CFLAGS) $< -o $@ -I$(INC_DIR) -L$(LIB_DIR) -lcthread

clean:
	rm -f $(patsubst %.c,%,$(SRCS))
