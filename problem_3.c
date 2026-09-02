/*
Problem_3: Implement a ring topology in which every MPI process sends a message to its next process
and receives a message from its previous process. Measure the communication time for different message
sizes.

Sample Input
Number of processes = 4
Initial value at P0 = 100
Communication: P0 → P1 → P2 → P3 → P0

Sample Output
P0 sends 100 to P1
P1 receives 100 from P0
P1 sends 100 to P2
P2 receives 100 from P1
P2 sends 100 to P3
P3 receives 100 from P2
P3 sends 100 to P0
P0 receives 100 from P3
Ring communication completed

run project = /usr/bin/mpicc problem_3.c -o problem_3
then /usr/bin/mpirun -np 4 ./problem_3
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Need at least 2 processes */
    if (size < 2)
    {
        if (rank == 0)
        {
            printf("Please run with at least 2 processes.\n");
        }

        MPI_Finalize();
        return 0;
    }

    // Ring topology
    //  Previous process
    int previous = (rank - 1 + size) % size;

    // Next process
    int next = (rank + 1) % size;

    // Message
    int value = 0;
    int received_value = 0;
    int sent_value = 0;

    // P0 starts with 100
    if (rank == 0)
    {
        value = 100;
    }

    // Synchronize all processes
    MPI_Barrier(MPI_COMM_WORLD);

    // Start communication timer
    double start_time = MPI_Wtime();

    /*
     * Even processes send first.
     * Odd processes receive first.
     *
     * This prevents deadlock with blocking
     * MPI_Send and MPI_Recv.
     */

    if (rank % 2 == 0)
    {
        sent_value = value;

        // Send to next process
        MPI_Send(
            &sent_value,
            1,
            MPI_INT,
            next,
            100,
            MPI_COMM_WORLD);

        // Receive from previous process
        MPI_Recv(
            &received_value,
            1,
            MPI_INT,
            previous,
            100,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);

        if (received_value != 0)
        {
            value = received_value;
        }
    }
    else
    {
        // Receive from previous process
        MPI_Recv(
            &received_value,
            1,
            MPI_INT,
            previous,
            100,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);

        value = received_value;
        sent_value = value;

        // Send to next process
        MPI_Send(
            &sent_value,
            1,
            MPI_INT,
            next,
            100,
            MPI_COMM_WORLD);
    }

    // End timer
    double end_time = MPI_Wtime();

    double communication_time = end_time - start_time;

    // Print communication in the sample order.
    if (rank == 0)
    {
        printf("P0 sends %d to P1\n", value);
        fflush(stdout);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (int p = 1; p < size; p++)
    {
        if (rank == p)
        {
            printf("P%d receives %d from P%d\n", rank, received_value, previous);
            fflush(stdout);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == p)
        {
            printf("P%d sends %d to P%d\n", rank, value, next);
            fflush(stdout);
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    if (rank == 0)
    {
        printf("P0 receives %d from P%d\n", received_value, previous);
        printf("Ring communication completed\n");
        fflush(stdout);
    }

    // Find maximum communication time
    double max_time;

    MPI_Reduce(
        &communication_time,
        &max_time,
        1,
        MPI_DOUBLE,
        MPI_MAX,
        0,
        MPI_COMM_WORLD);

    MPI_Finalize();

    return 0;
}