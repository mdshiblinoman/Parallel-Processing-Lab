/*
Problem_14: Each process generates a set of numerical values. Use MPI_Reduce to calculate the global
sum, maximum, minimum, and average.

Sample Input
Number of processes: 4
P0: 10 20 30
P1: 40 50 60
P2: 70 80 90
P3: 100 110 120

Sample Output
Local sums:
P0 = 60
P1 = 150
P2 = 240
P3 = 330
Global Sum = 780
Global Maximum = 120
Global Minimum = 10
Average = 65.00

Compile and run:
    /usr/bin/mpicc problem_14.c -o problem_14
    /usr/bin/mpirun -np 4 ./problem_14
*/

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size, values[3], local_sum, local_max, local_min, sums[16];
    int global_sum, global_max, global_min;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    values[0] = rank * 30 + 10;
    values[1] = rank * 30 + 20;
    values[2] = rank * 30 + 30;
    local_sum = values[0] + values[1] + values[2];
    local_max = values[2];
    local_min = values[0];

    MPI_Gather(&local_sum, 1, MPI_INT, sums, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Local sums:\n");
        for (int process = 0; process < size; process++)
            printf("P%d = %d\n", process, sums[process]);

        printf("Global Sum\n= %d\n", global_sum);
        printf("Global Maximum\n= %d\n", global_max);
        printf("Global Minimum\n= %d\n", global_min);
        printf("Average\n= %.2f\n", (double)global_sum / (size * 3));
    }

    MPI_Finalize();
    return 0;
}
