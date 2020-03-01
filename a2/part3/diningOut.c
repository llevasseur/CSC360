#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define N 5 //Number
#define LEFT (current_id + 4) % N
#define RIGHT (current_id + 1) % N

char **state;
int phil_id[N] = {0, 1, 2, 3, 4};

sem_t mutex;
sem_t S[N];

void check_for_forks(int current_id)
{
    if((strcmp(state[current_id], "Hungry") == 0) && (strcmp(state[LEFT], "Eating") != 0) && (strcmp(state[RIGHT], "Eating") != 0))
    {
        printf("hi\n");
        state[current_id] = "Eating";
        sleep(2);
        printf("Philosopher %d takes fork %d and %d\n", current_id, LEFT, RIGHT);

        //Used to wake up hungry philosophers during give_fork
        sem_post(&S[current_id]);
    }
}

void take_fork(int current_id)
{
    sem_wait(&mutex);

    //update state
    state[current_id] = "Hungry";

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
    state[current_id] = "Thinking";

    printf("Philosopher %d putting fork %d and %d down", current_id, LEFT, RIGHT);
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

    state = (char**) malloc (N * sizeof(char*));
    if(state == NULL)
    {
        printf("Error allocating memory for state\n");
        return 1;
    }

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

    free(state);
    //return 0;
}
