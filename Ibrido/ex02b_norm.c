/*
 *
 * PUNTO DI PARTENZA: ex02a_norm.c
 * ────────────────────────────────
 * In 02A avete calcolato la norma euclidea (L2) con
 * OpenMP reduction(+) + MPI_Allreduce(MPI_SUM).
 *
 * VARIANTE: norma infinito (L∞)
 * ──────────────────────────────
 * La norma infinito è il massimo del valore assoluto:
 *
 *   ||v||∞ = max(|v[0]|, |v[1]|, ..., |v[N-1]|)
 *
 * Stessa struttura di 02A, ma:
 *   - OpenMP reduction con l'operatore max invece di +
 *   - MPI_Allreduce con MPI_MAX invece di MPI_SUM
 *
 * Inizializzazione:
 *   v[i] = (double)(rank * chunk + i) / N   → valori in [0, 1)
 *   La norma infinito attesa è (N-1)/N ≈ 1.0
 *
 * NOTA su OpenMP reduction max:
 *   #pragma omp parallel for reduction(max:local_max)
 *   Disponibile da OpenMP 3.1. Con GCC: -fopenmp.
 *
 * COMPILARE:
 *   mpicc -Wall -O2 -fopenmp -o norm_b ex02b_norm.c -lm
 * ESEGUIRE:
 *   OMP_NUM_THREADS=4 mpirun -np 2 ./norm_b
 */

#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N (1 << 22)

int main(int argc, char **argv)
{
    int rank, size, provided;

    /* TODO 1: MPI_Init_thread(MPI_THREAD_FUNNELED) + rank + size */


    /* TODO 2: Allocare v[chunk] e inizializzare:
     */


    /* TODO 3: OpenMP reduction per il massimo locale.
     */


    /* TODO 4: MPI_Allreduce con MPI_MAX per ottenere global_max.
     *   Stampare solo su rank 0. */


    MPI_Finalize();
    return 0;
}
