#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <signal.h>
#include <unistd.h>
#include <sched.h>

struct timespec ts = {0, 10};

// CPU ID
int cpu_p0 = -1, cpu_p1 = -1;

//Peteron's solution variable
atomic_int turn=0;
atomic_int flag[2] = {0, 0};

// number of threads in CS
int in_cs = 0;
// number of each threads into CS
int p1_in_cs = 0;
int p0_in_cs =0;

void per_second(int signum) {
    static int p0_pre, p1_pre;
    printf("Input C.S. times per secondes -> p0(on core#%d): %5d, p1(on core#%d): %5d\n", cpu_p0, p0_in_cs, 
                                                                                          cpu_p1, p1_in_cs);    
    p0_pre = p0_in_cs;
    p1_pre = p1_in_cs;
    alarm(1);
}

void p0(void) {
    printf("start p0\n");
    while (1) {

        atomic_store(&flag[0], 1);
        atomic_thread_fence(memory_order_seq_cst);
        atomic_store(&turn, 1);
        while (atomic_load(&flag[1]) && atomic_load(&turn)==1)
                ;   //waiting

        // -- critical section --
        cpu_p0 = sched_getcpu();
        in_cs++;	

        //nanosleep(&ts, NULL);
        if (in_cs == 2) 
            fprintf(stderr, "Both p0 and p1 are in critical section\n");

        p0_in_cs++;	
        //nanosleep(&ts, NULL);

        in_cs--;	
		// ----------------------
        atomic_store(&flag[0], 0);
    } 
}

void p1(void) {
    printf("start p1\n");
    while (1) {
        atomic_store(&flag[1], 1);
        atomic_thread_fence(memory_order_seq_cst);
        atomic_store(&turn, 0);
        while (atomic_load(&flag[0]) && atomic_load(&turn)==0)
                ;   //waiting

        // -- critical section --
        cpu_p1 = sched_getcpu();

        in_cs++;
        //nanosleep(&ts, NULL);

        if (in_cs == 2) 
            fprintf(stderr, "Both p0 and p1 are in critical section\n");

        p1_in_cs++;
        //nanosleep(&ts, NULL);

        in_cs--;
        // ----------------------
        atomic_store(&flag[1], 0);
    } 
}

int main(void) {
	pthread_t id1, id2;

    alarm(1);
    signal(SIGALRM, per_second);

	pthread_create(&id1,NULL,(void *) p0,NULL);
	pthread_create(&id2,NULL,(void *) p1,NULL);

	pthread_join(id1,NULL);
	pthread_join(id2,NULL);

	return (0);
}
