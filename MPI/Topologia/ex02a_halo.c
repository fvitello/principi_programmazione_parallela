/*
 * Esercizio 02A  ·  Halo exchange    
 *
 * COSA IMPARIAMO
 * ──────────────
 * MPI_Dims_create calcola automaticamente le dimensioni ottimali
 * della griglia dato il numero di processi.
 *
 * Con bordi NON periodici (periods=0), i processi sui bordi
 * non hanno vicini in quella direzione — MPI_Cart_shift
 * restituisce MPI_PROC_NULL per quei vicini.
 *
 * MPI_Sendrecv verso MPI_PROC_NULL completa immediatamente
 * senza comunicare — nessun if sui bordi necessario.
 *
 * SCENARIO
 * ────────
 * Ogni processo ha un valore locale = proprio rank.
 * Fa halo exchange con tutti e 4 i vicini (su, giù, sin, des).
 * Stampa i valori ricevuti — MPI_PROC_NULL produce -1
 * (valore sentinella per indicare "nessun vicino").
 *
 * COMPILARE:  mpicc -Wall -O2 -o halo_a ex02a_halo.c
 * ESEGUIRE:   mpirun -np 6 ./halo_a
 */

#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* ── MPI_Dims_create ─────────────────────────────────────────
     * Calcola le dimensioni ottimali della griglia.
     * Firma: MPI_Dims_create(nnodes, ndims, dims)
     *
     * nnodes: numero totale di processi da distribuire
     * ndims:  numero di dimensioni
     * dims:   array di dimensioni — elementi già != 0 vengono
     *         mantenuti fissi, 0 viene calcolato da MPI.
     *
     * Con 6 processi: dims = {2, 3} o {3, 2}
     * Sceglie la configurazione più "quadrata" possibile. */
    int dims[2] = {0, 0};   /* 0 = lascia decidere a MPI */
    MPI_Dims_create(size, 2, dims);

    if (rank == 0)
        printf("Griglia: %d × %d  (%d processi)\n",
               dims[0], dims[1], size);

    /* ── Topologia NON periodica ─────────────────────────────────
     * periods = {0, 0}: bordi aperti in entrambe le direzioni.
     * I processi sui bordi non hanno vicini in quella direzione.
     * MPI_Cart_shift restituisce MPI_PROC_NULL per quei vicini. */
    int periods[2] = {0, 0};
    MPI_Comm cart;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart);

    int coords[2];
    MPI_Cart_coords(cart, rank, 2, coords);

    /* Vicini in tutte e 4 le direzioni */
    int up, down, left, right;
    MPI_Cart_shift(cart, 0, 1, &up,   &down);
    MPI_Cart_shift(cart, 1, 1, &left, &right);

    printf("rank %d (%d,%d): su=%2d giu=%2d sin=%2d des=%2d\n",
           rank, coords[0], coords[1],
           up, down, left, right);
    /* MPI_PROC_NULL = -1 → indica assenza di vicino */

    /* ── Halo exchange con tutti e 4 i vicini ────────────────────
     * Inviamo il proprio rank, riceviamo il rank del vicino.
     * Sendrecv verso MPI_PROC_NULL: completa immediatamente,
     * recvbuf rimane al valore iniziale (-1 = sentinella).
     * Nessun if sui bordi — il codice è uniforme per tutti. */
    int send_val = rank;
    int from_up = -1, from_down = -1;
    int from_left = -1, from_right = -1;

    /* Scambio verticale */
    MPI_Sendrecv(&send_val, 1, MPI_INT, down, 0,
                 &from_up,  1, MPI_INT, up,   0,
                 cart, MPI_STATUS_IGNORE);

    MPI_Sendrecv(&send_val,   1, MPI_INT, up,   1,
                 &from_down, 1, MPI_INT, down, 1,
                 cart, MPI_STATUS_IGNORE);

    /* Scambio orizzontale */
    MPI_Sendrecv(&send_val,  1, MPI_INT, right, 2,
                 &from_left, 1, MPI_INT, left,  2,
                 cart, MPI_STATUS_IGNORE);

    MPI_Sendrecv(&send_val,   1, MPI_INT, left,  3,
                 &from_right, 1, MPI_INT, right, 3,
                 cart, MPI_STATUS_IGNORE);

    printf("rank %d (%d,%d): ricevuto su=%2d giu=%2d sin=%2d des=%2d\n",
           rank, coords[0], coords[1],
           from_up, from_down, from_left, from_right);
    /* -1 = nessun vicino in quella direzione (bordo aperto) */

    MPI_Comm_free(&cart);
    MPI_Finalize();
    return 0;
}
