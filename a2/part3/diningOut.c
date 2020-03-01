#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define N 5 //Number
#define LEFT (current_id + 4) % N
#define RIGHT (current_id + 1) % N
#define HUNGRY 1
#define EATING 0
#define THINKING 2

int state[N];
int phil_id[N] = {0, 1, 2, 3, 4};

sem_t mutex;
sem_t S[N];

void check_for_forks(int current_id)
{
    if((state[current_id] == HUNGRY) && (state[LEFT] != EATING) && (state[RIGHT] != EATING))
    {
        state[current_id] = EATING;
        sleep(2);
        printf("Philosopher %d takes fork %d and %d\n", current_id, LEFT, current_id);
        printf("Philosopher %d is Eating\n", current_id);

        //Used to wake up hungry philosophers during give_fork
        sem_post(&S[current_id]);
    }
}

void take_fork(int current_id)
{
    sem_wait(&mutex);

    //update state
    state[current_id] = HUNGRY;

    printf("Philosopher %d is Hungry\n", current_id);

    //You can eat if the neighbours are not
    check_for_forks(current_id);

    sem_post(&mutex);

    //wait if not able to eat. will be signaled when available
    sem_wait(&S[current_id]);

    sleep(1);
}

void give_fork(int current_id)
{
    sem_wait(&mutex);

    //update state
    state[current_id] = THINKING;

    printf("Philosopher %d putting fork %d and %d down\n", current_id, LEFT, current_id);
    printf("Philosopher %d is thinking\n", current_id);

    check_for_forks(LEFT);
    check_for_forks(RIGHT);

    sem_post(&mutex);
}

void *philosopher(void* t_id)
{
    while(1)
    {
        int *current_id = t_id;
        sleep(1);
        take_fork(*current_id);
        sleep(0);
        give_fork(*current_id);
    }
}


int main()
{
    int i;
    pthread_t thread_id[N];


    //initialize semaphores
    sem_init(&mutex, 0, 1);

    for(i = 0; i < N; i++)
    {
        sem_init(&S[i], 0, 0);
    }

    for(i = 0; i < N; i++)
    {
        //create philosopher threads
        pthread_create(&thread_id[i], NULL, philosopher, &phil_id[i]);
        printf("Philosopher %d is Thinking\n", i);
    }

    for(i = 0; i < N; i++)
    {
        pthread_join(thread_id[i], NULL);
    }

    //return 0;
}
