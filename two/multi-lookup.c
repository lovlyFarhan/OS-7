// AUTHORS: Dominic Tonozzi, Jacob Resman and the right honorable Edward Crawford
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include "queue.h"
#include "util.h"

#define THREAD_MAX 10

pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rLock = PTHREAD_MUTEX_INITIALIZER;
queue q;
int threadsDone;
int qSize = 10;

typedef struct {
    char *fileName;
} threadParam;

void* request(void* param)
{
    // Read the file
    FILE *file;
    char *line = NULL;
    size_t len = 0;
    char read;
    char *fileName;
    threadParam *param2 = param;
    fileName = param2->fileName;
    file = fopen(fileName, "r");

    if (file == NULL){
        printf("ERROR: Invalid input file.");
    }  

    while ((read = getline(&line, &len, file)) != -1) {
        pthread_mutex_lock(&queueLock);
        while (queue_is_full(&q)) {
            pthread_mutex_unlock(&queueLock);
            usleep(10);
            pthread_mutex_lock(&queueLock);
        }
        int ret;
		char *ptr;
		ptr = malloc(strlen(line));
		strcpy(ptr, line);
        ret = queue_push(&q, ptr);
        if (ret) {
            printf("push returned: %d\n", ret);
        }
	    pthread_mutex_unlock(&queueLock);
    }
	
    return NULL;
}

void* resolve(void *output)
{
    (void) output;
    while (!threadsDone | !queue_is_empty(&q)){
        // Take the domain off the queue
        char *ptr;
        char ip[100];
        pthread_mutex_lock(&queueLock);
        if (!queue_is_empty(&q)) {
            ptr = queue_pop(&q);
            pthread_mutex_unlock(&queueLock);
        } else {
            //randomize
            pthread_mutex_unlock(&queueLock);
            usleep(10);
            continue;
        }

        // Strip newline
        char *pos;
        if ((pos=strchr(ptr, '\n')) != NULL)
            *pos = '\0';

        if (dnslookup(ptr, ip, 64))
            *ip = '\0';

        // Write the file
        pthread_mutex_lock(&file);
        FILE *f = fopen("results.txt", "a");
        if (f == NULL) {
            printf("Error opening file!\n");
            exit(1);
        }
        fprintf(f, "%s,%s\n", ptr, ip);
        fclose(f);
        pthread_mutex_unlock(&file);
		free(ptr);
    }
	return NULL;
}

int main(int argc, char *argv[])
{
    FILE *f = fopen("results.txt", "w");
    if (f == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    fclose(f);

    queue_init(&q, qSize);
    // Create a thread for each file
    int threadCount = argc-1;
    pthread_t *requesterThreads = malloc(sizeof(pthread_t)*threadCount);
    threadsDone = 0;
    int i;
    int err;
    threadParam *param = malloc(sizeof(threadParam)*threadCount);
    for (i = 0; i < threadCount; i++) {
        param[i].fileName = argv[i+1];
        err = pthread_create(&(requesterThreads[i]), NULL, request, &param[i]);
        if (err){
            printf("ERROR on pthread create(request): %d\n", err);
        }
    }

    // Make THREAD_MAX threads to resolve the domain names
    pthread_t resolverThreads[THREAD_MAX];
    for (i = 0; i < THREAD_MAX; i++){
        err = pthread_create(&(resolverThreads[i]), NULL, resolve, NULL);
        if (err){
            printf("ERROR on pthread create (resolve): %d\n", err);
        }
    }

    // Wait for each thread to finish
    for(i = 0; i<threadCount; i++){
        pthread_join(requesterThreads[i], NULL);
    }
    threadsDone = 1;
    for(i = 0; i<threadCount; i++){
        pthread_join(resolverThreads[i], NULL);
    }
	queue_cleanup(&q);
  
	printf("param"); 
	free(param);
	printf("reqthread"); 
	free(requesterThreads);
 
    return 0;
}
