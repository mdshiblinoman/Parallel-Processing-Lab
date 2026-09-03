/*
Problem_10: Design a producer–consumer system where one or more MPI processes generate data and
another process receives and processes the data using point-to-point communication.

Sample Input
Number of processes: 3
P0 = Producer
P1 = Consumer
P2 = Consumer
Number of items = 8
Items: 10 20 30 40 50 60 70 80

Sample Output
P0 produced: 10
P1 consumed: 10
P0 produced: 20
P2 consumed: 20
P0 produced: 30
P1 consumed: 30
P0 produced: 40
P2 consumed: 40
...
All 8 items processed successfully.

Compile and run with 3 processes:
    /usr/bin/mpicc problem_10.c -o problem_10
    /usr/bin/mpirun -np 3 ./problem_10
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define DATA_TAG 0
#define ACK_TAG 1
#define STOP_TAG 2

int main(int argc, char *argv[])
{
    int rank, size;
    int number_of_items = 0;
    int *items = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2)
    {
        if (rank == 0)
            printf("Please run with at least 2 processes.\n");
        MPI_Finalize();
        return 0;
    }

    if (rank == 0)
    {
        if (scanf("%d", &number_of_items) != 1 || number_of_items < 0)
            number_of_items = -1;

        if (number_of_items >= 0)
        {
            items = (int *)malloc((size_t)number_of_items * sizeof(int));
            if (number_of_items > 0 && items == NULL)
                number_of_items = -1;
        }

        for (int i = 0; i < number_of_items; i++)
            scanf("%d", &items[i]);
    }

    MPI_Bcast(&number_of_items, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (number_of_items < 0)
    {
        if (rank == 0)
            printf("Invalid input or memory allocation failure.\n");
        free(items);
        MPI_Finalize();
        return 1;
    }

    if (rank == 0)
    {
        for (int i = 0; i < number_of_items; i++)
        {
            int consumer = 1 + (i % (size - 1));
            int ack;

            printf("P0 produced: %d\n", items[i]);
            fflush(stdout);
            MPI_Send(&items[i], 1, MPI_INT, consumer, DATA_TAG, MPI_COMM_WORLD);
            MPI_Recv(&ack, 1, MPI_INT, consumer, ACK_TAG,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("P%d consumed: %d\n", consumer, items[i]);
            fflush(stdout);
        }

        for (int consumer = 1; consumer < size; consumer++)
            MPI_Send(NULL, 0, MPI_INT, consumer, STOP_TAG, MPI_COMM_WORLD);
    }
    else
    {
        int value;
        MPI_Status status;

        while (1)
        {
            MPI_Recv(&value, 1, MPI_INT, 0, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == STOP_TAG)
                break;

            int ack = 1;
            MPI_Send(&ack, 1, MPI_INT, 0, ACK_TAG, MPI_COMM_WORLD);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
        printf("\nAll %d items processed successfully.\n", number_of_items);

    free(items);
    MPI_Finalize();
    return 0;
}
