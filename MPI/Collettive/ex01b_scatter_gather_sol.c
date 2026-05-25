/*
 * Soluzione Esercizio 01B Scatter+Gather
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 16

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0)
    {
        if (rank == 0)
            fprintf(stderr, "N non divisibile per %d\n", size);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int chunk = N / size;
    double *array = NULL;
    double *local = malloc(chunk * sizeof(double));
    double *maxes = NULL;

    /* TODO 1: Root alloca array[N] e lo inizializza con
     *   srand(42) e valori (double)rand()/RAND_MAX
     *   Root alloca anche maxes[size] per raccogliere i massimi */

    if (rank == 0)
    {
        array = malloc(N * sizeof(double));
        srand(42);
        for (int i = 0; i < N; i++)
            array[i] = (double)rand() / RAND_MAX;
        maxes = malloc(size * sizeof(double));
    }

    /* TODO 2: MPI_Scatter per distribuire i chunk. */
    MPI_Scatter(array, chunk, MPI_DOUBLE,
                local, chunk, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    /* TODO 3: Ogni processo calcola il massimo del proprio chunk.
     *   double local_max = local[0];
     *   for (int i = 1; i < chunk; i++)
     *       if (local[i] > local_max) local_max = local[i];
     *   printf("rank %d: max locale = %.4f\n", rank, local_max) */
    double local_max = local[0];
    for (int i = 1; i < chunk; i++)
        if (local[i] > local_max)
            local_max = local[i];
    printf("rank %d: max locale = %.4f\n", rank, local_max);
    /* TODO 4: MPI_Gather per raccogliere i massimi parziali. */
    MPI_Gather(&local_max, 1, MPI_DOUBLE,
               maxes, 1, MPI_DOUBLE,
               0, MPI_COMM_WORLD);
    /* TODO 5: Root trova il massimo globale tra maxes[] e stampa */
    if (rank == 0)
    {
        double global_max = maxes[0];
        for (int i = 1; i < size; i++)
            if (maxes[i] > global_max)
                global_max = maxes[i];
        printf("Root: massimo globale = %.4f\n", global_max);
        free(array);
        free(maxes);
    }

    free(local);
    MPI_Finalize();
    return 0;
}
