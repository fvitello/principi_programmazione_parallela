/*
 * ╔══════════════════════════════════════════════════════════╗
 * ║  Lab Ibrido  ·  Esercizio 01A  ·  Stencil 1D            ║
 * ║  Codice mostrato dal docente                             ║
 * ╚══════════════════════════════════════════════════════════╝
 *
 * COSA IMPARIAMO
 * ──────────────
 * Lo stencil 1D è il pattern fondamentale nelle simulazioni
 * numeriche su griglia (diffusione del calore, equazione delle
 * onde, ecc.). Ogni elemento dipende dai suoi vicini.
 *
 * Nel pattern ibrido:
 *   MPI:    scambia le ghost cells (halo exchange) tra processi
 *   OpenMP: parallelizza il calcolo interno del dominio
 *
 * La separazione è netta: prima halo exchange (MPI),
 * poi calcolo (OpenMP). Non si mescolano mai.
 *
 * SCHEMA
 * ──────
 * Ogni processo gestisce CHUNK elementi + 2 ghost cells:
 *
 *   [ghost_left | u[0] u[1] ... u[CHUNK-1] | ghost_right]
 *        ↑                                        ↑
 *   ricevuto dal                           ricevuto dal
 *   processo sinistra                      processo destra
 *
 * Stencil: u_new[i] = (u[i-1] + u[i] + u[i+1]) / 3
 *
 * COMPILARE:
 *   mpicc -Wall -O2 -fopenmp -o stencil_a ex01a_stencil.c
 * ESEGUIRE:
 *   OMP_NUM_THREADS=4 mpirun -np 2 ./stencil_a
 */

#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNK  (1 << 20)   /* elementi per processo */
#define NSTEP  10          /* passi temporali       */

int main(int argc, char **argv)
{
    int rank, size, provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Vicini nel ring periodico */
    int left  = (rank - 1 + size) % size;
    int right = (rank + 1) % size;

    /* Allocare il dominio locale con ghost cells */
    double *u     = malloc((CHUNK + 2) * sizeof(double));
    double *u_new = malloc((CHUNK + 2) * sizeof(double));

    /* Inizializzazione: valore = rank per identificare i blocchi */
    for (int i = 1; i <= CHUNK; i++)
        u[i] = (double)rank;
    u[0] = u[CHUNK + 1] = 0.0;   /* ghost cells inizialmente 0 */

    if (rank == 0)
        printf("Stencil 1D ibrido: %d proc × %d elem × %d thread × %d passi\n",
               size, CHUNK, omp_get_max_threads(), NSTEP);

    for (int step = 0; step < NSTEP; step++)
    {
        /* ── PASSO 1: halo exchange (MPI, thread principale) ─────
         * Inviare il bordo sinistro al vicino sinistra,
         * ricevere il bordo destro del vicino sinistra.
         * E viceversa per il lato destro.
         *
         * Questo deve avvenire FUORI dalla regione parallela
         * perché con THREAD_FUNNELED solo il thread principale
         * può chiamare MPI. */
        MPI_Sendrecv(&u[1],     1, MPI_DOUBLE, left,  0,
                     &u[CHUNK+1], 1, MPI_DOUBLE, right, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Sendrecv(&u[CHUNK], 1, MPI_DOUBLE, right, 1,
                     &u[0],     1, MPI_DOUBLE, left,  1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* ── PASSO 2: calcolo stencil (OpenMP) ───────────────────
         * Ora tutte le ghost cells sono aggiornate.
         * OpenMP parallelizza il calcolo su tutti gli elementi
         * interni — nessuna dipendenza tra iterazioni. */
        #pragma omp parallel for schedule(static)
        for (int i = 1; i <= CHUNK; i++)
            u_new[i] = (u[i-1] + u[i] + u[i+1]) / 3.0;

        /* Swap dei buffer */
        double *tmp = u; u = u_new; u_new = tmp;
    }

    /* Verifica: stampa min e max del dominio locale */
    double local_min = u[1], local_max = u[1];
    for (int i = 2; i <= CHUNK; i++) {
        if (u[i] < local_min) local_min = u[i];
        if (u[i] > local_max) local_max = u[i];
    }
    printf("rank %d: min=%.4f  max=%.4f  (dopo %d passi)\n",
           rank, local_min, local_max, NSTEP);

    free(u); free(u_new);
    MPI_Finalize();
    return 0;
}
