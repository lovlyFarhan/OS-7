#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_MAX 10

pthread_mutex queue = PTHREAD_MUTEX_INITALIZER;
pthread_mutex file= PTHREAD_MUTEX_INITALIZER;

void request(char *input[])
{
	// Read the file
	// for each line
	// Lock the queue
	// Put the name on the queue
	// Unlock the queue
}

void resolve(char *output[])
{
	// while not locked
	// Lock the queue
	// If there's no more items in the queue, return
	// Take the domain off the queue
	// Unlock the queue
	// get an IP
	// lock the file
	// write the comma and IP
	// unlock the file
}

int main(int argc, char *argv[])
{
	// Call the requester for each file
	// Spawn THEAD_MAX threads in resolve
}
