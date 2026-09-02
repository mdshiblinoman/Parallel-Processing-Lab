/* Problem_1: Divide a large integer array among MPI processes. Each process exchanges its boundary
elements with neighboring processes using point-to-point communication and computes a local result.
Sample Input
Number of processes: 4
N = 12
Array: 10 20 30 40 50 60 70 80 90 100 110 120
Distribution:
P0: 10 20 30; P1: 40 50 60; P2: 70 80 90; P3: 100 110 120
Sample Output
P0: Right boundary sent = 30
P1: Left boundary received = 30
P1: Right boundary sent = 60
P2: Left boundary received = 60
P2: Right boundary sent = 90
P3: Left boundary received = 90
Boundary exchange completed successfully. */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size;

    int N = 12;

    int array[12] = {
        10, 20, 30,
        40, 50, 60,
        70, 80, 90,
        100, 110, 120};

    int local_array[3];

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 4)
    {
        if (rank == 0)
            printf("Run with exactly 4 processes.\n");

        MPI_Finalize();
        return 0;
    }

    /* Divide array among processes */
    MPI_Scatter(
        array,
        3,
        MPI_INT,
        local_array,
        3,
        MPI_INT,
        0,
        MPI_COMM_WORLD);

    int left_received;

    /*
       Even processes send first.
       Odd processes receive first.

       This avoids deadlock.
    */

    if (rank % 2 == 0)
    {
        /* Send right boundary */
        if (rank < size - 1)
        {
            MPI_Send(
                &local_array[2],
                1,
                MPI_INT,
                rank + 1,
                100,
                MPI_COMM_WORLD);
        }

        /* Receive left boundary */
        if (rank > 0)
        {
            MPI_Recv(
                &left_received,
                1,
                MPI_INT,
                rank - 1,
                100,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
        }
    }
    else
    {
        /* Receive left boundary */
        MPI_Recv(
            &left_received,
            1,
            MPI_INT,
            rank - 1,
            100,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);

        /* Send right boundary */
        if (rank < size - 1)
        {
            MPI_Send(
                &local_array[2],
                1,
                MPI_INT,
                rank + 1,
                100,
                MPI_COMM_WORLD);
        }
    }

    /*
       Print in order
    */

    for (int p = 0; p < size; p++)
    {
        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == p)
        {
            if (rank > 0)
                printf(
                    "P%d: Left boundary received = %d\n",
                    rank,
                    left_received);

            if (rank < size - 1)
                printf(
                    "P%d: Right boundary sent = %d\n",
                    rank,
                    local_array[2]);

            fflush(stdout);
        }
    }

    if (rank == 0)
        printf("Boundary exchange completed successfully.\n");

    MPI_Finalize();

    return 0;
}