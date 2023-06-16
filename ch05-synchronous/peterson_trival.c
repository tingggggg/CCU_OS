#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <signal.h>
#include <unistd.h>
#include <sched.h>

struct timespec ts = {0, 10};

struct timespec ts_big = {5, 10};

// CPU ID
int cpu_p0 = -1, cpu_p1 = -1;

//Peteron's solution variable
int turn=0;
int flag1 = 0;
int flag0 = 0;

// number of threads in CS
int in_cs = 0;
// number of each threads into CS
int p1_in_cs = 0;
int p0_in_cs = 0;

void per_second(int signum) {
    printf("Input C.S. times per secondes -> p0(on core#%d): %5d, p1(on core#%d): %5d\n", cpu_p0, p0_in_cs, 
                                                                                          cpu_p1, p1_in_cs);
    p0_in_cs = 0;
    p1_in_cs = 0;
    alarm(1);
}

void p0(void) {
    printf("p0: start\n");
    while (1) {
        flag0 = 1;
        turn = 1;
        while (flag1 == 1 && turn == 1)
            ;   //waiting

        // -- critical section --
        cpu_p0 = sched_getcpu();
        in_cs++;

        if (in_cs == 2) {
            fprintf(stderr, "Both p0 and p1 are in critical section\n");
            nanosleep(&ts_big, NULL);
        }

        p0_in_cs++;

        in_cs--;	
        // ----------------------

        flag0=0;
    }
}

void p1(void) {
    printf("p1: start\n");
    while (1) {
        flag1 = 1;
        turn = 0;
        while (flag0 == 1 && turn == 0)
            ;   //waiting

        // -- critical section --
        cpu_p1 = sched_getcpu();
        in_cs++;

        if (in_cs == 2) {
            fprintf(stderr, "Both p0 and p1 are in critical section\n");
            nanosleep(&ts_big, NULL);
        }
            

        p1_in_cs++;
        (&ts, NULL);

        in_cs--;
        // ----------------------

        flag1=0;
    }
}

int main(void) {
    pthread_t id1, id2;

    alarm(1);
    signal(SIGALRM, per_second);

    pthread_create(&id1, NULL, (void *) p0, NULL);
    pthread_create(&id2, NULL, (void *) p1, NULL);

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    return (0);
}
