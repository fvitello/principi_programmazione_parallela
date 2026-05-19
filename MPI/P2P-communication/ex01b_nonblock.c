/*
 * Esercizio 01B  ·  Non-bloccanti         
 *
 * PUNTO DI PARTENZA: ex01a_nonblock.c
 * ────────────────────────────────────
 * In 01A ogni processo inviava sbuf[] al peer e riceveva
 * rbuf[], sovrapponendo una somma di work[].
 *
 * VARIANTE
 * ────────
 * Stesso schema, ma il "calcolo" sovrapposto è più realistico:
 * invece di sommare work[], ogni processo calcola la norma
 * euclidea del proprio sbuf[] durante la comunicazione.
 *
 * norma = sqrt(sbuf[0]^2 + sbuf[1]^2 + ... + sbuf[N-1]^2)
 *
 * Dopo il Wait, verificate che rbuf[0] == peer * N
 * come in 01A.
 *
 * OBIETTIVO PEDAGOGICO
 * ────────────────────
 * Il calcolo della norma è indipendente dalla comunicazione
 * (non tocca sbuf durante Isend, non tocca rbuf prima di Wait)
 * quindi può essere sovrapposto in modo sicuro.
 *
 * ATTENZIONE: sbuf NON deve essere modificato tra Isend e Wait.
 *             rbuf NON deve essere letto prima di Wait(&rreq).
 *
 * COMPILARE:  mpicc -Wall -O2 -o pp_nb_b ex01b_nonblock.c -lm
 * ESEGUIRE:   mpirun -np 2 ./pp_nb_b
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N    (1 << 20)
#define NREP 5

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0) fprintf(stderr, "Servono 2 processi.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int peer = 1 - rank;

    double *sbuf = malloc(N * sizeof(double));
    double *rbuf = malloc(N * sizeof(double));
    for (int i = 0; i < N; i++)
        sbuf[i] = (double)(rank * N + i);

    for (int rep = 0; rep < NREP; rep++)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        double t0 = MPI_Wtime();

        /* TODO 1: Dichiarare MPI_Request sreq, rreq */


        /* TODO 2: MPI_Irecv di rbuf dal peer, tag=0, request=&rreq
         * Postare PRIMA la Recv, poi la Send. */


        /* TODO 3: MPI_Isend di sbuf al peer, tag=0, request=&sreq */


        /* TODO 4: Calcolare la norma euclidea di sbuf[] mentre
         * la comunicazione avviene in background.
         * Questo è il calcolo sovrapposto — sbuf è in SOLA LETTURA
         * (non modificatelo — è ancora in uso da Isend).
         *
         */


        /* TODO 5: MPI_Wait per sreq (Send completata) */


        /* TODO 6: MPI_Wait per rreq (Recv completata, rbuf pronto) */


        double elapsed = (MPI_Wtime() - t0) * 1000.0;

        if (rank == 0)
            printf("rep %d: %.3f ms  norma=%.2f  rbuf[0]=%.0f\n",
                   rep, elapsed, norma, rbuf[0]);
    }

    free(sbuf); free(rbuf);
    MPI_Finalize();
    return 0;
}
