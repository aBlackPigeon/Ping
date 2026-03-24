CC=gcc
CFLAGS=-Wall

all: myping

myping: ping.c checksum.c utils.c
	$(CC) $(CFLAGS) ping.c checksum.c utils.c -o myping

clean:
	rm -f myping