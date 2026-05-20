/*
 * Esercizio 02B  ·  Gather non-bloccante
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N (1 << 22)

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
    double *local = malloc(chunk * sizeof(double));
    for (int i = 0; i < chunk; i++)
        local[i] = (double)(rank * chunk + i);

    if (rank == 0) {
        double *array = malloc(N * sizeof(double));
        double t0 = MPI_Wtime();

        /* TODO 1: Allocare array di MPI_Request di dimensione size-1 */

        MPI_Request *reqs = malloc((size-1) * sizeof(MPI_Request));

        /* TODO 2: Avviare Irecv da ogni processo p=1..size-1.
         *   MPI_Irecv(&array[p*chunk], chunk, MPI_DOUBLE,
         *             p, 0, MPI_COMM_WORLD, &reqs[p-1]);
         *
         * Nota: array[p*chunk] è dove vogliamo il chunk del processo p.
         */
        for (int p = 1; p < size; p++)
            MPI_Irecv(&array[p * chunk], chunk, MPI_DOUBLE,
                      p, 0, MPI_COMM_WORLD, &reqs[p-1]);

        /* TODO 3: Calcolo sovrapposto — copiare e sommare il chunk
         * del processo 0 (non dipende dalle Irecv in corso).
         *   for i: array[i] = local[i]
         *   calcolare local_sum0 = somma di local[]
         */
        double local_sum0 = 0.0;
        for (int i = 0; i < chunk; i++) {
            array[i] = local[i];
            local_sum0 += local[i];
        }

        /* TODO 4: MPI_Waitall per aspettare tutte le Irecv.
         *   MPI_Waitall(size-1, reqs, MPI_STATUSES_IGNORE);
         */
        MPI_Waitall(size-1, reqs, MPI_STATUSES_IGNORE);

        /* TODO 5: Calcolare la somma totale di array[] e stamparla.
         * Dovrebbe essere N*(N-1)/2.
         */
        double total = local_sum0;
        for (int p = 1; p < size; p++)
            for (int i = 0; i < chunk; i++)
                total += array[p * chunk + i];

        double elapsed = (MPI_Wtime() - t0) * 1000.0;
        printf("P0: gather completato in %.3f ms\n", elapsed);
        printf("P0: somma totale = %.0f  (atteso %.0f)\n",
               total, (double)N * (N-1) / 2.0);

        free(array); free(reqs);
    } else {
        MPI_Send(local, chunk, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    free(local);
    MPI_Finalize();
    return 0;
}
