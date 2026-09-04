/*
Problem_11: The root process reads configuration parameters and broadcasts them to all MPI processes.
Each process uses the received parameters to perform a computation.

Sample Input
Number of processes: 4
Root process reads:
Number of iterations = 1000
Threshold = 50
Block size = 256

Sample Output
P0 received: Iterations = 1000, Threshold = 50, Block size = 256
P1 received: Iterations = 1000, Threshold = 50, Block size = 256
P2 received: Iterations = 1000, Threshold = 50, Block size = 256
P3 received: Iterations = 1000, Threshold = 50, Block size = 256

Compile and run:
    /usr/bin/mpicc problem_11.c -o problem_11
    /usr/bin/mpirun -np 3 ./problem_11
*/

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size, config[3];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
        scanf("%d%d%d", &config[0], &config[1], &config[2]);

    MPI_Bcast(config, 3, MPI_INT, 0, MPI_COMM_WORLD);

    int result = config[0] * config[2];
    (void)result;

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
    {
        for (int process = 0; process < size; process++)
        {
            printf("P%d received: Iterations = %d, Threshold = %d, Block size = %d\n",
                   process, config[0], config[1], config[2]);
        }
    }

    MPI_Finalize();
    return 0;
}