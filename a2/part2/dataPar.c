#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

#define BUFSIZE 100



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

    //char *filename = "stremflow_time_series_2002.csv";
    char *filename = "testHarness.csv";
    infile = fopen(filename, "r");
    char line[1024];
    int buffer = BUFSIZE;
    int max_index = buffer - 1;
    float *values = (float*) malloc (buffer * sizeof(float));

    if (values == NULL)
    {
        printf("error allocating mem\n");
        return 1;
    }

    int index = 0;

    if (infile == NULL)
    {
        //We shouldn't use the file
        printf("File %s could not be found\n", filename);
    } else {
        //We can use the file
        if (!fgets(line, 1024, infile)) return 1;
        while (fgets(line, 1024, infile))
        {
            if(index > max_index)
            {
                values = (float*) realloc(values, (max_index + 1 + buffer) * sizeof(float));
                if (values == NULL)
                {
                    printf("New Error allocating mem\n");
                    return 1;
                }
                max_index += buffer;
            }
            char* tmp = strdup(line);
            //values[index] = atof(getfield(tmp, 2));
            values[index] = atof(getfield(tmp, 8));
            index++;
            free(tmp);
        }
    }
    printf("index is %d\n", index);

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
            slope = (values[j] - values[i]) / (j - i);
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
