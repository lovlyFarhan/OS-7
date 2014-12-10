all:
	gcc -Wall -Wextra `pkg-config fuse --cflags` encfs.c -o pa5-encfs `pkg-config fuse --libs` aes-crypt.c aes-crypt.h -l crypto -lrt
