/*
 * ╔══════════════════════════════════════════════════════════╗
 * ║  Lab Ibrido  ·  Esercizio 01B  ·  Stencil 1D            ║
 * ╚══════════════════════════════════════════════════════════╝
 *
 * PUNTO DI PARTENZA: ex01a_stencil.c
 * ────────────────────────────────────
 * In 01A lo stencil calcolava la media di 3 elementi vicini.
 *
 * VARIANTE: stencil a 5 punti
 * ────────────────────────────
 * Invece della media a 3 punti, usate la media a 5 punti:
 *
 *   u_new[i] = (u[i-2] + u[i-1] + u[i] + u[i+1] + u[i+2]) / 5.0
 *
 * Questo richiede 2 ghost cells per lato invece di 1.
 * Il halo exchange deve inviare i 2 elementi di bordo.
 *
 * SCHEMA:
 *   [gl2 gl1 | u[1] u[2] ... u[CHUNK] | gr1 gr2]
 *    ↑   ↑                               ↑   ↑
 *    ghost cells sx (2)          ghost cells dx (2)
 *
 * ATTENZIONE:
 *   - il loop interno parte da i=3 e va fino a CHUNK-2
 *     (i primi e ultimi 2 elementi dipendono da ghost cells
 *      che non avete ancora — semplificate ignorandoli
 *      oppure aggiungete ghost cells anche per quei bordi)
 *   - in questa versione semplificata: loop da i=3 a CHUNK-2
 *
 * COMPILARE:
 *   mpicc -Wall -O2 -fopenmp -o stencil_b ex01b_stencil.c
 * ESEGUIRE:
 *   OMP_NUM_THREADS=4 mpirun -np 2 ./stencil_b
 */

#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNK  (1 << 20)
#define NSTEP  10

int main(int argc, char **argv)
{
    int rank, size, provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int left  = (rank - 1 + size) % size;
    int right = (rank + 1) % size;

    /* Array con 4 ghost cells: 2 a sinistra, 2 a destra
     * Indici: 0,1 = ghost sx | 2..CHUNK+1 = dominio | CHUNK+2,CHUNK+3 = ghost dx */
    double *u     = calloc(CHUNK + 4, sizeof(double));
    double *u_new = calloc(CHUNK + 4, sizeof(double));

    for (int i = 2; i <= CHUNK + 1; i++)
        u[i] = (double)rank;

    for (int step = 0; step < NSTEP; step++)
    {
        /* TODO 1: Halo exchange — inviare 2 elementi di bordo.
         *   Sinistra: inviare u[2],u[3] al vicino left,
         *             ricevere in u[CHUNK+2],u[CHUNK+3] da right.
         *   Destra:   inviare u[CHUNK],u[CHUNK+1] al vicino right,
         *             ricevere in u[0],u[1] da left.
         *
         *   MPI_Sendrecv(&u[2], 2, MPI_DOUBLE, left, 0,
         *                &u[CHUNK+2], 2, MPI_DOUBLE, right, 0,
         *                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         *   (e speculare per l'altro lato)
         */


        /* TODO 2: Stencil a 5 punti con OpenMP.
         *   Loop da i=2 a i=CHUNK+1 (dominio completo).
         *   u_new[i] = (u[i-2]+u[i-1]+u[i]+u[i+1]+u[i+2]) / 5.0
         *
         *   #pragma omp parallel for schedule(static)
         */


        /* TODO 3: Swap dei buffer. */
    }

    double local_min = u[2], local_max = u[2];
    for (int i = 3; i <= CHUNK + 1; i++) {
        if (u[i] < local_min) local_min = u[i];
        if (u[i] > local_max) local_max = u[i];
    }
    printf("rank %d: min=%.4f  max=%.4f\n", rank, local_min, local_max);

    free(u); free(u_new);
    MPI_Finalize();
    return 0;
}
