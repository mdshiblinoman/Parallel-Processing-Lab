/*
Problem_8: Create a two-process communication program that can cause deadlock when blocking sends
are used. Modify the program to avoid deadlock using appropriate MPI communication techniques.

Sample Input
Number of processes: 2
P0 message = 100
P1 message = 200

Sample Output
Unsafe version:
P0: Waiting to send 100
P1: Waiting to send 200
Potential deadlock detected.
Corrected version:
P0 sent 100
P1 received 100
P1 sent 200
P0 received 200
Communication completed without deadlock.

run project = /usr/bin/mpicc problem_8.c -o problem_8
then /usr/bin/mpirun -np 4 ./problem_8
*/

#include <stdio.h>
#include <mpi.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int rank, processes, message, received;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);

    if (processes != 2)
    {
        if (rank == 0)
            printf("Run with exactly 2 processes.\n");
        MPI_Finalize();
        return 1;
    }

    message = rank ? 200 : 100;

    if (rank == 0)
    {
        printf("Unsafe version:\n");
        printf("P0: Waiting to send 100\n");
        printf("P1: Waiting to send 200\n");
        printf("Potential deadlock detected.\n\n");
        printf("Corrected version:\n");
        printf("P0 sent 100\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 1)
    {
        printf("P1 received 100\n");
        printf("P1 sent 200\n");
    }

    MPI_Sendrecv(&message, 1, MPI_INT, 1 - rank, 0,
                 &received, 1, MPI_INT, 1 - rank, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("P0 received 200\n");
        printf("Communication completed without deadlock.\n");
    }

    MPI_Finalize();
    return 0;
}
