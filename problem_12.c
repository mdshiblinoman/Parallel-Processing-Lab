/*
Problem_12: Use MPI_Scatter to distribute a large vector among processes. Each process calculates the
square or cube of its elements, and MPI_Gather collects the results at the root process.

Sample Input
Number of processes: 4
Vector: 1 2 3 4 5 6 7 8
After MPI_Scatter:
P0: 1 2
P1: 3 4
P2: 5 6
P3: 7 8
Operation: y[i] = x[i]^2

Sample Output
P0: 1 4
P1: 9 16
P2: 25 36
P3: 49 64
Gathered result: 1 4 9 16 25 36 49 64

Compile and run:
    /usr/bin/mpicc problem_12.c -o problem_12
    /usr/bin/mpirun -np 3 ./problem_12
*/

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size, vector[8], local[2], result[2], gathered[8];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 4)
    {
        if (rank == 0)
            printf("Run this program with 4 processes.\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0)
        for (int i = 0; i < 8; i++)
            scanf("%d", &vector[i]);

    MPI_Scatter(vector, 2, MPI_INT, local, 2, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < 2; i++)
        result[i] = local[i] * local[i];

    MPI_Gather(result, 2, MPI_INT, gathered, 2, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        for (int process = 0; process < 4; process++)
            printf("P%d: %d %d\n", process, gathered[2 * process], gathered[2 * process + 1]);

        printf("Gathered result:");
        for (int i = 0; i < 8; i++)
            printf(" %d", gathered[i]);
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
