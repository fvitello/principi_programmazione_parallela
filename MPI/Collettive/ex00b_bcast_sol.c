/*
 * Soluzione Esercizio 00B MPI_Bcast
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10

int main(int argc, char **argv)
{
    int rank, size;
    double array[N];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* TODO 1: Solo il root (rank==0) inizializza array[]
     *   with valori rand() / RAND_MAX
     *  suggerimento: srand(42) prima della rand() per avere valori deterministici */

    if (rank == 0) {
        srand(42);
        for (int i = 0; i < N; i++)
            array[i] = (double)rand() / RAND_MAX;
        printf("Root genera: array[0]=%.4f  array[N-1]=%.4f\n",
               array[0], array[N-1]);
    }

    /* TODO 2: MPI_Bcast per distribuire array[] a tutti. */
    MPI_Bcast(array, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    /* TODO 3: Tutti i processi stampano array[0] e array[N-1].
     *   I valori devono essere identici su tutti i processi. */
    printf("rank %d: array[0]=%.4f  array[N-1]=%.4f\n",
           rank, array[0], array[N-1]);

    MPI_Finalize();
    return 0;
}
