/*
 * ╔══════════════════════════════════════════════════════════╗
 * ║  Lab Ibrido  ·  Esercizio 02A  ·  Norma euclidea        ║
 * ║  Codice mostrato dal docente                             ║
 * ╚══════════════════════════════════════════════════════════╝
 *
 * COSA IMPARIAMO
 * ──────────────
 * La riduzione ibrida: OpenMP reduction dentro ogni processo,
 * poi MPI_Allreduce per combinare i risultati globalmente.
 *
 * Questo pattern è alla base di quasi tutti gli algoritmi
 * iterativi paralleli: gradiente coniugato, power method,
 * normalizzazione di vettori, controllo di convergenza.
 *
 * SCENARIO
 * ────────
 * Calcolare la norma euclidea di un vettore distribuito:
 *
 *   ||v|| = sqrt(v[0]² + v[1]² + ... + v[N-1]²)
 *
 *   1. Ogni processo calcola la somma dei quadrati del
 *      proprio chunk con OpenMP reduction
 *   2. MPI_Allreduce somma le somme parziali
 *   3. Ogni processo calcola sqrt del risultato globale
 *
 * COMPILARE:
 *   mpicc -Wall -O2 -fopenmp -o norm_a ex02a_norm.c -lm
 * ESEGUIRE:
 *   OMP_NUM_THREADS=4 mpirun -np 2 ./norm_a
 */

#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N (1 << 22)   /* 4M elementi */

int main(int argc, char **argv)
{
    int rank, size, provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk = N / size;

    /* Ogni processo alloca e inizializza il proprio chunk.
     * v[i] = 1.0 per tutti → norma attesa = sqrt(N) */
    double *v = malloc(chunk * sizeof(double));
    for (int i = 0; i < chunk; i++)
        v[i] = 1.0;

    /* ── OpenMP reduction: somma dei quadrati locale ─────────────
     * Ogni thread lavora su un sottointervallo del chunk.
     * reduction(+:local_sum) garantisce che ogni thread
     * accumuli in una variabile privata, poi le somme vengono
     * combinate alla fine — no race condition. */
    double local_sum = 0.0;

    #pragma omp parallel for reduction(+:local_sum) schedule(static)
    for (int i = 0; i < chunk; i++)
        local_sum += v[i] * v[i];

    /* ── MPI_Allreduce: somma globale ────────────────────────────
     * Tutti i processi ricevono la somma globale dei quadrati.
     * Allreduce invece di Reduce perché tutti hanno bisogno
     * della norma per normalizzare il proprio chunk. */
    double global_sum = 0.0;
    MPI_Allreduce(&local_sum, &global_sum, 1, MPI_DOUBLE,
                  MPI_SUM, MPI_COMM_WORLD);

    double norm = sqrt(global_sum);

    /* Ora ogni processo può normalizzare il proprio chunk */
    for (int i = 0; i < chunk; i++)
        v[i] /= norm;

    /* Verifica: la norma del vettore normalizzato deve essere 1 */
    double check_sum = 0.0;
    #pragma omp parallel for reduction(+:check_sum) schedule(static)
    for (int i = 0; i < chunk; i++)
        check_sum += v[i] * v[i];

    double check_global = 0.0;
    MPI_Allreduce(&check_sum, &check_global, 1, MPI_DOUBLE,
                  MPI_SUM, MPI_COMM_WORLD);

    if (rank == 0)
        printf("N=%d  ||v||=%.6f  ||v_norm||=%.6f  (atteso 1.0)\n",
               N, norm, sqrt(check_global));

    free(v);
    MPI_Finalize();
    return 0;
}
