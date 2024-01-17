CFLAGS = -Wall -Wextra -pedantic -std=c99 -02 -g
LDFLAGS = -ljson-c
CC = gcc -g

subtrans: main.c
	$(CC) $^ $(LDFLAGS) -o $@
%.o:%.c
	$(CC) $^ $(CFLAGS) -o $@
.PHONY: clean
clean:
	-rm -f subtrans.o
