/*
 * Esercizio 02A  ·  Scatter non-bloccante
 *
 * COSA IMPARIAMO
 * ──────────────
 * MPI_Waitall aspetta che un array di MPI_Request completino
 * tutte insieme. Permette di avviare N comunicazioni
 * non-bloccanti, fare calcolo nel mezzo, poi aspettare tutto.
 *
 * SCENARIO
 * ────────
 * Il processo 0 distribuisce i chunk di un array a tutti
 * gli altri processi usando Isend non-bloccanti. Mentre
 * le comunicazioni avvengono in background, il processo 0
 * calcola il proprio chunk. Poi MPI_Waitall assicura che
 * tutte le Send siano completate.
 *
 * Schema:
 *   P0: Isend chunk1→P1, Isend chunk2→P2, ...,
 *       calcola chunk0,
 *       Waitall
 *   P1..Pn: Recv chunk
 *
 * COMPILARE:  mpicc -Wall -O2 -o scatter_a ex02a_waitall.c
 * ESEGUIRE:   mpirun -np 4 ./scatter_a
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N (1 << 22) /* 4M double = 32 MB totale */

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
    double *local = malloc(chunk * sizeof(double));

    /* ════════════════════ PROCESSO 0 ════════════════════════════ */
    if (rank == 0)
    {
        double *array = malloc(N * sizeof(double));
        for (int i = 0; i < N; i++)
            array[i] = (double)i;

        double t0 = MPI_Wtime();

        /* ── Allocare array di request ───────────────────────────
         * Un request per ogni Isend verso gli altri processi.
         * size-1 richieste (non contiamo il processo 0 stesso). */
        MPI_Request *reqs = malloc((size - 1) * sizeof(MPI_Request));

        /* ── Avviare tutte le Isend ──────────────────────────────
         * Per ogni worker: inviare il suo chunk in modo
         * non-bloccante. Tutte le comunicazioni partono quasi
         * contemporaneamente — non aspettiamo nessuna. */
        for (int p = 1; p < size; p++)
        {
            MPI_Isend(&array[p * chunk], chunk, MPI_DOUBLE,
                      p, 0, MPI_COMM_WORLD, &reqs[p - 1]);
        }

        /* ── Calcolo sovrapposto: chunk 0 è del processo 0 ───────
         * Mentre le Isend avvengono in background, copiamo
         * il nostro chunk localmente e calcoliamo la sua somma.
         * NOTA: array[] non è toccato qui — è in uso da Isend. */
        memcpy(local, array, chunk * sizeof(double));
        double local_sum = 0.0;
        for (int i = 0; i < chunk; i++)
            local_sum += local[i];

        /* ── MPI_Waitall: aspettare tutte le Isend ───────────────
         * Firma: MPI_Waitall(count, reqs[], statuses[])
         * MPI_STATUSES_IGNORE: non ci interessa l'array di status.
         * Dopo il Waitall tutti i buffer (i chunk di array[])
         * sono stati inviati e possono essere riusati. */
        MPI_Waitall(size - 1, reqs, MPI_STATUSES_IGNORE);

        double elapsed = (MPI_Wtime() - t0) * 1000.0;
        printf("P0: scatter completato in %.3f ms\n", elapsed);
        printf("P0: somma chunk 0 = %.0f\n", local_sum);

        free(array);
        free(reqs);
    }
    /* ════════════════════ ALTRI PROCESSI ════════════════════════ */
    else
    {
        /* Ricevere il proprio chunk con Recv bloccante semplice */
        MPI_Recv(local, chunk, MPI_DOUBLE, 0, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* Calcolare la somma del chunk ricevuto */
        double local_sum = 0.0;
        for (int i = 0; i < chunk; i++)
            local_sum += local[i];

        printf("P%d: ricevuto chunk, somma = %.0f\n", rank, local_sum);
    }

    free(local);
    MPI_Finalize();
    return 0;
}
