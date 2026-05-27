/*
 * Soluzione Esercizio 01B  ·  Cart_shift + Sendrecv   
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

    int dims[2]    = {2, 3};
    int periods[2] = {0, 0};
    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);

     /* TODO 2: Ottenere le proprie coordinate con MPI_Cart_coords.
     */
    int coords[2];
    MPI_Cart_coords(cart_comm, rank, 2, coords);
    /* TODO 3: Trovare i vicini SINISTRA e DESTRA con MPI_Cart_shift
     *   sulla dimensione 1 (colonne), spostamento +1.
     */

    int left, right;
    MPI_Cart_shift(cart_comm, 1, 1, &left, &right);

    /* TODO 4: MPI_Sendrecv per scambiare il proprio rank.
     *   Inviare rank a right, ricevere recv_val da left.
     */
    int send_val = rank;
    int recv_val = -1;
    MPI_Sendrecv(&send_val, 1, MPI_INT, right, 0,
                 &recv_val, 1, MPI_INT, left,  0,
                 cart_comm, MPI_STATUS_IGNORE);

    /* TODO 5: Stampare e verificare.
     *   Il valore ricevuto deve essere uguale al rank del
     *   vicino sinistro (left).
     */
    printf("rank %d (%d,%d): inviato %d a %d, ricevuto %d da %d %s\n",
           rank, coords[0], coords[1],
           send_val, right, recv_val, left,
           recv_val == left ? "✓" : "✗");

    /* TODO 6: MPI_Comm_free(&cart_comm) */
    MPI_Comm_free(&cart_comm);
    MPI_Finalize();
    return 0;
}
