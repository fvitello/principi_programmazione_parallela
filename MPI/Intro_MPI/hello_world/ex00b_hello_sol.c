/*
 * ESEMPIO 00b: HELLO WORLD IN MPI (VARIANTE)
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int rank, size;

    /* TODO 1: MPI_Init */
    MPI_Init(&argc, &argv);

    /* TODO 2: MPI_Comm_rank e MPI_Comm_size */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

     /* TODO 3: Controllo — se size < 2, il processo 0 stampa errore
     *         e tutti i processi chiamano MPI_Abort.
     */
     if (size < 2) {
        if (rank == 0)
            fprintf(stderr, "Errore: servono almeno 2 processi.\n"
                            "Uso: mpirun -np N ./hello_b  con N >= 2\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* TODO 4: Logica applicativa
     *   - rank 0 → stampa il messaggio del root
     *   - altri  → stampa rank e parità (rank % 2 == 0 → PARI)
     */
    if (rank == 0) {
        printf("[ROOT rank=0] Ci sono %d processi. Aspetto %d risultati.\n",
               size, size - 1);
    } else {
        const char *parita = (rank % 2 == 0) ? "PARI" : "DISPARI";
        printf("[rank=%d] sono %s\n", rank, parita);
    }

    /* TODO 5: MPI_Finalize */
    MPI_Finalize();
    return 0;
}
