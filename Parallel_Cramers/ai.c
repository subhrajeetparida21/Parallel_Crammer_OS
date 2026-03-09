/*
===============================================================================
Parallel Solution of Linear Equations using Cramer's Rule
===============================================================================

This program solves a system of 'n' linear equations using Cramer's Rule and
parallel processing with fork().

How the program works:
1. A random system of equations is generated where:
      A → coefficient matrix (n x n)
      B → constant vector
2. The determinant of the coefficient matrix det(A) is calculated recursively
   using Laplace expansion.
3. For each variable Xi, a child process is created using fork().
4. Each child process creates a modified matrix Ai by replacing the i-th
   column of A with vector B, then computes det(Ai).
5. The parent process waits for all child processes to finish using waitpid().
6. Execution time for solving the system is measured.
7. The experiment runs for multiple values of n (3 ≤ n ≤ 10).
8. The runtime results are saved in a CSV file "ai_results.csv" so that a
   Time vs n graph can be plotted.

Note:
Recursive determinant computation grows extremely fast (factorial complexity),
so large matrix sizes become computationally expensive even with parallelism.
===============================================================================
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

double **allocate_matrix(int n)
{
    double **m = malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++)
        m[i] = malloc(n * sizeof(double));
    return m;
}

void free_matrix(double **m, int n)
{
    for (int i = 0; i < n; i++)
        free(m[i]);
    free(m);
}

void generate_system(double **A, double *B, int n)
{
    for (int i = 0; i < n; i++)
    {
        B[i] = random() % 20 + 1;

        for (int j = 0; j < n; j++)
            A[i][j] = random() % 20 + 1;
    }
}

double determinant(double **mat, int n)
{
    if (n == 1)
        return mat[0][0];

    double det = 0;

    for (int p = 0; p < n; p++)
    {
        double **sub = allocate_matrix(n - 1);

        for (int i = 1; i < n; i++)
        {
            int col = 0;
            for (int j = 0; j < n; j++)
            {
                if (j == p) continue;
                sub[i - 1][col++] = mat[i][j];
            }
        }

        double sub_det = determinant(sub, n - 1);

        double sign = (p % 2 == 0) ? 1 : -1;
        det += sign * mat[0][p] * sub_det;

        free_matrix(sub, n - 1);
    }

    return det;
}

int main()
{
    srand(time(NULL));

    FILE *csv = fopen("ai_results.csv", "w");
    fprintf(csv, "n,time_seconds\n");
    fclose(csv);

    for (int n = 3; n <= 10; n++)
    {
        double **A = allocate_matrix(n);
        double *B = malloc(n * sizeof(double));

        generate_system(A, B, n);

        clock_t start = clock();

        double main_det = determinant(A, n);

        fflush(stdout); // prevent buffer duplication after fork

        pid_t pids[n];

        for (int var = 0; var < n; var++)
        {
            pids[var] = fork();

            if (pids[var] == 0)
            {
                double **temp = allocate_matrix(n);

                for (int i = 0; i < n; i++)
                    for (int j = 0; j < n; j++)
                        temp[i][j] = A[i][j];

                for (int i = 0; i < n; i++)
                    temp[i][var] = B[i];

                determinant(temp, n);

                free_matrix(temp, n);
                exit(0);   // absolutely ensure child stops here
            }
        }

        /* Parent waits for all children */
        for (int i = 0; i < n; i++)
            waitpid(pids[i], NULL, 0);

        clock_t end = clock();
        double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

        /* Only parent writes */
        csv = fopen("ai_results.csv", "a");
        fprintf(csv, "%d,%lf\n", n, time_taken);
        fclose(csv);

        printf("Finished computation for n = %d\n", n);

        free_matrix(A, n);
        free(B);
    }

    return 0;
}
