#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_PHILOSOPHERS 6
#define LEFT (i + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS
#define RIGHT (i + 1) % NUM_PHILOSOPHERS

sem_t forks[NUM_PHILOSOPHERS]; // Limiting the number of diners in the table
sem_t room;

int eat_times[NUM_PHILOSOPHERS] = {0};

void per_second_monitor(int signum)
{
    printf("Number of each philosopher:\n");
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        printf("\tp%d: %d\n", i, eat_times[i]);
    }
    alarm(1);
}

void *philosopher(void *arg)
{
    int i = *((int *)arg);
    while (1) {
        sem_wait(&room);

        sem_wait(&forks[LEFT]);
        sem_wait(&forks[RIGHT]);
        
        eat_times[i]++;

        sem_post(&forks[RIGHT]);
        sem_post(&forks[LEFT]);
        
        sem_post(&room);
    }
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int i;

    alarm(1);
    signal(SIGALRM, per_second_monitor);

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_init(&forks[i], 0, 1);
    }

    sem_init(&room, 0, NUM_PHILOSOPHERS - 1); // Limiting the number of diners in the table

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        int *philosopher_number = malloc(sizeof(int));
        *philosopher_number = i;
        pthread_create(&philosophers[i], NULL, philosopher, philosopher_number);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&forks[i]);
    }
    sem_destroy(&room);

    return 0;
}