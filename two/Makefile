CC = gcc
CFLAGS = -c -g -Wall -Wextra
LFLAGS = -Wall -Wextra -pthread

.PHONY: all clean

all: multi-lookup

multi-lookup : multi-lookup.o queue.o util.o
	$(CC) $(LFLAGS) $^ -o $@

multi-lookup.o: multi-lookup.c multi-lookup.h
	$(CC) $(CFLAGS) $<

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) $<

util.o: util.c util.h
	$(CC) $(CFLAGS) $<

clean:
	rm -f lookup queueTest pthread-hello multi-lookup
	rm -f *.o
	rm -f *~
	rm -f results.txt
