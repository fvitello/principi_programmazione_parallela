#include <mpi.h>
#include <stdio.h>
#include <unistd.h>   // gethostname

int main(int argc, char** argv) {
    /*
     * Inizializzazione del runtime MPI.
     * Deve essere chiamata prima di usare qualunque funzione MPI.
     */

    MPI_Init(&argc, &argv);

    /*
     * size: numero totale di processi MPI nel communicator.
     * rank: identificatore del processo corrente (0 .. size-1).
     */
    int size, rank;

    /* Nome del nodo su cui sta girando il processo. */
    char hostname[256];

    /*
     * MPI_COMM_WORLD e' il communicator "globale" di default:
     * contiene tutti i processi lanciati nel job MPI.
     */
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Informazione locale del nodo (utile per capire la distribuzione). */
    gethostname(hostname, sizeof(hostname));

    /*
     * Ogni processo esegue questo stesso codice (modello SPMD),
     * ma stampa valori diversi in base al proprio rank.
     */
    printf("Hello from rank %d of %d running on %s\n",
           rank, size, hostname);

    /*
     * Chiusura del runtime MPI.
     * Dopo questa chiamata non si possono piu' usare primitive MPI.
     */
    MPI_Finalize();
    return 0;
}
