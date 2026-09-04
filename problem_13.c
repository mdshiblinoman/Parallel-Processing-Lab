/*
Problem_13: Distribute matrix rows among processes when the number of rows is not evenly divisible by
the number of processes. Collect the processed rows using MPI_Gatherv.

Sample Input
Number of processes: 3
Matrix:
1 2 3
4 5 6
7 8 9
10 11 12
13 14 15
Five rows are distributed as:
P0 → 2 rows
P1 → 2 rowsP2 → 1 row
Operation: multiply every element by 2.

Sample Output
P0:
2  4  6
8 10 12
P1:
14 16 18
20 22 24
P2:
26 28 30
Final matrix:
2   4  6
8  10 12
14 16 18
20 22 24
26 28 30

Compile and run:
    /usr/bin/mpicc problem_13.c -o problem_13
    /usr/bin/mpirun -np 3 ./problem_13
*/
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    const int rows = 5, columns = 3;
    int rank, size, matrix[15], result[15];
    int counts[16], displacements[16];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size > rows)
    {
        if (rank == 0)
            printf("Use at most 5 processes.\n");
        MPI_Finalize();
        return 1;
    }

    for (int process = 0, offset = 0; process < size; process++)
    {
        int process_rows = rows / size + (process < rows % size);
        counts[process] = process_rows * columns;
        displacements[process] = offset;
        offset += counts[process];
    }

    if (rank == 0)
        for (int i = 0; i < rows * columns; i++)
            scanf("%d", &matrix[i]);

    int local[15];
    MPI_Scatterv(matrix, counts, displacements, MPI_INT,
                 local, counts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < counts[rank]; i++)
        local[i] *= 2;

    MPI_Gatherv(local, counts[rank], MPI_INT, result, counts, displacements,
                MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        for (int process = 0; process < size; process++)
        {
            printf("P%d:\n", process);
            for (int i = 0; i < counts[process] / columns; i++)
            {
                for (int j = 0; j < columns; j++)
                    printf("%d%c", result[displacements[process] + i * columns + j],
                           j == columns - 1 ? '\n' : ' ');
            }
        }

        printf("Final matrix:\n");
        for (int i = 0; i < rows * columns; i++)
            printf("%d%c", result[i], (i + 1) % columns == 0 ? '\n' : ' ');
    }

    MPI_Finalize();
    return 0;
}
