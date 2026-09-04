/*
Problem_16: Each MPI process stores a local value. Use MPI_Scan to calculate the prefix sum across all
processes and compare it with a manually implemented point-to-point solution.

Sample Input
Number of processes: 4
P0 = 10
P1 = 20
P2 = 30
P3 = 40

Sample Output
P0: Local value = 10, Prefix sum = 10
P1: Local value = 20, Prefix sum = 30
P2: Local value = 30, Prefix sum = 60
P3: Local value = 40, Prefix sum = 100
Prefix result: 10 30 60 100

Compile and run:
    /usr/bin/mpicc -Wall -Wextra problem_16.c -o problem_16
    /usr/bin/mpirun -np 4 ./problem_16

No input is required.
*/

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size, local_value, scan_sum, manual_sum;
    int scan_results[16], manual_results[16];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    local_value = (rank + 1) * 10;

    MPI_Scan(&local_value, &scan_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    if (rank == 0)
        manual_sum = local_value;
    else
    {
        MPI_Recv(&manual_sum, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        manual_sum += local_value;
    }

    if (rank < size - 1)
        MPI_Send(&manual_sum, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);

    MPI_Gather(&scan_sum, 1, MPI_INT, scan_results, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&manual_sum, 1, MPI_INT, manual_results, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        for (int process = 0; process < size; process++)
            printf("P%d: Local value = %d, Prefix sum = %d\n", process,
                   (process + 1) * 10, scan_results[process]);

        printf("Prefix result:");
        for (int i = 0; i < size; i++)
            printf(" %d", scan_results[i]);
        printf("\nManual result:");
        for (int i = 0; i < size; i++)
            printf(" %d", manual_results[i]);
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
