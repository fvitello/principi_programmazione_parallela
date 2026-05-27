/*
* Soluzione Esercizio 00B  ·  Comm_split
 */
#include <mpi.h>
#include <stdio.h>

#define NCOLS 4

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 8) {
        if (rank == 0) fprintf(stderr, "Servono 8 processi.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int row = rank / NCOLS;
    int col = rank % NCOLS;

    /* TODO 1: MPI_Comm_split per creare un communicator per colonna. */
    MPI_Comm col_comm;
    MPI_Comm_split(MPI_COMM_WORLD, col, row, &col_comm);

    /* TODO 2: Ottenere col_rank e col_size */
    int col_rank, col_size;
    MPI_Comm_rank(col_comm, &col_rank);
    MPI_Comm_size(col_comm, &col_size);

    printf("rank %d: riga=%d col=%d  →  col_rank=%d\n",
           rank, row, col, col_rank);

    /* TODO 3: MPI_Reduce su col_comm per sommare i rank globali */
    double local_val = (double)rank;
    double col_sum   = 0.0;
    MPI_Reduce(&local_val, &col_sum, 1, MPI_DOUBLE,
               MPI_SUM, 0, col_comm);

    if (col_rank == 0)
        printf("col %d: somma rank = %.0f  (atteso %.0f)\n",
               col, col_sum, (double)(col + col + NCOLS));

    /* TODO 4: MPI_Comm_free(&col_comm) */

    MPI_Comm_free(&col_comm);
    MPI_Finalize();
    return 0;
}
