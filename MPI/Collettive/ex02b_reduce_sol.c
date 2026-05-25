/*
 * Soluzione Esercizio 02B Prodotto scalare 
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNK 1000

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = size * CHUNK;

    /* TODO 1: Allocare A[CHUNK] e B[CHUNK].
     *   A[i] = (double)(rank * CHUNK + i)
     *   B[i] = (double)(N - rank * CHUNK - i - 1) */
    double *A = malloc(CHUNK * sizeof(double));
    double *B = malloc(CHUNK * sizeof(double));
    for (int i = 0; i < CHUNK; i++) {
        A[i] = (double)(rank * CHUNK + i);
        B[i] = (double)(N - rank * CHUNK - i - 1);
    }

    /* TODO 2: Calcolare il contributo locale al prodotto scalare.
     *   local_dot = somma di A[i]*B[i] per i=0..CHUNK-1 */
    double local_dot = 0.0;
    for (int i = 0; i < CHUNK; i++)
        local_dot += A[i] * B[i];

    /* TODO 3: MPI_Allreduce con MPI_SUM per sommare i contributi.*/
    double global_dot = 0.0;
    MPI_Allreduce(&local_dot, &global_dot, 1, MPI_DOUBLE,
                  MPI_SUM, MPI_COMM_WORLD);

    /* TODO 4: Tutti i processi stampano il risultato. */
    printf("rank %d: dot = %.0f\n", rank, global_dot);


    free(A); free(B);
    MPI_Finalize();
    return 0;
}
