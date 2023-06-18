#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h> 
#include <stdatomic.h>
#include <assert.h>

__thread int thread_local_id;
atomic_int count;
atomic_int num_thread_in_cs;	//for debugging

struct ticketlock_t {
	volatile atomic_int next_ticket;	
	volatile int now_serving;			
} myTicketlock;

int thread_times[4] = {0};
void per_second_monitor(int signum)
{
    printf("Number of each threads (ticket number: %d):\n", myTicketlock.next_ticket);
    for (int i = 0; i < 4; i++) {
        printf("\tp%d: %d\n", i, thread_times[i]);
    }
    alarm(1);
}

void ticketLock_acquire(volatile atomic_int* next_ticket,volatile int* now_serving){
    int my_ticket;
    my_ticket = atomic_fetch_add(next_ticket, 1);
	//printf("thread_%d get tick %d\n", thread_local_id, my_ticket);	//for debugging
    while(*now_serving != my_ticket)
        ;
}

void ticketLock_release(volatile int *now_serving) {
    ++*now_serving;
	//printf("now_serving = %d\n", *now_serving);	//for debugging
}

void thread(void* para) {
	thread_local_id = atomic_fetch_add(&count, 1);
	pid_t x = syscall(__NR_gettid);		
	printf("pid = %d\n", getpid());
	printf("thread_%d's thread id is %d\n", thread_local_id, x);

	struct ticketlock_t* myTicketlock = (struct ticketlock_t*) para;
	//printf("thread ")
	for (int i=0; i< 1000000000; i++) {
		// printf("thread_%d wait for entering CS\n", thread_local_id);	//for debugging
		
		ticketLock_acquire(&myTicketlock->next_ticket, &myTicketlock->now_serving);
		// printf("thread_%d in CS\n", thread_local_id);
        thread_times[thread_local_id]++;
		atomic_fetch_add(&num_thread_in_cs, 1);	//for debugging;
		//printf("thread_%d in CS. loop_i = %d\n", thread_local_id, i);	//for debugging
		if (num_thread_in_cs > 1)	//for debugging
			fprintf(stderr, "ERROR: thread_%d is in CS, and num_thread is %d\n",thread_local_id, num_thread_in_cs);		//for debugging
		
		for (int i=0; i<random()%10000; i++)
			;

		atomic_fetch_sub(&num_thread_in_cs, 1);	//for debugging
		//printf("thread_%d exit CS. loop_i = %d\n", thread_local_id, i);	//for debugging
		ticketLock_release(&myTicketlock->now_serving);
		
		//remainder section
		for (int i=0; i<random()%10000; i++)	
			;

	}
}

int main(int argc, char** argv) {
	// struct ticketlock_t myTicketlock= {0};
	atomic_store(&num_thread_in_cs, 0);

    alarm(1);
    signal(SIGALRM, per_second_monitor);

	pthread_t* ptid;
    pthread_t w_id;
    int nThread = 4;
    if (argc == 2)
        sscanf(argv[1], "%d", &nThread);
    printf("Total %d reader threads\n", nThread);

    ptid = (pthread_t*)malloc(sizeof(pthread_t) * nThread);

    for (int i=0; i<nThread; i++) {
	    int ret = pthread_create(&ptid[i], NULL, (void *)thread, &myTicketlock);
        assert(ret==0);
    }

    for (int i=0; i<nThread; i++) {
	    pthread_join(ptid[i],NULL);
    }
}