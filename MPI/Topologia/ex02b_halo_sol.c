/*
* Soluzione Esercizio 02B  ·  Somma dei vicini
 */
#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* TODO 1: MPI_Dims_create per dimensionare la griglia. */
    int dims[2] = {0, 0};
    MPI_Dims_create(size, 2, dims);

    /* TODO 2: MPI_Cart_create con periods={0,0} (bordi aperti). */
    int periods[2] = {0, 0};
    MPI_Comm cart;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart);

    /* TODO 3: MPI_Cart_coords e MPI_Cart_shift per trovare
     *   up, down, left, right. */
    int coords[2];
    MPI_Cart_coords(cart, rank, 2, coords);

    int up, down, left, right;
    MPI_Cart_shift(cart, 0, 1, &up,   &down);
    MPI_Cart_shift(cart, 1, 1, &left, &right);

    /* TODO 4: Halo exchange con tutti e 4 i vicini usando
     *   MPI_Sendrecv. Inizializzare i buffer di ricezione a -1
     *   come sentinella per MPI_PROC_NULL.
     */

    int send_val = rank;
    int from_up = -1, from_down = -1;
    int from_left = -1, from_right = -1;

    MPI_Sendrecv(&send_val, 1, MPI_INT, down, 0,
                 &from_up,  1, MPI_INT, up,   0,
                 cart, MPI_STATUS_IGNORE);
    MPI_Sendrecv(&send_val,  1, MPI_INT, up,   1,
                 &from_down, 1, MPI_INT, down, 1,
                 cart, MPI_STATUS_IGNORE);
    MPI_Sendrecv(&send_val,  1, MPI_INT, right, 2,
                 &from_left, 1, MPI_INT, left,  2,
                 cart, MPI_STATUS_IGNORE);
    MPI_Sendrecv(&send_val,   1, MPI_INT, left,  3,
                 &from_right, 1, MPI_INT, right, 3,
                 cart, MPI_STATUS_IGNORE);

    /* TODO 5: Calcolare la somma dei valori ricevuti,
     *   escludendo i -1 (bordi aperti senza vicino).
 */

    int sum = 0;
    if (from_up    != -1) sum += from_up;
    if (from_down  != -1) sum += from_down;
    if (from_left  != -1) sum += from_left;
    if (from_right != -1) sum += from_right;

    printf("rank %d (%d,%d): somma vicini = %d\n",
           rank, coords[0], coords[1], sum);

    /* TODO 6: MPI_Comm_free */
    MPI_Comm_free(&cart);
    MPI_Finalize();
    return 0;
}
