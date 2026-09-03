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
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int previous, next;
    int value = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 4)
    {
        if (rank == 0)
            printf("Run the program with exactly 4 processes.\n");

        MPI_Finalize();
        return 0;
    }

    previous = (rank - 1 + size) % size;
    next = (rank + 1) % size;

    if (rank == 0)
    {
        value = 100;
        printf("P0 sends %d to P1\n", value);
        fflush(stdout);

        MPI_Send(&value, 1, MPI_INT, next, 100, MPI_COMM_WORLD);

        MPI_Recv(&value, 1, MPI_INT, previous, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("P0 receives %d from P3\n", value);
        printf("Ring communication completed\n");
        fflush(stdout);
    }
    else
    {
        MPI_Recv(&value, 1, MPI_INT, previous, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("P%d receives %d from P%d\n", rank, value, previous);
        fflush(stdout);

        MPI_Send(&value, 1, MPI_INT, next, 100, MPI_COMM_WORLD);

        printf("P%d sends %d to P%d\n", rank, value, next);
        fflush(stdout);
    }

    MPI_Finalize();
    return 0;
}