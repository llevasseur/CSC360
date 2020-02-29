#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

#define BUFSIZE 100;
//Found on Stack Overflow https://stackoverflow.com/questions/12911299/read-csv-file-in-c
const char* getfield(char* line, int num)
{
    const char* tok;
    for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n"))
    {
        if (!--num)
        {
            return tok;
        }
    }
    return NULL;
}


int main() {

    FILE *infile;

    char *filename = "stremflow_time_series.csv";
    infile = fopen(filename, "r");
    char line[1024];
    int buffer = BUFSIZE;
    float *values = (float*) malloc (buffer * sizeof(float));
    int index = 0;

    if (infile == NULL)
    {
        //We shouldn't use the file
        printf("File %s could not be found\n", filename);
    } else {
        //We can use the file
        char *firstline = fgets(line, 1024, infile);
        while (fgets(line, 1024, infile))
        {
            char* tmp = strdup(line);
            values[index] = atof(getfield(tmp, 2));
            printf("Value is %f\n", values[index]);
            index++;
            free(tmp);
        }
    }
    printf("index is %d\n", index);

    //initalizations of variables
    //float values[18] = {87.6, 88.9, 90.4, 91.3, 92.9, 95.4, 97.8, 100, 102.8, 104.7, 107, 109.1, 111.5, 114.1, 114.4, 116.5, 119.9, 121.7};
    //int values_length = sizeof(values)/sizeof(values[0]);
    int values_length = index;
    int combos = 0;
    float slope, intersect, sum, tmp_value;
    float min_sum = 10000000.0;
    float best_slope, best_inter;
    int best_xs[2];
    float best_ys[2];
    printf("%d\n", values_length);

    for (int i = 0; i < values_length; i++) //i and j are x values that map to values which are the y
    {
        for (int j = i+1; j < values_length; j++) //j = i+1 to avoid checking a line twice
        {
            combos++;
            //caluclate the line: point 1 = (i,values[i]), point 2 = (j,values[j])
          //printf("i = %d, j = %d\n", i, j);
            slope = (values[j] - values[i]) / (j - i);
            //printf("slope: %f\n", slope);
            intersect = values[j] - (slope * j);
            sum = 0;
            for (int k = 0; k < values_length; k++) //Sum the distance from each other point to the line.
            {
                if (k == i || k == j) //One of the points we are using already (tmp_value = 0 so maybe don't need)
                {
                    continue;
                } else {
                    tmp_value = fabsf(values[k] - (slope*k + intersect));
                    sum += tmp_value;
                }
            }
            //printf("Count = %d, Sum = %f\n", combos, sum);
            if (sum < min_sum) //Found a new smallest min, update final values
            {
                min_sum = sum;
                best_slope = slope;
                best_inter = intersect;
                best_xs[0] = i+1;
                best_ys[0] = values[i];
                best_xs[1] = j+1;
                best_ys[1] = values[j];
            }
        }
    }
    printf("Min sum is: %f\n", min_sum);
    printf("With line: y = %f * x + %f\n", best_slope, best_inter);
    printf("With points: (%d, %f), (%d, %f)\n", best_xs[0], best_ys[0], best_xs[1], best_ys[1]);
    printf("combos: %d\n", combos);
    free(values);
    return 0;
}
