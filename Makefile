CFLAGS = -Wall -Wextra -pedantic -std=c99 -02
LDFLAGS = -ljson-c
CC = gcc

subtrans: sub_trans.c
	$(CC) $^ $(LDFLAGS) -o $@
%.o:%.c
	$(CC) $^ $(CFLAGS) -o $@
.PHONY: clean
clean:
	-rm -f subtrans.o
