/*
Problem_4: Implement a ping-pong program between two MPI processes using MPI_Send and MPI_Recv.
Test message sizes from a few bytes to several megabytes and analyze communication latency and
bandwidth.

Sample Input
Number of processes: 2
Number of iterations = 5
Message size = 1024 bytes

Sample Output
Message size: 1024 bytes
Iterations: 5
Total round trips: 5
Average round-trip : 0.000018 seconds
Average one-way: 0.000009 seconds
Repeat the experiment for 64 bytes, 256 bytes, 1 KB, 4 KB, 16 KB, 64 KB, and 1 MB.


run project = /usr/bin/mpicc problem_4.c -o problem_4
then /usr/bin/mpirun -np 4 ./problem_4
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    const int sizes[] = {64, 256, 1024, 4096, 16384, 65536, 1048576};
    const int iterations = 5;
    int rank, processes;

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

    for (int s = 0; s < 7; s++)
    {
        int bytes = sizes[s];
        char *buffer = malloc(bytes);
        double total = 0.0;

        if (!buffer)
            MPI_Abort(MPI_COMM_WORLD, 1);

        MPI_Barrier(MPI_COMM_WORLD);
        for (int i = 0; i < iterations; i++)
        {
            if (rank == 0)
            {
                double start = MPI_Wtime();
                MPI_Send(buffer, bytes, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(buffer, bytes, MPI_BYTE, 1, 1, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);
                total += MPI_Wtime() - start;
            }
            else
            {
                MPI_Recv(buffer, bytes, MPI_BYTE, 0, 0, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);
                MPI_Send(buffer, bytes, MPI_BYTE, 0, 1, MPI_COMM_WORLD);
            }
        }

        if (rank == 0)
        {
            double round_trip = total / iterations;
            printf("Message size: %d bytes | Iterations: %d | ", bytes, iterations);
            printf("Average round-trip: %.9f s | One-way: %.9f s | ",
                   round_trip, round_trip / 2);
            printf("Bandwidth: %.3f MB/s\n",
                   (2.0 * bytes / round_trip) / (1024 * 1024));
        }
        free(buffer);
    }

    MPI_Finalize();
    return 0;
}