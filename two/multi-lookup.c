#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"
#include "util.h"

#define THREAD_MAX 10

pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file  = PTHREAD_MUTEX_INITIALIZER;
queue q;
int qSize = 1000000;

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
        int ret;
        char * ptr;
        ptr = malloc(strlen(line));
        strcpy(ptr, line);
        ret = queue_push(&q, ptr);
        if (ret) {
            printf("push returned: %d\n", ret);
        }
        pthread_mutex_unlock(&queueLock);
    }

    if (line) {
        free(line);
    }
    return NULL;
}

void resolve(void *output)
{
    (void) output;
    while (!queue_is_empty(&q)){
        // Take the domain off the queue
        char *ptr;
        char ip[100];
        char *pos;
        pthread_mutex_lock(&queueLock);
        ptr = queue_pop(&q);
        pthread_mutex_unlock(&queueLock);
        // Strip newline
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
}

int main(int argc, char *argv[])
{
    // Remove the old file
    FILE *f = fopen("results.txt", "w");
    if (!NULL) {
        if(unlink(f)) {
            printf("Error deleting output file: results.txt");
        }
    }
    fclose(f);

    queue_init(&q, qSize);
    // Create a thread for each file
    int threadCount = argc-1;
    pthread_t requesterThreads[threadCount];
    int i;
    int err;
    threadParam param[threadCount];
    for (i = 0; i < threadCount; i++) {
        param[i].fileName = argv[i+1];
        err = pthread_create(&(requesterThreads[i]), NULL, request, &param[i]);
        if (err){
            printf("ERROR on pthread create(request): %d\n", err);
        }
    }
    // Wait for each requester thread to finish
    for(i = 0; i<threadCount; i++){
        pthread_join(requesterThreads[i], NULL);
    }

    // Make THREAD_MAX threads to resolve the domain names
    pthread_t resolverThreads[threadCount];
    for (i = 0; i < THREAD_MAX; i++){
        err = pthread_create(&(resolverThreads[i]), NULL, resolve, NULL);
        if (err){
            printf("ERROR on pthread create (resolve): %d\n", err);
        }
    }
    // Wait for each resolver thread to finish
    for(i = 0; i<threadCount; i++){
        pthread_join(resolverThreads[i], NULL);
    }
    
    return 0;
}
