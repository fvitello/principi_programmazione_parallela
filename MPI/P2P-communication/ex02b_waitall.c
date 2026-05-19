/*
 * Esercizio 02B  ·  Gather non-bloccante 
 * PUNTO DI PARTENZA: ex02a_waitall.c
 * ────────────────────────────────────
 * In 02A il processo 0 usava Isend+Waitall per distribuire
 * i chunk (scatter non-bloccante).
 *
 * VARIANTE: gather non-bloccante
 * ───────────────────────────────
 * Ora ogni processo ha già il proprio chunk (inizializzato
 * localmente, non ricevuto). Il processo 0 raccoglie tutti
 * i chunk con MPI_Irecv non-bloccanti + MPI_Waitall.
 *
 * Schema:
 *   Tutti: local[i] = rank * chunk + i   (inizializzazione)
 *
 *   P1..Pn: MPI_Send del proprio chunk a P0
 *
 *   P0: MPI_Irecv da P1, MPI_Irecv da P2, ..., MPI_Irecv da Pn
 *       calcola somma del proprio chunk (calcolo sovrapposto)
 *       MPI_Waitall
 *       stampa la somma totale di tutti i chunk
 *
 * HINT
 * ────
 * È lo schema speculare di 02A: invece di Isend+Waitall
 * usate Irecv+Waitall sul processo 0.
 * Gli altri processi usano Send bloccante semplice.
 *
 * COMPILARE:  mpicc -Wall -O2 -o gather_b ex02b_waitall.c
 * ESEGUIRE:   mpirun -np 4 ./gather_b
 *
 * OUTPUT ATTESO (N=2^22, 4 processi):
 *   P0: gather completato in X ms, somma totale = Y
 *   (Y = somma di tutti gli interi da 0 a N-1)
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N (1 << 22)

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0) fprintf(stderr, "N non divisibile per %d\n", size);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int chunk = N / size;
    double *local = malloc(chunk * sizeof(double));

    /* Tutti i processi inizializzano il proprio chunk */
    for (int i = 0; i < chunk; i++)
        local[i] = (double)(rank * chunk + i);

    /* ════════════════════ PROCESSO 0 ════════════════════════════ */
    if (rank == 0)
    {
        double *array = malloc(N * sizeof(double));
        double t0 = MPI_Wtime();

        /* TODO 1: Allocare array di MPI_Request di dimensione size-1 */


        /* TODO 2: Avviare Irecv da ogni processo p=1..size-1.
         *   MPI_Irecv(&array[p*chunk], chunk, MPI_DOUBLE,
         *             p, 0, MPI_COMM_WORLD, &reqs[p-1]);
         *
         * Nota: array[p*chunk] è dove vogliamo il chunk del processo p.
         */


        /* TODO 3: Calcolo sovrapposto — copiare e sommare il chunk
         * del processo 0 (non dipende dalle Irecv in corso).
         *   for i: array[i] = local[i]
         *   calcolare local_sum0 = somma di local[]
         */


        /* TODO 4: MPI_Waitall per aspettare tutte le Irecv.
         *   MPI_Waitall(size-1, reqs, MPI_STATUSES_IGNORE);
         */


        /* TODO 5: Calcolare la somma totale di array[] e stamparla.
         * Dovrebbe essere N*(N-1)/2.
         */

        free(array);
    }
    /* ════════════════════ ALTRI PROCESSI ════════════════════════ */
    else
    {
        /* TODO 6: Inviare local[] al processo 0 con MPI_Send. */

    }

    free(local);
    MPI_Finalize();
    return 0;
}
