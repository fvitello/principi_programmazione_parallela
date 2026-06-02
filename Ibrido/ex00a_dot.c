/*
 * COSA IMPARIAMO
 * ──────────────
 * Il pattern ibrido base: MPI divide il lavoro tra i processi,
 * OpenMP parallelizza il calcolo dentro ogni processo.
 *
 * I due livelli non si mescolano mai:
 *   MPI:    comunica tra processi (tra nodi)
 *   OpenMP: parallelizza il calcolo locale (dentro il nodo)
 *
 * SCENARIO
 * ────────
 * Prodotto scalare parallelo di due vettori di N double.
 *
 *   MPI_Scatter → ogni processo riceve chunk di A e B
 *   OpenMP reduction → calcola contributo locale
 *   MPI_Allreduce → somma tutti i contributi
 *
 * COMPILARE:
 *   mpicc -Wall -O2 -fopenmp -o dot_a ex00a_dot.c
 * ESEGUIRE:
 *   OMP_NUM_THREADS=4 mpirun -np 2 ./dot_a
 */

#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N (1 << 20)   /* elementi totali per vettore = shift di 1 a sinistra per venti posizioni = 1048576 */

int main(int argc, char **argv)
{
    int rank, size;

    /* ── MPI_Init_thread ─────────────────────────────────────────
     * In un programma ibrido dobbiamo dichiarare il livello di
     * thread-safety richiesto a MPI.
     *
     * MPI_THREAD_FUNNELED: solo il thread principale chiama MPI.
     * È il livello più comune nel pattern "OpenMP dentro, MPI fuori".
     *
     * provided: livello effettivamente supportato dall'implementazione.
     * Se provided < required, dobbiamo adattare il nostro codice. */
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if (provided < MPI_THREAD_FUNNELED) {
        fprintf(stderr, "MPI non supporta MPI_THREAD_FUNNELED\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0) fprintf(stderr, "N non divisibile per %d\n", size);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int chunk = N / size;
    int nthreads = omp_get_max_threads();

    if (rank == 0)
        printf("N=%d  processi MPI=%d  thread OpenMP=%d  chunk=%d\n",
               N, size, nthreads, chunk);

    /* Root inizializza i vettori completi */
    double *A = NULL, *B = NULL;
    if (rank == 0) {
        A = malloc(N * sizeof(double));
        B = malloc(N * sizeof(double));
        for (int i = 0; i < N; i++) {
            A[i] = (double)i / N;
            B[i] = (double)(N - i) / N;
        }
    }

    /* Distribuire i chunk con MPI_Scatter */
    double *lA = malloc(chunk * sizeof(double));
    double *lB = malloc(chunk * sizeof(double));
    MPI_Scatter(A, chunk, MPI_DOUBLE, lA, chunk, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, chunk, MPI_DOUBLE, lB, chunk, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /* ── OpenMP reduction sul chunk locale ───────────────────────
     * Ogni thread calcola una parte del prodotto scalare locale.
     * La reduction(+:local_dot) somma i contributi dei thread
     * in modo thread-safe — nessuna race condition. */
    double local_dot = 0.0;

    #pragma omp parallel for reduction(+:local_dot) schedule(static)
    for (int i = 0; i < chunk; i++)
        local_dot += lA[i] * lB[i];

    /* ── MPI_Allreduce per combinare i contributi ────────────────
     * Somma i contributi locali di tutti i processi.
     * Chiamata FUORI dalla regione parallela — solo il thread
     * principale chiama MPI (THREAD_FUNNELED). */
    double global_dot = 0.0;
    MPI_Allreduce(&local_dot, &global_dot, 1, MPI_DOUBLE,
                  MPI_SUM, MPI_COMM_WORLD);

    if (rank == 0)
        printf("Prodotto scalare = %.6f\n", global_dot);

    free(lA); free(lB);
    if (rank == 0) { free(A); free(B); }
    MPI_Finalize();
    return 0;
}
