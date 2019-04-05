AR=ar
CC=gcc

BIN_DIR=bin
INC_DIR=include
LIB_DIR=lib
SRC_DIR=src

CFLAGS := -std=gnu99 -Wall -Wextra

CTHREAD_SRC=$(SRC_DIR)/cthread.c
CTHREAD_OBJ=$(BIN_DIR)/cthread.o
CTHREAD_LIB=$(LIB_DIR)/libcthread.a

SUPPORT_SRC=$(BIN_DIR)/support.c
SUPPORT_OBJ=$(BIN_DIR)/support.o
SUPPORT_OBJ64=$(BIN_DIR)/support64.o

.PHONY: all all64 clean

all: $(CTHREAD_OBJ) $(SUPPORT_OBJ)
	rm -f $(CTHREAD_LIB)
	$(AR) crs $(CTHREAD_LIB) $^

all64: $(CTHREAD_OBJ) $(SUPPORT_OBJ64)
	rm -f $(CTHREAD_LIB)
	$(AR) crs $(CTHREAD_LIB) $^

$(CTHREAD_OBJ): $(CTHREAD_SRC)
	$(CC) $(CFLAGS) -c $^ -o $@ -I$(INC_DIR)

$(SUPPORT_OBJ):
	

$(SUPPORT_OBJ64): $(SUPPORT_SRC)
	$(CC) $(CFLAGS) -c $^ -o $@ -I$(INC_DIR)

clean:
	rm -f $(CTHREAD_OBJ) $(SUPPORT_OBJ64) $(CTHREAD_LIB)
