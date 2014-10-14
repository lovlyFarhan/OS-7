#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_MAX 10

pthread_mutex_t queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file  = PTHREAD_MUTEX_INITIALIZER;

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
    printf("%s", fileName);
    file = fopen(fileName, "r");

    if (file == NULL){
        printf("ERROR: File is Empty");
    }  

    while ((read = getline(&line, &len, file)) != -1) {
        printf("Retrieved line of length %d :\n", read);
        printf("%s", line);
    }

    if (line) {
        free(line);
    }

    // for each line
    // Lock the queue
    // Put the name on the queue
    // Unlock the queue
    return NULL;
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
    int i;
    int threadCount = argc-1;
    pthread_t threads[threadCount];
    int err;
    threadParam param[threadCount];

    for (i = 0; i < threadCount; i++) {
        param[i].fileName = argv[i+1];
        err = pthread_create(&(threads[i]), NULL, request, &param[i]);
        if (err){
            printf("ERROR on pthread create: %d\n", err);
        }
    }
    
    //Waiting for threads to finish
    for(i = 0; i<threadCount; i++){
        pthread_join(threads[i], NULL);
    }

    // Spawn THEAD_MAX threads in resolve
    
    return 0;
}
