Leevon Levasseur v00868326
University of Victoria, CSC 360 A1
February 29, 2020
Collaborated with Daniel Z. and Rylan C.

diningOut.c solves the Dining Philosophers problem using the chandy/misra solution. An in-depth walk through of the solution is found at https://en.wikipedia.org/wiki/Dining_philosophers_problem.

I found it very useful to follow github.com/ethanttbui/dpp-chandy-misra-C to make the code.

On input, 3 integers are needed after the ./exec: NUM_PHILS, SEED, and TIME.
NUM_PHILS is the number of philosophers, number of resources, and forks.
SEED is the random seed used, and the time it takes to use the resource.
TIME is the length time you wish the experiment to go.

On input, for example ./diningOut 5 2 20