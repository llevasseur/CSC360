Leevon Levasseur v00868326
University of Victoria, CSC 360 A1
February 29, 2020
Collaborated with Daniel Z. and Rylan C.

I was told that you HAVE to use semaphores in this part of the assignment so I re-did this part. I still do really like how this was implemented with pthread_mutex, it really solidified my understanding of locking a fork out. diningOut.c is the version with semaphores but I would really like to show how otherDiningOut.c looks like. I will leave descriptions for both.

diningOut.c: uses information from https://tutorialspoint.dev/computer-science/operating-systems/operating-system-dining-philosopher-problem-using-semaphores as help. The semaphores act as a signal rather than a hard lock to update data mutually exclusively. This approach was personally harder to wrap my head around but I do feel much more comfortable now. I wasn’t able to find a way to transfer otherDiningOut.c from mutex to semaphore, due to pthread_mutex_trylock(). I would love to know if there is an easy way to loop infinitely until something is unlocked with a semaphore.


otherDiningOut.c: solves the Dining Philosophers problem using the chandy/misra solution. An in-depth walk through of the solution is found at https://en.wikipedia.org/wiki/Dining_philosophers_problem.

I found it very useful to follow github.com/ethanttbui/dpp-chandy-misra-C to make the code.

On input, 3 integers are needed after the ./exec: NUM_PHILS, SEED, and TIME.
NUM_PHILS is the number of philosophers, number of resources, and forks.
SEED is the random seed used, and the time it takes to use the resource.
TIME is the length time you wish the experiment to go.

On input, for example ./diningOut 5 2 20





