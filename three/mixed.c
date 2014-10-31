/* Local Includes */
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sched.h>

#define DEFAULT_ITERATIONS 1000000
#define RADIUS (RAND_MAX / 2)

inline double dist(double x0, double y0, double x1, double y1){
    return sqrt(pow((x1-x0),2) + pow((y1-y0),2));
}

inline double zeroDist(double x, double y){
    return dist(0, 0, x, y);
}

int main(int argc, char* argv[]){

    (void) argc;
    long i;
    long iterations;
    struct sched_param param;
    int policy;
    int forks;
    double x, y;
    double inCircle = 0.0;
    double inSquare = 0.0;
    double pCircle = 0.0;
    double piCalc = 0.0;


   /* Process program arguments to select iterations and policy */
    iterations = 1000000;

    if(!strcmp(argv[1], "SCHED_OTHER")){
        policy = SCHED_OTHER;
    }
    else if(!strcmp(argv[1], "SCHED_FIFO")){
        policy = SCHED_FIFO;
    }
    else if(!strcmp(argv[1], "SCHED_RR")){
        policy = SCHED_RR;
    }
    else{
        fprintf(stderr, "Unhandeled scheduling policy\n");
        exit(EXIT_FAILURE);
    }

    forks = atoi(argv[2])-1;

    /* Set process to max priority for given scheduler */
    param.sched_priority = sched_get_priority_max(policy);

    /* Set new scheduler policy */
    fprintf(stdout, "Current Scheduling Policy: %d\n", sched_getscheduler(0));
    fprintf(stdout, "Setting Scheduling Policy to: %d\n", policy);
    if(sched_setscheduler(0, policy, &param)){
        perror("Error setting scheduler policy");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "New Scheduling Policy: %d\n", sched_getscheduler(0));

    while (forks > 0 && !fork()) {
        forks--;
    }

    char str[55];
    sprintf(str, "mixedOut-%d.txt", getpid());
   
    FILE *f = fopen(str, "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
        
    /* Calculate pi using statistical methode across all iterations*/
    for(i=0; i<iterations; i++){
        x = (random() % (RADIUS * 2)) - RADIUS;
        y = (random() % (RADIUS * 2)) - RADIUS;
        if(zeroDist(x,y) < RADIUS){
            inCircle++;
        }
        
        pCircle = inCircle/inSquare;
        piCalc = pCircle * 4.0;
        fprintf(f, "Intermediate Pi: %f\n", piCalc);
        
        inSquare++;
    }

    fclose(f);
    
    wait(NULL);
    return 0;
}
