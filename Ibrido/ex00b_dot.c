/*
 * ╔══════════════════════════════════════════════════════════╗
 * ║  Lab Ibrido  ·  Esercizio 00B  ·  Prodotto scalare      ║
 * ╚══════════════════════════════════════════════════════════╝
 *
 * PUNTO DI PARTENZA: ex00a_dot.c
 * ────────────────────────────────
 * In 00A il root inizializzava i vettori e usava Scatter.
 *
 * VARIANTE
 * ────────
 * Ogni processo inizializza direttamente il proprio chunk
 * senza Scatter — non serve un root che alloca tutto.
 * Poi OpenMP reduction + MPI_Allreduce come in 00A.
 *
 * Inizializzazione:
 *   lA[i] = (double)(rank * chunk + i) / N
 *   lB[i] = (double)(N - rank * chunk - i) / N
 *
 * COMPILARE:
 *   mpicc -Wall -O2 -fopenmp -o dot_b ex00b_dot.c
 * ESEGUIRE:
 *   OMP_NUM_THREADS=4 mpirun -np 2 ./dot_b
 */

#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N (1 << 20)

int main(int argc, char **argv)
{
    int rank, size, provided;

    /* TODO 1: MPI_Init_thread con MPI_THREAD_FUNNELED.
     *   Verificare che provided >= MPI_THREAD_FUNNELED. */


    /* TODO 2: Comm_rank, Comm_size.
     *   Calcolare chunk = N / size.
     *   Stampare: processi, thread, chunk. */


    /* TODO 3: Allocare lA[chunk] e lB[chunk].
     *   Inizializzare direttamente (senza Scatter):
     *   lA[i] = (double)(rank * chunk + i) / N
     *   lB[i] = (double)(N - rank * chunk - i) / N */


    /* TODO 4: OpenMP reduction per calcolare local_dot.
     *   #pragma omp parallel for reduction(+:local_dot)
     *   for (int i = 0; i < chunk; i++)
     *       local_dot += lA[i] * lB[i]; */


    /* TODO 5: MPI_Allreduce(MPI_SUM) per ottenere global_dot.
     *   Stampare solo su rank 0. */


    MPI_Finalize();
    return 0;
}
