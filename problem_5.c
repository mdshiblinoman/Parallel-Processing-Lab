/*
Problem_5: Distribute matrix rows from the root process to other processes using MPI_Send. Each process
performs its portion of matrix multiplication and sends the result back to the root.

Sample Input
Number of processes: 2
Matrix A:
    1 2
    3 4
Matrix B:
    5 6
    7 8

Sample Output
Matrix C = A × B
    19 22
    43 50
C[0][0] = 1×5 + 2×7 = 19
C[0][1] = 1×6 + 2×8 = 22
C[1][0] = 3×5 + 4×7 = 43
C[1][1] = 3×6 + 4×8 = 50


run project = /usr/bin/mpicc problem_5.c -o problem_5
then /usr/bin/mpirun -np 4 ./problem_5
*/
#include <stdio.h>
#include <mpi.h>

#define N 2

int main(int argc, char **argv)
{
    int rank, processes;
    int A[N][N] = {{1, 2}, {3, 4}};
    int B[N][N] = {{5, 6}, {7, 8}};
    int C[N][N] = {0};
    int row[N], result[N];

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

    if (rank == 0)
    {
        row[0] = A[0][0];
        row[1] = A[0][1];
        MPI_Send(B, N * N, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(A[1], N, MPI_INT, 1, 1, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Recv(B, N * N, MPI_INT, 0, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        MPI_Recv(row, N, MPI_INT, 0, 1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }

    for (int column = 0; column < N; column++)
    {
        result[column] = 0;
        for (int k = 0; k < N; k++)
            result[column] += row[k] * B[k][column];
    }

    if (rank == 0)
    {
        C[0][0] = result[0];
        C[0][1] = result[1];
        MPI_Recv(C[1], N, MPI_INT, 1, 2, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        printf("Matrix C = A x B\n");
        printf("%d %d\n%d %d\n", C[0][0], C[0][1], C[1][0], C[1][1]);
        printf("C[0][0] = 1*5 + 2*7 = %d\n", C[0][0]);
        printf("C[0][1] = 1*6 + 2*8 = %d\n", C[0][1]);
        printf("C[1][0] = 3*5 + 4*7 = %d\n", C[1][0]);
        printf("C[1][1] = 3*6 + 4*8 = %d\n", C[1][1]);
    }
    else
        MPI_Send(result, N, MPI_INT, 0, 2, MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}