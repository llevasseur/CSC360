#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <string.h>
//#include <threads.h>

#define THREADS 4
#define N 18

float minSum = INFINITY;
float minIntercept;
float minSlope;
float values[N] = {87.6, 88.9, 90.4, 91.3, 92.9, 95.4, 97.8, 100, 102.8, 104.7, 107, 109.1, 111.5, 114.1, 114.4, 116.5, 119.9, 121.7};
int* csvArr;
int csvSize;

pthread_mutex_t mutex;

struct arg_struct {
    int arg1;
    int arg2;
    float* arg3;
};

void findMinSum(int lower, int upper, float* newCsvArr) {
    //int length = sizeof(values)/sizeof(values[0]);
    //printf("this isnt actually used %d\n", upper);
    float slope, intersect, sum;
    for(int x1 = lower; x1 < upper; x1++) {
        for(int x2 = 0; x2 < csvSize; x2++) {
            if(x1 == x2) continue;
            //printf("%d\n", newCsvArr[x2]);
            slope = (newCsvArr[x2] - newCsvArr[x1]) / (x2 - x1);
            //printf("%8.4lf\n", slope);
            intersect = newCsvArr[x1] - slope * x1;
            sum = 0;
            for(int x3 = 0; x3 < csvSize; x3++) {
                float pointOnLine = intersect + slope * x3;
                float distance = fabsf(newCsvArr[x3] - pointOnLine);
                sum += distance;
            }
            
            pthread_mutex_lock(&mutex);
            if(sum < minSum) {
                // We found a best fit, update
                minSum = sum;
                minIntercept = intersect;
                minSlope = slope;
            }
            pthread_mutex_unlock(&mutex);
        }
    }
    
    
    
}

void* findMinSumHelper(void* arguements) {
    //printf("hello again\n");
    struct arg_struct *args = (struct arg_struct *)arguements;
    int lowerInt = args -> arg1;
    int upperInt = args -> arg2;
    float* newCsvArr = args -> arg3;
    //printf("test : %d\n", newnewCsvArr[0]);
    //float valuesInt[N] = args -> arg3;
    findMinSum(lowerInt, upperInt, newCsvArr);
    return NULL;
}

const char* getfield(char* line, int num)
{
	const char* tok;
	for (tok = strtok(line, ",");
			tok && *tok;
			tok = strtok(NULL, ";\n"))
	{
		if (!--num)
			return tok;
	}
	return NULL;
}

float* copyValues(int size, float* arr) {
    float* returnArr = malloc(size * sizeof(int));
    int test = 0;
    for(int i = 0; i < size; i++) {
        returnArr[i] = arr[i];
        test++;
    }
    
    // for(int i = 0; i < size; i++) {
    //     printf("%d\n", returnArr[i]);
    // }
    // printf("the length of it is %d\n", test);
    // printf("it should be %d\n", size);
    return returnArr;
}

bool checkIfSame(int* arr1, int* arr2) {
    for(int i = 0; i < csvSize; i++) {
        if(arr1[i] != csvArr[i] || arr2[i] != csvArr[i]) {
            return false;
        }
    }
    return true;
}

int main() {
    FILE* stream = fopen("./stremflow_time_series.csv", "r");
    char line[1024];
    int allocSize = 64;
    csvSize = 0;
    float* csvArr = malloc(sizeof(int) * allocSize);
    if(!csvArr) {
        fprintf(stderr, "malloc error");
        exit(1);
    }
    int i = 0;
    while(fgets(line, 1024, stream)) {
        char* tmp = strdup(line);
        const char* test = getfield(tmp, 2);
        int testAgain = atof(test);
        if(i == 0) {
            i++;
            continue;
        }
        csvArr[csvSize] = testAgain;
        csvSize++;
        if(csvSize >= allocSize) {
            allocSize *= 2;
            csvArr = realloc(csvArr, allocSize * sizeof(int));
            if(!csvArr) {
                fprintf(stderr, "Realloc Error!");
                exit(0);
            }
        }
    }
    double time_spent = 0.0;
    clock_t begin = clock();


    pthread_t thread[THREADS];

    printf("size of csv is %d\n", csvSize);
    pthread_mutex_init(&mutex, NULL);
    //printf("ahhhhhhhh %d", newCsvArr[1000]);
    for(int i = 0; i < THREADS; i++) {
        //printf("hello!\n");
        int lower = i * (csvSize / THREADS);
        int upper = (i + 1) * (csvSize / THREADS);
        struct arg_struct args;
        args.arg1 = lower;
        args.arg2 = upper;
        args.arg3 = copyValues(csvSize, csvArr);
        //memcpy(args.arg3, csvArr, csvSize + 1);
        pthread_create(&thread[i], NULL, &findMinSumHelper, (void *)&args);
    }

    for(int i = 0; i < THREADS; i++) {
      pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    printf("The min sum is %8.4lf\n", minSum);
    printf("The slope is %8.4lf\n", minSlope);
    printf("The intercept is %8.4lf\n", minIntercept);
    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("The elapsed time was %f seconds\n", time_spent);
}