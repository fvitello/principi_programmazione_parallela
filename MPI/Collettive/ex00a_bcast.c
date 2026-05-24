/*
 * Esercizio 00A  ·  MPI_Bcast   
 *
 * COSA IMPARIAMO
 * ──────────────
 * MPI_Bcast distribuisce un dato dal processo root a TUTTI
 * gli altri processi in O(log P) invece di O(P) che avremmo 
 * con le chiamate P2P.
 *
 * SCENARIO
 * ────────
 * Il processo root legge un parametro N dalla riga di comando
 * e lo distribuisce a tutti i processi con MPI_Bcast.
 * Ogni processo usa N per allocare e inizializzare il proprio
 * array locale.
 *
 * COMPILARE:  mpicc -Wall -O2 -o bcast_a ex00a_bcast.c
 * ESEGUIRE:   mpirun -np 4 ./bcast_a 1000
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int rank, size;
    int N = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* ── Solo il root legge il parametro ─────────────────────────
     * Gli altri processi non hanno accesso agli argomenti — o
     * meglio, li potrebbero avere a seconda delle implementazioni di MPI.
     * Il modo più corretto è distribuire i parametri via MPI_Bcast. */
    if (rank == 0) {
        if (argc < 2) {
            fprintf(stderr, "Uso: mpirun -np N ./bcast_a <dimensione>\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        N = atoi(argv[1]);
        printf("Root legge N=%d dalla riga di comando\n", N);
    }

    /* ── MPI_Bcast ───────────────────────────────────────────────
     * Firma: MPI_Bcast(buf, count, type, root, comm)
     *
     * buf:   puntatore al dato da distribuire
     *        - sul root:    contiene il dato da inviare
     *        - sugli altri: verrà sovrascritto con il dato ricevuto
     * count: numero di elementi
     * type:  tipo MPI
     * root:  rank del processo che ha il dato originale
     * comm:  communicator
     *
     * TUTTI i processi chiamano MPI_Bcast — non solo il root.
     * È una operazione collettiva: tutti devono partecipare. */
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* Ora tutti i processi hanno N */
    printf("rank %d: N=%d\n", rank, N);

    /* Ogni processo alloca e inizializza il proprio array */
    double *array = malloc(N * sizeof(double));
    for (int i = 0; i < N; i++)
        array[i] = (double)(rank * N + i);

    printf("rank %d: array[0]=%.0f  array[N-1]=%.0f\n",
           rank, array[0], array[N-1]);

    free(array);
    MPI_Finalize();
    return 0;
}
