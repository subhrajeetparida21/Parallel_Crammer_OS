#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

double determinant(double **mat, int n)
{
    if (n == 1)
        return mat[0][0];

    double det = 0;

    for (int p = 0; p < n; p++)
    {
        double **temp = malloc((n - 1) * sizeof(double *));
        for (int i = 0; i < n - 1; i++)
            temp[i] = malloc((n - 1) * sizeof(double));

        for (int i = 1; i < n; i++)
        {
            int col = 0;
            for (int j = 0; j < n; j++)
            {
                if (j == p) continue;
                temp[i - 1][col++] = mat[i][j];
            }
        }

        double subdet = determinant(temp, n - 1);

        if (p % 2 == 0)
            det += mat[0][p] * subdet;
        else
            det -= mat[0][p] * subdet;

        for (int i = 0; i < n - 1; i++)
            free(temp[i]);
        free(temp);
    }

    return det;
}

double **alloc_matrix(int n)
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

int main()
{
    srand(time(NULL));

    /* Write CSV header once */
    FILE *csv = fopen("human_results.csv", "w");
    fprintf(csv, "n,time_seconds\n");
    fclose(csv);

    for (int n = 3; n <= 10; n++)
    {
        double **A = alloc_matrix(n);
        double *B = malloc(n * sizeof(double));

        for (int i = 0; i < n; i++)
        {
            B[i] = random() % 10 + 1;
            for (int j = 0; j < n; j++)
                A[i][j] = random() % 10 + 1;
        }

        clock_t start = clock();

        double main_det = determinant(A, n);

        for (int var = 0; var < n; var++)
        {
            pid_t pid = fork();

            if (pid == 0)
            {
                double **temp = alloc_matrix(n);

                for (int i = 0; i < n; i++)
                    for (int j = 0; j < n; j++)
                        temp[i][j] = A[i][j];

                for (int i = 0; i < n; i++)
                    temp[i][var] = B[i];

                determinant(temp, n);

                free_matrix(temp, n);
                exit(0);
            }
        }

        for (int i = 0; i < n; i++)
            wait(NULL);

        clock_t end = clock();
        double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

        /* Parent writes result AFTER children finish */
        csv = fopen("human_results.csv", "a");
        fprintf(csv, "%d,%lf\n", n, time_taken);
        fclose(csv);

        free_matrix(A, n);
        free(B);

        printf("Completed n = %d\n", n);
    }

    return 0;
}
