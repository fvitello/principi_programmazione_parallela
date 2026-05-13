/*
 * demo_memoria_privata.c
 *
 * Dimostra che i processi MPI NON condividono la memoria.
 * Il processo 0 modifica x, ma gli altri non vedono la modifica.
 *
 * Compilare:  mpicc -O0 -o demo demo_memoria_privata.c
 * Eseguire:   mpirun -np 4 ./demo
 *
 * Output atteso:
 *   rank 0: x = 42   ← processo 0 ha scritto x
 *   rank 1: x = 0    ← processo 1 non ha visto nulla
 *   rank 2: x = 0
 *   rank 3: x = 0
 */

#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int rank, size;
    int x = 0;          /* ogni processo ha la SUA copia di x */

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Solo il processo 0 modifica x.
     * Con i thread (OpenMP) tutti vedrebbero x = 42.
     * Con i processi MPI ogni processo ha la propria copia — gli altri
     * non vengono mai a sapere di questa modifica. */
    if (rank == 0)
        x = 42;

    /* Piccola pausa per lasciar "completare" l'assegnazione —
     * non serve davvero, ma rende chiaro che non è un problema
     * di timing: x degli altri processi è fisicamente altrove. */
    printf("rank %d di %d: x = %d\n", rank, size, x);

    /* Per mandare x agli altri serve una comunicazione esplicita.
     * Decommentare le righe seguenti per vedere la differenza: */

    /*
    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    printf("rank %d dopo Bcast: x = %d\n", rank, x);
    */

    MPI_Finalize();
    return 0;
}
