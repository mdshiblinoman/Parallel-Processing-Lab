/*
Problem_18: Divide a large dataset among MPI processes. Each process generates a local histogram, and
MPI_Allreduce combines all local histograms into a global histogram.

Sample Input
Number of processes: 4
Values range from 0 to 9.
P0: 1 2 2 3
P1: 2 3 4 4
P2: 1 4 5 5
P3: 0 2 5 9

Sample Output
Final histogram after MPI_Allreduce:
Value Frequency
 0       1
 1       2
 2       4
 3       2
 4       3
 5       3
 6       0
 7       0
 8       0
 9       1

Total number of elements = 20

Compile and run:
    /usr/bin/mpicc -Wall -Wextra problem_18.c -o problem_18
    /usr/bin/mpirun -np 4 ./problem_18
*/

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, values[4], local_histogram[10] = {0}, global_histogram[10];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    values[0] = rank == 0 ? 1 : rank == 1 ? 2
                            : rank == 2   ? 1
                                          : 0;
    values[1] = rank == 0 ? 2 : rank == 1 ? 3
                            : rank == 2   ? 4
                                          : 2;
    values[2] = rank == 0 ? 2 : rank == 1 ? 4
                            : rank == 2   ? 5
                                          : 5;
    values[3] = rank == 0 ? 3 : rank == 1 ? 4
                            : rank == 2   ? 5
                                          : 9;

    for (int i = 0; i < 4; i++)
        local_histogram[values[i]]++;

    MPI_Allreduce(local_histogram, global_histogram, 10, MPI_INT,
                  MPI_SUM, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Final histogram after MPI_Allreduce:\n");
        printf("Value Frequency\n");
        for (int value = 0; value < 10; value++)
            printf("%2d %8d\n", value, global_histogram[value]);
    }

    MPI_Finalize();
    return 0;
}
