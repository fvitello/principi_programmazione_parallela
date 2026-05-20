/*
 * Esercizio 00B  ·  MPI_Sendrecv
 * PUNTO DI PARTENZA: ex00a_sendrecv.c
 * ────────────────────────────────────
 * In 00A ogni processo inviava il proprio rank al vicino destro
 * e riceveva dal vicino sinistro (rotazione a destra).
 *
 * VARIANTE
 * ────────
 * Ogni processo invia un array di N double al vicino destro
 * e riceve un array di N double dal vicino sinistro.
 * Dopo la comunicazione, ogni processo stampa la somma
 * dei valori ricevuti.
 *
 * Inizializzazione: processo k riempie il suo array con
 * il valore (double)k, cioè tutti gli elementi valgono k.
 *   array[i] = (double)rank   per ogni i
 *
 * SCHEMA (N=3, 4 processi):
 *   rank 0: invia [0,0,0] a rank 1, riceve [3,3,3] da rank 3
 *   rank 1: invia [1,1,1] a rank 2, riceve [0,0,0] da rank 0
 *   rank 2: invia [2,2,2] a rank 3, riceve [1,1,1] da rank 1
 *   rank 3: invia [3,3,3] a rank 0, riceve [2,2,2] da rank 2
 *
 * OUTPUT ATTESO (N=3):
 *   rank 0: somma ricevuta = 9.00  (3+3+3)
 *   rank 1: somma ricevuta = 0.00  (0+0+0)
 *   rank 2: somma ricevuta = 3.00  (1+1+1)
 *   rank 3: somma ricevuta = 6.00  (2+2+2)
 *
 * COMPILARE:  mpicc -Wall -O2 -o ring_b ex00b_sendrecv.c
 * ESEGUIRE:   mpirun -np 4 ./ring_b
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 3   /* dimensione dell'array */

int main(int argc, char **argv)
{
    int rank, size;
    double sbuf[N];   /* buffer di invio   */
    double rbuf[N];   /* buffer di ricezione */

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* TODO 1: Inizializzare sbuf.
     */


    /* TODO 2: Calcolare i vicini nel ring.
     */


    /* TODO 3: MPI_Sendrecv per scambiare gli array.
     * Differenza rispetto a 00A: count=N e tipo=MPI_DOUBLE
     * invece di count=1 e MPI_INT.
     */


    /* TODO 4: Calcolare e stampare la somma di rbuf.
     *   double sum = 0.0;
     *   for (int i = 0; i < N; i++) sum += rbuf[i];
     *   printf("rank %d: somma ricevuta = %.2f\n", rank, sum);
     */


    MPI_Finalize();
    return 0;
}
