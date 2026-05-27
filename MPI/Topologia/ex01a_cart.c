/*
 * Esercizio 01A  ·  MPI_Cart_create 
 *
 * COSA IMPARIAMO
 * ──────────────
 * MPI_Cart_create crea un communicator con una topologia
 * cartesiana — una griglia N-dimensionale di processi.
 * MPI fornisce funzioni per:
 *   - trovare le coordinate di un processo nella griglia
 *   - trovare i vicini in ogni direzione
 *   - calcolare il rank da coordinate e viceversa
 *
 * PERCHÉ È UTILE
 * ──────────────
 * In simulazioni numeriche su griglia  ogni processo
 * gestisce un sottodominio e comunica con i vicini.
 * Senza topologia cartesiana dovete calcolare manualmente
 * i rank dei vicini con aritmetica modulo — con Cart è
 * automatico e il codice è più leggibile.
 *
 * SCENARIO
 * ────────
 * 6 processi su griglia 2×3. Ogni processo stampa:
 *   - il proprio rank globale
 *   - le proprie coordinate (riga, colonna)
 *   - i propri vicini su, giù, sinistra, destra
 *
 * COMPILARE:  mpicc -Wall -O2 -o cart_a ex01a_cart.c
 * ESEGUIRE:   mpirun -np 6 ./cart_a
 */

#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 6) {
        if (rank == 0) fprintf(stderr, "Servono esattamente 6 processi.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* ── MPI_Cart_create ─────────────────────────────────────────
     * Firma: MPI_Cart_create(comm_old, ndims, dims, periods,
     *                        reorder, comm_cart)
     *
     * ndims:    numero di dimensioni (2 per una griglia 2D)
     * dims:     dimensioni della griglia [nrighe, ncols]
     * periods:  se la griglia è periodica (wrap-around) in
     *           ciascuna dimensione. periods[i]=1 → periodica.
     * reorder:  se MPI può riassegnare i rank per ottimizzare
     *           il mapping sulla topologia fisica. 0 = no.
     * comm_cart: il nuovo communicator con topologia. */
    int dims[2]    = {2, 3};   /* 2 righe × 3 colonne */
    int periods[2] = {1, 1};   /* periodica in entrambe le direzioni */
    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);

    /* ── MPI_Cart_coords ─────────────────────────────────────────
     * Dato un rank, restituisce le coordinate nella griglia.
     * Firma: MPI_Cart_coords(comm, rank, ndims, coords) */
    int coords[2];
    MPI_Cart_coords(cart_comm, rank, 2, coords);
    printf("rank %d: coord=(%d,%d)\n", rank, coords[0], coords[1]);

    /* ── MPI_Cart_shift ──────────────────────────────────────────
     * Calcola i vicini in una direzione.
     * Firma: MPI_Cart_shift(comm, direction, disp,
     *                       rank_source, rank_dest)
     *
     * direction: dimensione (0=righe, 1=colonne)
     * disp:      spostamento (+1 = avanti, -1 = indietro)
     * rank_source: vicino "indietro" (da cui ricevereste)
     * rank_dest:   vicino "avanti"   (a cui inviereste)
     *
     * Con periods=1: i bordi si connettono (wrap-around).
     * Senza periodicità: MPI_PROC_NULL sui bordi. */
    int up, down, left, right;
    MPI_Cart_shift(cart_comm, 0, 1, &up,   &down);   /* dir 0 = righe */
    MPI_Cart_shift(cart_comm, 1, 1, &left, &right);  /* dir 1 = col  */

    printf("rank %d (%d,%d): su=%d giu=%d sin=%d des=%d\n",
           rank, coords[0], coords[1], up, down, left, right);

    MPI_Comm_free(&cart_comm);
    MPI_Finalize();
    return 0;
}
