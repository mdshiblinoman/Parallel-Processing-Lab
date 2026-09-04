/*
Problem_15: Implement the trapezoidal rule for numerical integration. Each MPI process calculates a
portion of the integral, and MPI_Reduce combines the partial results.

Sample Input
Number of processes: 4
Function: f(x) = x²
Lower limit = 0
Upper limit = 1
Number of intervals = 1000
Use the trapezoidal rule.

Sample Output
Numerical integral ≈ 0.333333
Exact integral = 0.333333
Absolute error ≈ 0.000000
Students should report partial integrals and repeat the experiment with different numbers of intervals.

Compile and run:
    /usr/bin/mpicc problem_15.c -lm -o problem_15
    printf "0 1 1000\n" | /usr/bin/mpirun -np 4 ./problem_15

Input format:
    lower_limit upper_limit number_of_intervals
*/

#include <math.h>
#include <stdio.h>
#include <mpi.h>

double function(double x)
{
    return x * x;
}

int main(int argc, char *argv[])
{
    int rank, size, intervals, parameters[1];
    double limits[2], partial, integral, parts[16];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        scanf("%lf%lf%d", &limits[0], &limits[1], &intervals);
        parameters[0] = intervals;
    }

    MPI_Bcast(limits, 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(parameters, 1, MPI_INT, 0, MPI_COMM_WORLD);
    intervals = parameters[0];

    double width = (limits[1] - limits[0]) / intervals;
    int start = rank * intervals / size;
    int end = (rank + 1) * intervals / size;
    partial = 0.0;

    for (int i = start; i < end; i++)
    {
        double x1 = limits[0] + i * width;
        double x2 = x1 + width;
        partial += (function(x1) + function(x2)) * width / 2.0;
    }

    MPI_Gather(&partial, 1, MPI_DOUBLE, parts, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Reduce(&partial, &integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        for (int process = 0; process < size; process++)
            printf("P%d partial integral = %.6f\n", process, parts[process]);

        double exact = (limits[1] * limits[1] * limits[1] -
                        limits[0] * limits[0] * limits[0]) /
                       3.0;
        printf("Numerical integral ~= %.6f\n", integral);
        printf("Exact integral = %.6f\n", exact);
        printf("Absolute error ~= %.6f\n", fabs(integral - exact));
    }

    MPI_Finalize();
    return 0;
}
