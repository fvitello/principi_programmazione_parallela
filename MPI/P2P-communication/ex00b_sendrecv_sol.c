/*
 * ╔══════════════════════════════════════════════════════════╗
 * ║  Lab MPI  ·  Esercizio 00B  ·  MPI_Sendrecv [SOLUZIONE] ║
 * ╚══════════════════════════════════════════════════════════╝
 */
#include <mpi.h>
#include <stdio.h>
#define N 3

int main(int argc, char **argv)
{
    int rank, size;
    double sbuf[N], rbuf[N];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* TODO 1: Inizializzare sbuf.
     *   for (int i = 0; i < N; i++)
     *       sbuf[i] = (double)rank;
     */

    for (int i = 0; i < N; i++) sbuf[i] = (double)rank;

    /* TODO 2: Calcolare i vicini nel ring.
     *   int right = (rank + 1) % size;
     *   int left  = (rank - 1 + size) % size;
     */
    int right = (rank + 1) % size;
    int left  = (rank - 1 + size) % size;

    /* TODO 3: MPI_Sendrecv per scambiare gli array.
     *
     * MPI_Sendrecv(
     *     sbuf, N, MPI_DOUBLE, right, 0,
     *     rbuf, N, MPI_DOUBLE, left,  0,
     *     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
     *
     * Differenza rispetto a 00A: count=N e tipo=MPI_DOUBLE
     * invece di count=1 e MPI_INT.
     */
    MPI_Sendrecv(sbuf, N, MPI_DOUBLE, right, 0,
                 rbuf, N, MPI_DOUBLE, left,  0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    /* TODO 4: Calcolare e stampare la somma di rbuf.
     *   double sum = 0.0;
     *   for (int i = 0; i < N; i++) sum += rbuf[i];
     *   printf("rank %d: somma ricevuta = %.2f\n", rank, sum);
     */

    double sum = 0.0;
    for (int i = 0; i < N; i++) sum += rbuf[i];
    printf("rank %d: somma ricevuta = %.2f\n", rank, sum);

    MPI_Finalize();
    return 0;
}
