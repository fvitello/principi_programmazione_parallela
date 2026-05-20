/*
 *  Esercizio 01B  ·  Non-bloccanti         
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
    for (int i = 0; i < N; i++) sbuf[i] = (double)(rank * N + i);

    for (int rep = 0; rep < NREP; rep++) {
        MPI_Barrier(MPI_COMM_WORLD);
        double t0 = MPI_Wtime();

        /* TODO 1: Dichiarare MPI_Request sreq, rreq */

        MPI_Request sreq, rreq;

        /* TODO 2: MPI_Irecv di rbuf dal peer, tag=0, request=&rreq */
        MPI_Irecv(rbuf, N, MPI_DOUBLE, peer, 0, MPI_COMM_WORLD, &rreq);

        /* TODO 3: MPI_Isend di sbuf al peer, tag=0, request=&sreq */
        MPI_Isend(sbuf, N, MPI_DOUBLE, peer, 0, MPI_COMM_WORLD, &sreq);

        /* TODO 4: Calcolare la norma euclidea di sbuf[] mentre
         * la comunicazione avviene in background.
         * Questo è il calcolo sovrapposto — sbuf è in SOLA LETTURA
         * (non modificatelo — è ancora in uso da Isend).
         *
         */
        double norma = 0.0;
        for (int i = 0; i < N; i++)
            norma += sbuf[i] * sbuf[i];
        norma = sqrt(norma);

        /* TODO 5: MPI_Wait per sreq (Send completata) */
        MPI_Wait(&sreq, MPI_STATUS_IGNORE);
        
        /* TODO 6: MPI_Wait per rreq (Recv completata, rbuf pronto) */
        MPI_Wait(&rreq, MPI_STATUS_IGNORE);

        double elapsed = (MPI_Wtime() - t0) * 1000.0;

        if (rank == 0)
            printf("rep %d: %.3f ms  norma=%.2f  rbuf[0]=%.0f\n",
                   rep, elapsed, norma, rbuf[0]);
    }

    free(sbuf); free(rbuf);
    MPI_Finalize();
    return 0;
}
