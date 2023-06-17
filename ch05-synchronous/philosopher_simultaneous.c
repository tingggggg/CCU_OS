#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_PHILOSOPHERS 6

sem_t forks[NUM_PHILOSOPHERS];
sem_t eatPermission[NUM_PHILOSOPHERS];
sem_t eatSet;

int eat_times[NUM_PHILOSOPHERS] = {0};

void per_second_monitor(int signum)
{
    printf("Number of each philosopher:\n");
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        printf("\tp%d: %d\n", i, eat_times[i]);
    }
    alarm(1);
}

void* philosopher(void* arg) {
    int i = *((int*)arg);
    while (1) {
        
        sem_wait(&eatPermission[i]);

        sem_wait(&forks[i]);
        sem_wait(&forks[(i + 1) % NUM_PHILOSOPHERS]);

        eat_times[i]++;
        
        sem_post(&forks[i]);
        sem_post(&forks[(i + 1) % NUM_PHILOSOPHERS]);

        sem_post(&eatSet);
    }
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int i;

    alarm(1);
    signal(SIGALRM, per_second_monitor);

    for (i = 0; i < NUM_PHILOSOPHERS; ++i) {
        sem_init(&forks[i], 0, 1);
    }


    for (i = 0; i < NUM_PHILOSOPHERS; ++i) {
        sem_init(&eatPermission[i], 0, 0);
    }
    sem_init(&eatSet, 0, NUM_PHILOSOPHERS - 1);

    for (i = 0; i < NUM_PHILOSOPHERS; ++i) {
        int* philosopher_number = malloc(sizeof(int));
        *philosopher_number = i;
        pthread_create(&philosophers[i], NULL, philosopher, philosopher_number);
    }

    while (1) {
        sem_wait(&eatSet);
        int philosopher_id = rand() % NUM_PHILOSOPHERS;
        sem_post(&eatPermission[philosopher_id]);
    }

    for (i = 0; i < NUM_PHILOSOPHERS; ++i) {
        pthread_join(philosophers[i], NULL);
    }

    for (i = 0; i < NUM_PHILOSOPHERS; ++i) {
        sem_destroy(&forks[i]);
        sem_destroy(&eatPermission[i]);
    }
    sem_destroy(&eatSet);

    return 0;
}