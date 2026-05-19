/*
 *   Esercizio 01A  ·  Ping-Pong non-bloccante
 * COSA IMPARIAMO
 * ──────────────
 * MPI_Isend e MPI_Irecv avviano la comunicazione e ritornano
 * immediatamente. La comunicazione avviene in background.
 * MPI_Wait aspetta il completamento.
 *
 * Confronto con il ping-pong bloccante di ex01a_pingpong.c:
 *   Bloccante:     Send → attesa → Recv → attesa → calcolo
 *   Non-bloccante: Irecv + Isend → calcolo sovrapposto → Wait
 *
 * In questo esercizio simuliamo lavoro utile con una somma
 * di array per mostrare il concetto di latency hiding.
 *
 * COMPILARE:  mpicc -Wall -O2 -o pp_nb_a ex01a_nonblock.c
 * ESEGUIRE:   mpirun -np 2 ./pp_nb_a
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N       (1 << 20)   /* 1M double = 8 MB per buffer */
#define NREP    5

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0) fprintf(stderr, "Servono esattamente 2 processi.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int peer = 1 - rank;   /* l'altro processo: se sono 0, peer=1 e viceversa */

    /* Allocare e inizializzare i buffer */
    double *sbuf = malloc(N * sizeof(double));
    double *rbuf = malloc(N * sizeof(double));
    double *work = malloc(N * sizeof(double));  /* buffer per il "calcolo" */
    for (int i = 0; i < N; i++) {
        sbuf[i] = (double)(rank * N + i);
        work[i] = 1.0;
    }

    if (rank == 0)
        printf("N = %d double (%.1f MB per buffer)\n\n",
               N, N * 8.0 / (1 << 20));

    for (int rep = 0; rep < NREP; rep++)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        double t0 = MPI_Wtime();

        /* ── PASSO 1: avviare Irecv PRIMA di Isend ───────────────
         * Buona pratica: postare la Recv prima così il buffer di
         * ricezione è già registrato quando i dati arrivano.
         * Il runtime può trasferire direttamente senza copie. ──── */
        MPI_Request rreq, sreq;

        MPI_Irecv(rbuf, N, MPI_DOUBLE, peer, 0,
                  MPI_COMM_WORLD, &rreq);

        MPI_Isend(sbuf, N, MPI_DOUBLE, peer, 0,
                  MPI_COMM_WORLD, &sreq);

        /* ── PASSO 2: calcolo sovrapposto ────────────────────────
         * Mentre la comunicazione avviene in background, facciamo
         * qualcosa di utile. Questo è il latency hiding.
         * ATTENZIONE: non toccare sbuf qui — è in uso dal runtime!
         * work[] è un buffer separato, sicuro da usare. ─────────── */
        double local_sum = 0.0;
        for (int i = 0; i < N; i++)
            local_sum += work[i];   /* somma work[], NON sbuf[] */

        /* ── PASSO 3: aspettare il completamento ─────────────────
         * MPI_Wait blocca finché l'operazione è completata.
         * Dopo il Wait: sreq = MPI_REQUEST_NULL, sbuf è libero.
         *               rreq = MPI_REQUEST_NULL, rbuf contiene i dati. */
        MPI_Wait(&sreq, MPI_STATUS_IGNORE);
        MPI_Wait(&rreq, MPI_STATUS_IGNORE);

        double elapsed = (MPI_Wtime() - t0) * 1000.0;  /* ms */

        /* Verifica: il primo elemento di rbuf deve valere
         * peer * N + 0 = peer * N */
        double expected = (double)(peer * N);
        int ok = (fabs(rbuf[0] - expected) < 0.5);

        if (rank == 0)
            printf("rep %d: %.3f ms  somma_locale=%.0f  rbuf[0]=%.0f %s\n",
                   rep, elapsed, local_sum, rbuf[0],
                   ok ? "✓" : "✗");
    }

    free(sbuf); free(rbuf); free(work);
    MPI_Finalize();
    return 0;
}
