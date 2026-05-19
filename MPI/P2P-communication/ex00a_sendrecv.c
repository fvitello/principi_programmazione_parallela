/*
 * Esercizio 00A  ·  MPI_Sendrecv 
 *
 * COSA IMPARIAMO
 * ──────────────
 * MPI_Sendrecv esegue Send e Recv come un'unica operazione
 * atomica, eliminando il rischio di deadlock da Send simmetrica.
 *
 * SCENARIO: ring di P processi
 * ─────────────────────────────
 * Ogni processo invia il proprio valore al vicino destro
 * e riceve il valore dal vicino sinistro.
 *
 *   rank 0 ──► rank 1 ──► rank 2 ──► rank 3
 *     ▲                                 │
 *     └─────────────────────────────────┘
 *
 * Con MPI_Send bloccante questo creerebbe deadlock su
 * messaggi grandi. MPI_Sendrecv lo risolve.
 *
 * COMPILARE:  mpicc -Wall -O2 -o ring_a ex00a_sendrecv.c
 * ESEGUIRE:   mpirun -np 4 ./ring_a
 */

#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int rank, size;
    int send_val, recv_val;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Ogni processo ha come valore il proprio rank */
    send_val = rank;

    /* ── Calcolare i vicini nel ring ────────────────────────────
     * Il vicino destro riceve il nostro valore.
     * Il vicino sinistro ci invia il suo valore.
     * Il modulo % size gestisce i bordi: il processo 0 riceve
     * dal processo size-1 (l'anello si chiude). */
    int right = (rank + 1) % size;   /* destinatario della Send */
    int left  = (rank - 1 + size) % size; /* mittente della Recv */

    printf("rank %d: invio %d a rank %d, aspetto da rank %d\n",
           rank, send_val, right, left);

    /* ── MPI_Sendrecv ────────────────────────────────────────────
     * Firma: MPI_Sendrecv(
     *   sendbuf, sendcount, sendtype, dest,   sendtag,
     *   recvbuf, recvcount, recvtype, source, recvtag,
     *   comm, status)
     *
     * Parametri di invio:
     *   &send_val  = puntatore al valore da inviare
     *   1, MPI_INT = un intero
     *   right      = rank del destinatario (vicino destro)
     *   0          = tag della Send
     *
     * Parametri di ricezione:
     *   &recv_val  = dove scrivere il valore ricevuto
     *   1, MPI_INT = un intero
     *   left       = rank del mittente atteso (vicino sinistro)
     *   0          = tag della Recv (deve coincidere con quello
     *                usato nella Send del mittente)
     *
     * NOTA: dest e source sono DIVERSI — non è uno scambio
     * simmetrico tra due processi, è una rotazione circolare.
     * Ogni processo invia a destra e riceve da sinistra. ──────── */
    MPI_Sendrecv(
        &send_val, 1, MPI_INT, right, 0,   /* Send → vicino destro */
        &recv_val, 1, MPI_INT, left,  0,   /* Recv ← vicino sinistro */
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    printf("rank %d: ricevuto %d da rank %d\n", rank, recv_val, left);

    /* Verifica: ogni processo dovrebbe aver ricevuto rank-1
     * (con wrap-around: il processo 0 riceve da size-1) */
    int expected = left;  /* il vicino sinistro ha rank=left */
    if (recv_val == expected)
        printf("rank %d: ✓ corretto\n", rank);
    else
        printf("rank %d: ✗ errore! atteso %d, ricevuto %d\n",
               rank, expected, recv_val);

    MPI_Finalize();
    return 0;
}
