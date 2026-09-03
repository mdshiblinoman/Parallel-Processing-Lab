/*
Problem_7: Implement a distributed vector computation using MPI_Isend and MPI_Irecv. Compare its
execution time with the equivalent blocking MPI_Send/MPI_Recv implementation.

Sample Input
Number of processes: 2
Array size = 8
P0: 10 20 30 40 50 60 70 80
P1: 1 2 3 4 5 6 7 8
Each process exchanges its array using MPI_Isend and MPI_Irecv.

Sample Output
P0 received: 1 2 3 4 5 6 7 8
P1 received: 10 20 30 40 50 60 70 80
Nonblocking communication completed.
Sample performance:
Blocking communication time = 0.000021 s
Nonblocking communication time = 0.000015 s

run project = /usr/bin/mpicc problem_7.c -o problem_7
then /usr/bin/mpirun -np 4 ./problem_7
*/

#include <stdio.h>
#include <mpi.h>

#define N 8

int main(int argc, char *argv[])
{
    int rank, size;

    int send_array[N];
    int recv_array[N];

    MPI_Request requests[2];
    MPI_Status statuses[2];

    double start, end;
    double blocking_time;
    double nonblocking_time;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Exactly 2 processes are required */
    if (size != 2)
    {
        if (rank == 0)
        {
            printf("Error: Please run with exactly 2 processes.\n");
        }

        MPI_Finalize();
        return 0;
    }

    /* Initialize arrays */
    if (rank == 0)
    {
        int temp[N] = {10, 20, 30, 40, 50, 60, 70, 80};

        for (int i = 0; i < N; i++)
            send_array[i] = temp[i];
    }
    else
    {
        int temp[N] = {1, 2, 3, 4, 5, 6, 7, 8};

        for (int i = 0; i < N; i++)
            send_array[i] = temp[i];
    }

    // PART 1: BLOCKING COMMUNICATION
    // MPI_Send + MPI_Recv

    MPI_Barrier(MPI_COMM_WORLD);

    start = MPI_Wtime();

    if (rank == 0)
    {
        /* P0 sends its array to P1 */
        MPI_Send(send_array, N, MPI_INT,
                 1, 100, MPI_COMM_WORLD);

        /* P0 receives P1's array */
        MPI_Recv(recv_array, N, MPI_INT,
                 1, 200, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }
    else
    {
        /* P1 receives P0's array */
        MPI_Recv(recv_array, N, MPI_INT,
                 0, 100, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        /* P1 sends its array to P0 */
        MPI_Send(send_array, N, MPI_INT,
                 0, 200, MPI_COMM_WORLD);
    }

    end = MPI_Wtime();

    blocking_time = end - start;

    // PART 2: NON-BLOCKING COMMUNICATION
    // MPI_Isend + MPI_Irecv

    MPI_Barrier(MPI_COMM_WORLD);

    start = MPI_Wtime();

    if (rank == 0)
    {
        /*
         * P0 sends to P1
         * P0 receives from P1
         */

        MPI_Isend(send_array, N, MPI_INT,
                  1, 300, MPI_COMM_WORLD,
                  &requests[0]);

        MPI_Irecv(recv_array, N, MPI_INT,
                  1, 400, MPI_COMM_WORLD,
                  &requests[1]);
    }
    else
    {
        /*
         * P1 sends to P0
         * P1 receives from P0
         */

        MPI_Isend(send_array, N, MPI_INT,
                  0, 400, MPI_COMM_WORLD,
                  &requests[0]);

        MPI_Irecv(recv_array, N, MPI_INT,
                  0, 300, MPI_COMM_WORLD,
                  &requests[1]);
    }

    /*
     * Wait until both Isend and Irecv
     * operations are completed.
     */
    MPI_Waitall(2, requests, statuses);

    end = MPI_Wtime();

    nonblocking_time = end - start;

    // PRINT RECEIVED ARRAYS

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("\nP0 received: ");

        for (int i = 0; i < N; i++)
            printf("%d ", recv_array[i]);

        printf("\n");
    }
    else
    {
        printf("P1 received: ");

        for (int i = 0; i < N; i++)
            printf("%d ", recv_array[i]);

        printf("\n");
    }

    // COMPARE EXECUTION TIME

    double max_blocking_time;
    double max_nonblocking_time;

    /*
     * Use maximum time among processes as the
     * overall communication time.
     */
    MPI_Reduce(&blocking_time,
               &max_blocking_time,
               1,
               MPI_DOUBLE,
               MPI_MAX,
               0,
               MPI_COMM_WORLD);

    MPI_Reduce(&nonblocking_time,
               &max_nonblocking_time,
               1,
               MPI_DOUBLE,
               MPI_MAX,
               0,
               MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("\nNonblocking communication completed.\n");

        printf("\nPerformance Comparison:\n");
        printf("----------------------------------\n");

        printf("Blocking communication time    = %.9f s\n",
               max_blocking_time);

        printf("Nonblocking communication time = %.9f s\n",
               max_nonblocking_time);
    }

    MPI_Finalize();

    return 0;
}