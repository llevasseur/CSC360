Leevon Levasseur V00868326
University of Victoria, CSC 360 A1
February 29, 2020
Collaborated with Daniel Z. and Rylan C.

See PDF included to have a full evaluation test.

I will try to complete more of this tomorrow with the understanding that there will be a 10% reduction due to handing it in late. Here is what I have now, a single thread solution to finding L1.

My idea for what to do with multiple threads would be to divide the values into how ever many subsections that I will have the threads concurrently solve the equation for. Each subsection will start with the lowest value in it as the first point and check with every point above it, then repeat with the second lowest value, until the max value of the set has evaluated all points above it. This means that the first thread will do the majority of the work but still a quarter as much as one single thread did before. Thus, due to parallelism, the code should run much faster.


I wasn’t sure what a “test harness” was but I found a csv file that has the stocks for a couple companies and I used the volume part to get a L1.