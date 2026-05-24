/*
 * Esercizio 02A  ·  MPI_Reduce + Allreduce
 * COSA IMPARIAMO
 * ──────────────
 * MPI_Reduce applica un operatore (somma, massimo, ecc.) a
 * valori distribuiti su tutti i processi e raccoglie il
 * risultato sul root.
 *
 * MPI_Allreduce fa lo stesso ma distribuisce il risultato
 * a TUTTI i processi — non solo al root.
 *
 * SCENARIO
 * ────────
 * Ogni processo calcola la somma del proprio chunk locale.
 * MPI_Reduce raccoglie le somme parziali e le somma sul root.
 * MPI_Allreduce fa lo stesso ma tutti i processi ricevono
 * il risultato — utile quando tutti hanno bisogno del totale
 * per decidere cosa fare dopo (es. controllo di convergenza).
 *
 * COMPILARE:  mpicc -Wall -O2 -o reduce_a ex02a_reduce.c
 * ESEGUIRE:   mpirun -np 4 ./reduce_a
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000   /* elementi per processo */

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Ogni processo inizializza il proprio chunk locale.
     * rank k ha elementi [k*N, k*N+1, ..., k*N+N-1] */
    double *local = malloc(N * sizeof(double));
    for (int i = 0; i < N; i++)
        local[i] = (double)(rank * N + i);

    /* Somma locale */
    double local_sum = 0.0;
    for (int i = 0; i < N; i++) local_sum += local[i];
    printf("rank %d: somma locale = %.0f\n", rank, local_sum);

    /* ── MPI_Reduce ──────────────────────────────────────────────
     * Firma: MPI_Reduce(sendbuf, recvbuf, count, type,
     *                   op, root, comm)
     *
     * sendbuf: valore locale da contribuire alla riduzione
     * recvbuf: dove scrivere il risultato (solo sul root)
     *          sugli altri processi viene ignorato — passate NULL
     *          o qualsiasi puntatore, non verrà scritto
     * count:   numero di elementi
     * type:    tipo MPI
     * op:      operatore: MPI_SUM, MPI_MAX, MPI_MIN,
     *                     MPI_PROD, MPI_LAND, MPI_LOR, ...
     * root:    processo che riceve il risultato
     *
     * Il risultato è: recvbuf = op(sendbuf_0, sendbuf_1, ...) */
    double total_sum = 0.0;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE,
               MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        /* L'array complessivo è la sequenza 0, 1, 2, ..., size*N-1. 
        La somma di tutti gli elementi da 0 a M-1 vale M*(M-1)/2, 
        quindi con M = size*N:*/
        double expected = (double)(size*N) * (size*N - 1) / 2.0;
        printf("Root: somma totale = %.0f  (atteso %.0f)\n",
               total_sum, expected);
    }

    /* ── MPI_Allreduce ───────────────────────────────────────────
     * Stessa firma di Reduce ma senza il parametro root.
     * Il risultato viene distribuito a TUTTI i processi.
     * Utile per: controllo di convergenza, normalizzazione,
     *            decisioni collettive basate su dati globali. */
    double global_max = 0.0;
    MPI_Allreduce(&local_sum, &global_max, 1, MPI_DOUBLE,
                  MPI_MAX, MPI_COMM_WORLD);

    /* Ora TUTTI i processi hanno il massimo delle somme parziali */
    printf("rank %d: massimo delle somme parziali = %.0f\n",
           rank, global_max);

    free(local);
    MPI_Finalize();
    return 0;
}
