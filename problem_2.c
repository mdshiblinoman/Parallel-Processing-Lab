#include <stdio.h>
#include <mpi.h>

#define ROWS 4
#define COLS 4

int main(int argc, char *argv[])
{
    int rank, size;

    int matrix[ROWS][COLS] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}};

    int row[COLS];
    int row_sum;
    int total_sum = 0;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* We need exactly 4 processes */
    if (size != 4)
    {
        if (rank == 0)
        {
            printf("Please run the program with 4 processes.\n");
        }

        MPI_Finalize();
        return 0;
    }

    /*
     * P0 is the root.
     *
     * P0 keeps the first row.
     * P0 sends the remaining rows to P1, P2 and P3.
     */

    if (rank == 0)
    {
        /* Root gets its own row */
        for (int j = 0; j < COLS; j++)
        {
            row[j] = matrix[0][j];
        }

        /* Send row 1 to P1 */
        MPI_Send(
            matrix[1],
            COLS,
            MPI_INT,
            1,
            100,
            MPI_COMM_WORLD);

        /* Send row 2 to P2 */
        MPI_Send(
            matrix[2],
            COLS,
            MPI_INT,
            2,
            100,
            MPI_COMM_WORLD);

        /* Send row 3 to P3 */
        MPI_Send(
            matrix[3],
            COLS,
            MPI_INT,
            3,
            100,
            MPI_COMM_WORLD);
    }
    else
    {
        /*
         * Worker receives its row from P0
         */
        MPI_Recv(
            row,
            COLS,
            MPI_INT,
            0,
            100,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);
    }

    /*
     * Calculate local row sum
     */
    row_sum = 0;

    for (int j = 0; j < COLS; j++)
    {
        row_sum += row[j];
    }

    /*
     * Print row and local sum
     */
    for (int p = 0; p < size; p++)
    {
        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == p)
        {
            printf("P%d: Row = ", rank);

            for (int j = 0; j < COLS; j++)
            {
                printf("%d ", row[j]);
            }

            printf("\n");
            printf("Sum = %d\n", row_sum);

            fflush(stdout);
        }
    }

    /*
     * Workers send their sums back to root.
     */
    if (rank == 0)
    {
        /*
         * Root adds its own row sum
         */
        total_sum = row_sum;

        int received_sum;

        /* Receive sum from P1 */
        MPI_Recv(
            &received_sum,
            1,
            MPI_INT,
            1,
            200,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);

        total_sum += received_sum;

        /* Receive sum from P2 */
        MPI_Recv(
            &received_sum,
            1,
            MPI_INT,
            2,
            200,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);

        total_sum += received_sum;

        /* Receive sum from P3 */
        MPI_Recv(
            &received_sum,
            1,
            MPI_INT,
            3,
            200,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);

        total_sum += received_sum;

        printf("Total matrix sum = %d\n", total_sum);
    }
    else
    {
        /*
         * Send local sum to root
         */
        MPI_Send(
            &row_sum,
            1,
            MPI_INT,
            0,
            200,
            MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}