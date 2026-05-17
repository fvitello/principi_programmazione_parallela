/*
 * ╔══════════════════════════════════════════════════════════╗
 * ║  Lab MPI  ·  Esercizio 01B  ·  Print ordinate           ║
 * ╚══════════════════════════════════════════════════════════╝
 *
 * PUNTO DI PARTENZA
 * ─────────────────
 * Avete appena visto il ping-pong (01A): Send e Recv bloccanti.
 * Ora usiamo gli stessi strumenti per risolvere il problema
 * dell'output disordinato che avete visto nell'esercizio 00A.
 *
 * PROBLEMA
 * ────────
 * Con mpirun -np 4 ./hello ogni printf arriva al terminale
 * nell'ordine in cui il SO schedula i processi — casuale ad
 * ogni esecuzione. Non è un bug: è la natura dei processi
 * indipendenti.
 *
 * SOLUZIONE
 * ─────────
 * Il processo 0 decide l'ordine: chiede i messaggi uno per uno,
 * prima a rank 1, poi a rank 2, poi a rank 3, e li stampa lui.
 *
 *   Worker k:                    Processo 0:
 *   ─────────                    ───────────
 *   MPI_Send(messaggio) ──────►  for k = 1, 2, ..., size-1:
 *                                    MPI_Recv da rank k
 *                                    printf(messaggio)
 *
 * NOTA IMPORTANTE sull'ordine Recv:
 *   Processo 0 usa src=k (non MPI_ANY_SOURCE) — chiede
 *   esplicitamente a rank 1, poi a rank 2, ecc.
 *   Questo è ciò che garantisce l'ordine nell'output.
 *
 * COMPITI
 * ───────
 *  1. Completare i TODO.
 *  2. Compilare:  mpicc -Wall -O2 -o print_b ex01b_print.c
 *  3. Eseguire:   mpirun -np 6 ./print_b
 *  4. Rieseguire 5 volte: l'output è sempre ordinato? Perché?
 *  5. Cosa succede se nel TODO 2 usate MPI_ANY_SOURCE
 *     invece di src=k? Provate e osservate.
 *
 * OUTPUT ATTESO (sempre in questo ordine):
 *   [rank 0] sono il root, ci sono 6 processi
 *   [rank 1] sono un worker
 *   [rank 2] sono un worker
 *   [rank 3] sono un worker
 *   [rank 4] sono un worker
 *   [rank 5] sono un worker
 */

#include <mpi.h>
#include <stdio.h>
#include <string.h>

#define MAXLEN 64   /* lunghezza massima del messaggio */

int main(int argc, char **argv)
{
    int  rank, size;
    char msg[MAXLEN];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        /* Il processo 0 stampa subito il proprio messaggio */
        printf("[rank 0] sono il root, ci sono %d processi\n", size);

        /* TODO 1: ricevere il messaggio da ogni worker in ordine.
         *
         * for k = 1, 2, ..., size-1:
         *   MPI_Recv(msg, MAXLEN, MPI_CHAR,
         *            src=k, tag=0,
         *            MPI_COMM_WORLD, MPI_STATUS_IGNORE)
         *   printf("%s\n", msg)
         *
         * src=k (non MPI_ANY_SOURCE): è questo che forza l'ordine.
         * MPI_CHAR: stiamo trasferendo caratteri, un byte ciascuno.
         * MAXLEN: capacità massima del buffer di ricezione.
         */

    }
    else
    {
        /* TODO 2: costruire il messaggio e inviarlo al processo 0.
         *
         * snprintf(msg, MAXLEN, "[rank %d] sono un worker", rank)
         *
         * MPI_Send(msg, strlen(msg)+1, MPI_CHAR,
         *          dest=0, tag=0,
         *          MPI_COMM_WORLD)
         *
         * strlen(msg)+1: inviamo anche il terminatore '\0'
         *                così il processo 0 può stampare con printf.
         */

    }

    MPI_Finalize();
    return 0;
}
