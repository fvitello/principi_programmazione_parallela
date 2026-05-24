/*
 * Esercizio 01A  ·  MPI_Scatter + MPI_Gather
 *
 * COSA IMPARIAMO
 * ──────────────
 * MPI_Scatter distribuisce parti uguali di un array dal root
 * a tutti i processi. MPI_Gather fa l'operazione inversa.
 *
 * La scorsa volta avete fatto scatter e gather
 * manualmente con Isend/Irecv + Waitall. MPI_Scatter e
 * MPI_Gather fanno lo stesso in una sola chiamata.
 *
 * SCENARIO
 * ────────
 * Root ha un array di N elementi. Lo distribuisce in chunk
 * uguali a tutti i processi (Scatter). Ogni processo calcola
 * la somma del proprio chunk. Il root raccoglie le somme
 * parziali (Gather) e calcola la somma totale.
 *
 *   Root: [0,1,2,...,N-1]
 *         ↓ MPI_Scatter
 *   P0: [0,1,...,chunk-1]      → somma parziale s0
 *   P1: [chunk,...,2chunk-1]   → somma parziale s1
 *   P2: [2chunk,...,3chunk-1]  → somma parziale s2
 *         ↑ MPI_Gather
 *   Root: [s0, s1, s2]  → somma totale = s0+s1+s2
 *
 * COMPILARE:  mpicc -Wall -O2 -o sg_a ex01a_scatter_gather.c
 * ESEGUIRE:   mpirun -np 4 ./sg_a
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 16   /* deve essere divisibile per size */

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0) fprintf(stderr, "N non divisibile per %d\n", size);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int chunk = N / size;
    double *array  = NULL;   /* solo il root alloca l'array completo */
    double *local  = malloc(chunk * sizeof(double));
    double *sums   = NULL;   /* solo il root raccoglie le somme dei processi */

    /* ── Root inizializza l'array completo ───────────────────── */
    if (rank == 0) {
        array = malloc(N * sizeof(double));
        for (int i = 0; i < N; i++) array[i] = (double)i;
        sums = malloc(size * sizeof(double));
        printf("Root: array = [0, 1, 2, ..., %d]\n", N-1);
    }

    /* ── MPI_Scatter ─────────────────────────────────────────────
     * Firma: MPI_Scatter(sendbuf, sendcount, sendtype,
     *                    recvbuf, recvcount, recvtype,
     *                    root, comm)
     *
     * sendbuf:   array completo sul root (ignorato sugli altri)
     * sendcount: elementi inviati a CIASCUN processo (= chunk)
     * recvbuf:   buffer locale dove ricevere il proprio chunk
     * recvcount: elementi ricevuti (= chunk, uguale a sendcount)
     *
     * MPI_Scatter divide sendbuf in blocchi da sendcount elementi
     * e invia il k-esimo blocco al processo k.
     * Il root riceve il proprio chunk in recvbuf come gli altri. */
    MPI_Scatter(array, chunk, MPI_DOUBLE,
                local, chunk, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    /* Ogni processo calcola la somma del proprio chunk */
    double local_sum = 0.0;
    for (int i = 0; i < chunk; i++) local_sum += local[i];
    printf("rank %d: chunk=[%.0f..%.0f]  somma=%.0f\n",
           rank, local[0], local[chunk-1], local_sum);

    /* ── MPI_Gather ──────────────────────────────────────────────
     * Firma: MPI_Gather(sendbuf, sendcount, sendtype,
     *                   recvbuf, recvcount, recvtype,
     *                   root, comm)
     *
     * Speculare a Scatter: ogni processo invia sendcount elementi,
     * il root li raccoglie in recvbuf nell'ordine dei rank.
     * recvbuf sul root: [somma_P0, somma_P1, somma_P2, ...] */
    MPI_Gather(&local_sum, 1, MPI_DOUBLE,
               sums,       1, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    /* Root calcola la somma totale */
    if (rank == 0) {
        double total = 0.0;
        for (int i = 0; i < size; i++) total += sums[i];
        printf("Root: somma totale = %.0f  (atteso %.0f)\n",
               total, (double)N*(N-1)/2.0);
        free(array); free(sums);
    }

    free(local);
    MPI_Finalize();
    return 0;
}
