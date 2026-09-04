/*
Problem_17: Each MPI process generates data intended for every other process. Implement the
communication using MPI_Alltoall and analyze its communication cost as the number of processes
increases.

Sample Input
Number of processes: 4
P0: 0 1 2 3
P1: 10 11 12 13
P2: 20 21 22 23
P3: 30 31 32 33
Each process sends one value to every process.

Sample Output
After MPI_Alltoall:
P0 received: 0 10 20 30
P1 received: 1 11 21 31
P2 received: 2 12 22 32
P3 received: 3 13 23 33
Students should measure communication time for P = 2, 4, 8, 16 and different message sizes.

Compile and run:
    /usr/bin/mpicc -Wall -Wextra problem_17.c -o problem_17
    /usr/bin/mpirun -np 4 ./problem_17
    /usr/bin/mpirun -np 4 ./problem_17 100
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size, message_size = 1;
    int *send_data, *received_data, *all_received = NULL;
    const int repetitions = 1000;
    double start, elapsed, communication_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc > 1)
        message_size = atoi(argv[1]);
    if (message_size < 1)
        message_size = 1;

    send_data = malloc(size * message_size * sizeof(int));
    received_data = malloc(size * message_size * sizeof(int));

    for (int destination = 0; destination < size; destination++)
        for (int i = 0; i < message_size; i++)
            send_data[destination * message_size + i] =
                rank * 10 + destination * message_size + i;

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    for (int i = 0; i < repetitions; i++)
        MPI_Alltoall(send_data, message_size, MPI_INT,
                     received_data, message_size, MPI_INT, MPI_COMM_WORLD);
    elapsed = MPI_Wtime() - start;
    MPI_Reduce(&elapsed, &communication_time, 1, MPI_DOUBLE, MPI_MAX, 0,
               MPI_COMM_WORLD);

    if (rank == 0)
        all_received = malloc(size * size * message_size * sizeof(int));
    MPI_Gather(received_data, size * message_size, MPI_INT,
               all_received, size * message_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        if (message_size == 1)
        {
            printf("After MPI_Alltoall:\n");
            for (int process = 0; process < size; process++)
            {
                printf("P%d received:", process);
                for (int source = 0; source < size; source++)
                    printf(" %d", all_received[process * size + source]);
                printf("\n");
            }
        }
        printf("Average communication time = %.9f seconds\n",
               communication_time / repetitions);
    }

    free(send_data);
    free(received_data);
    free(all_received);
    MPI_Finalize();
    return 0;
}
