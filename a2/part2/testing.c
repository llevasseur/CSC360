//#include <stdbool.h>
#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
#include <pthread.h>

//#define NUM_THREADS 20

int x = 0;
pthread_mutex_t test_mutex; //Create mutex

//int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
//  void *(*start_routine) (void *), void *arg);

void *fun(void *in)
{
    int i;
    for (i = 0; i < 10000000; i++) {
        //Lock mutex before going to change variable
        pthread_mutex_lock(&test_mutex);
        x++;
        //Unlock mutex after changing the variable
        pthread_mutex_unlock(&test_mutex);
    }
    return 0;
}



int main() {
    FILE *infile;

    char *filename = "stremflow_time_series.csv";
    infile = fopen(filename, "r");

    if (infile == NULL)
    {
        //We shouldn't use the file
        printf("File %s could not be found\n", filename);
    } else {
        //We can use the file
        char c;
        while ((c = getc(infile)) != EOF)
        {
            //Do something
        }
    }

    pthread_t t1, t2;
    printf("Point 1 >> x is %d\n", x);
    pthread_mutex_init(&test_mutex, NULL); //Initialize mutex

    pthread_create(&t1, NULL, fun, NULL);
    pthread_create(&t2, NULL, fun, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&test_mutex); //Destroy mutex after use
    printf("Point 2 >> X is %d\n", x);
    return 0;
}
