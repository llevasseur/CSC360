#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define MAX_SLEEP 10000000

unsigned int NUM_PHILS, SEED, TIME;

//mutex array for the forks
//sem_t mutex;
sem_t *fork_mutex;

//Philosophers' states (Thinking, Waiting, Eating, Terminated)
char **state;

//Who is holding the fork based on Philosophers ID
int *held_by;

//Who was using the fork based on ID
int *prev_used_by;

//Variable to determine if the watcher should continue running
int watcher_run = 1;

void *philosopher(void *t_id)
{
    int current_id = *(int*) t_id;
    int first_fork = current_id;
    int second_fork = current_id+1;

    //Case where current_id = NUM_PHILS-1 first and second fork have different ids
    if(current_id == NUM_PHILS - 1)
    {
        first_fork = 0;
        second_fork = NUM_PHILS - 1;
    }

    //exit while loop when current Id has been set to -1
    while(*(int*) t_id != -1)
    {
        state[current_id] = "Thinking";
        usleep(random() % MAX_SLEEP);

        state[current_id] = "Waiting";

        //pick up first fork
        sem_wait(&fork_mutex[first_fork]);
        held_by[first_fork] = current_id;
        int value;
        sem_getvalue(&for_mutex[second_fork], &value);
        printf("%d\n", value);

        //if second fork is not locked and first fork was last used by
        //current philosopher, release the first fork and wait again.
        while(sem_trywait(&fork_mutex[second_fork]))
        {
            printf("h!i\n");
            if(prev_used_by[first_fork] == current_id)
            {
                held_by[first_fork] = -1;
                prev_used_by[first_fork] = -1; //not actually used by current phil
                sem_post(&fork_mutex[first_fork]);
            }
        }

        held_by[second_fork] = current_id;

        //If current philosopher has the second fork, check to see if they still
        //have the first. If not, re-aquire.
        if(held_by[first_fork] != current_id)
        {
            sem_wait(&fork_mutex[first_fork]);
            held_by[first_fork] = current_id;
        }

        state[current_id] = "Eating";
        usleep(random() % MAX_SLEEP);

        //put both forks back down
        sem_post(&fork_mutex[first_fork]);
        held_by[first_fork] = -1;
        prev_used_by[first_fork] = current_id;

        sem_post(&fork_mutex[second_fork]);
        held_by[second_fork] = -1;
        prev_used_by[second_fork] = current_id;

    }

    state[current_id] = "Terminated";
    pthread_exit(NULL);

}

void *watcher()
{
    int i, used, available, thinking, waiting, eating;
    while(watcher_run)
    {
        used = 0, available = 0, thinking = 0, waiting = 0, eating = 0;
        //wait for 0.5 seconds before printing what we see
        usleep(500000);

        //begin printing
        printf("Philo   State               Fork    Held by\n");
        for(i = 0; i < NUM_PHILS; i++)
        {
            if (held_by[i] != -1)
            {
                printf("[%2d]:   %-20s[%2d]:     %d\n", i, state[i], i, held_by[i]);
                used++;

            } else {
                printf("[%2d]:   %-20s[%2d]:     Free\n", i, state[i], i);
                available++;

            }

            if(strcmp(state[i], "Thinking") == 0)
            {
                thinking++;

            } else if(strcmp(state[i], "Waiting") == 0) {
                waiting++;

            } else if(strcmp(state[i], "Eating") == 0) {
                eating++;
            }

        }

        printf("thinking=%2d waiting=%2d eating=%2d     used=%2d available=%2d\n\n\n", thinking, waiting,
      eating, used, available);

    }

    pthread_exit(NULL);

}

int main(int argc, char **argv)
{
    NUM_PHILS = atoi(argv[1]);
    SEED = atoi(argv[2]);
    TIME = atoi(argv[3]);

    pthread_t threads[NUM_PHILS -1];
    fork_mutex = (sem_t*) malloc (NUM_PHILS * sizeof(sem_t));
    //fork_mutex = (pthread_mutex_t*) malloc (NUM_PHILS * sizeof(pthread_mutex_t));
    if(fork_mutex == NULL)
    {
        printf("Error allocating memory for fork_mutex\n");
        return 1;
    }

    state = (char**) malloc (NUM_PHILS * sizeof(char*));
    if(state == NULL)
    {
        printf("Error allocating memory for state\n");
        return 1;
    }

    held_by = (int*) malloc (NUM_PHILS * sizeof(int));
    if(held_by == NULL)
    {
        printf("Error allocating memory for held_by\n");
        return 1;
    }

    prev_used_by = (int*) malloc (NUM_PHILS * sizeof(int));
    if(prev_used_by == NULL)
    {
        printf("Error allocating memory for prev_used_by\n");
        return 1;
    }


    int i;
    int phil_id[NUM_PHILS];
    //sem_init(&mutex, 0, 1)

    //initialize the arrays
    for(i = 0; i < NUM_PHILS; i++)
    {
        phil_id[i] = i;
        sem_init(&fork_mutex[i], 0, 0);
        state[i] = "Waiting";
        held_by[i] = -1;
        prev_used_by[i] = i;
    }

    //create and run threads
    srandom(SEED);
    for(i=0; i < NUM_PHILS; i++)
    {
        pthread_create(&threads[i], NULL, philosopher, (void*)&phil_id[i]);
    }
    pthread_create(&threads[NUM_PHILS], NULL, watcher, NULL);

    sleep(TIME);


    //Cancel and wait for philosopher threads to Terminate
    for(i = 0; i < NUM_PHILS; i++)
    {
        //Set id to -1 to tell that the thread should terminate after cycle
        phil_id[i] = -1;
    }

    for(i = 0; i < NUM_PHILS; i++)
    {
        //wait for termination
        pthread_join(threads[i], NULL);
    }

    //Cancel and wait for watcher thread to terminate
    watcher_run = 0;
    pthread_join(threads[NUM_PHILS], NULL);

    //destroy variables
    for(i = 0; i < NUM_PHILS; i++)
    {
        sem_destroy(&fork_mutex[i]);
    }

    //free up memory
    free(fork_mutex);
    free(state);
    free(held_by);
    free(prev_used_by);

    pthread_exit(NULL);
    //return 0;
}
