/*
Problem_6: Divide a one-dimensional array among MPI processes. Exchange halo/boundary values
between neighboring processes using MPI_Send and MPI_Recv, then perform a stencil computation.

Sample Input
Number of processes: 4
Array: 10 20 30 40 50 60 70 80
Radius = 1
Distribution:
P0: 10 20
P1: 30 40
P2: 50 60
P3: 70 80
Stencil: out[i] = left[i] + current[i] + right[i]

Sample Output
After halo exchange:
P0: [10 20] + right halo 30
P1: left halo 20, [30 40], right halo 50
P2: left halo 40, [50 60], right halo 70
P3: left halo 60, [70 80]
Stencil result (using the stated simple boundary convention):
30 60 90 120 150 180 210 230

run project = /usr/bin/mpicc problem_6.c -o problem_6
then /usr/bin/mpirun -np 4 ./problem_6
*/

#include <stdio.h>
#include <mpi.h>

#define N 8

int main(int argc, char **argv)
{
    int rank, processes, local[2], result[2], output[N];
    int array[N] = {10, 20, 30, 40, 50, 60, 70, 80};
    int left = 0, right = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);

    if (processes != 4)
    {
        if (rank == 0)
            printf("Run with exactly 4 processes.\n");
        MPI_Finalize();
        return 1;
    }

    MPI_Scatter(array, 2, MPI_INT, local, 2, MPI_INT, 0, MPI_COMM_WORLD);

    /* Exchange boundary values with neighboring processes. */
    if (rank % 2 == 0)
    {
        if (rank < processes - 1)
            MPI_Send(&local[1], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        if (rank > 0)
            MPI_Recv(&left, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
        if (rank > 0)
            MPI_Send(&local[0], 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD);
        if (rank < processes - 1)
            MPI_Recv(&right, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
    }
    else
    {
        MPI_Recv(&left, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        if (rank < processes - 1)
            MPI_Send(&local[1], 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
        if (rank < processes - 1)
            MPI_Recv(&right, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
        MPI_Send(&local[0], 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD);
    }

    for (int i = 0; i < 2; i++)
    {
        int l = i ? local[i - 1] : (rank ? left : 0);
        int r = i ? (rank == processes - 1 ? local[i] : right) : local[i + 1];
        result[i] = l + local[i] + r;
    }

    MPI_Gather(result, 2, MPI_INT, output, 2, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        printf("Stencil result: ");
        for (int i = 0; i < N; i++)
            printf("%d%c", output[i], i == N - 1 ? '\n' : ' ');
    }

    MPI_Finalize();
    return 0;
}
