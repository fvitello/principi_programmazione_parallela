/*
 *  Esercizio 01B  ·  Cart_shift + Sendrecv   
 *
 * PUNTO DI PARTENZA: ex01a_cart.c
 * ────────────────────────────────
 * In 01A avete creato la griglia e trovato i vicini con
 * MPI_Cart_shift. Ora usate quei vicini per comunicare.
 *
 * VARIANTE: scambio con i vicini lungo la riga
 * ─────────────────────────────────────────────
 * Ogni processo invia il proprio rank al vicino DESTRO
 * e riceve dal vicino SINISTRO, usando MPI_Sendrecv.
 *
 * È lo stesso ring exchange di Lab Sessione 2 esercizio 00A,
 * ma ora i vicini vengono calcolati automaticamente da
 * MPI_Cart_shift invece che con (rank+1)%size.
 *
 * SCHEMA (griglia 2×3, solo la riga 0):
 *
 *   P0 ──► P1 ──► P2
 *    ▲               │
 *    └───────────────┘   (periodica → P2 invia a P0)
 *
 * Ogni processo verifica che il valore ricevuto sia
 * uguale al rank del vicino sinistro.
 *
 * COMPILARE:  mpicc -Wall -O2 -o cart_b ex01b_cart.c
 * ESEGUIRE:   mpirun -np 6 ./cart_b
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
        if (rank == 0) fprintf(stderr, "Servono 6 processi.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* TODO 1: Creare la topologia cartesiana 2×3.
    */


    /* TODO 2: Ottenere le proprie coordinate con MPI_Cart_coords.
     */


    /* TODO 3: Trovare i vicini SINISTRA e DESTRA con MPI_Cart_shift
     *   sulla dimensione 1 (colonne), spostamento +1.
     */


    /* TODO 4: MPI_Sendrecv per scambiare il proprio rank.
     *   Inviare rank a right, ricevere recv_val da left.
     */



    /* TODO 5: Stampare e verificare.
     *   Il valore ricevuto deve essere uguale al rank del
     *   vicino sinistro (left).
     */


    /* TODO 6: MPI_Comm_free(&cart_comm) */


    MPI_Finalize();
    return 0;
}
