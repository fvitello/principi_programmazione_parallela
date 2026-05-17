/*
 * ESEMPIO 00b: HELLO WORLD IN MPI (VARIANTE)
 *
 * PUNTO DI PARTENZA
 * ─────────────────
 * Avete appena visto ex00a_hello.c. La struttura è identica.
 * Qui completate voi i TODO.
 *
 * VARIANTE RISPETTO A 0A
 * ──────────────────────
 * Invece di root/worker generici, il programma deve:
 *
 *   - Se size < 2 → il processo 0 stampa un errore e termina
 *     (usate MPI_Abort per terminare tutti i processi)
 *
 *   - Processo 0 (root) stampa quanti processi ci sono e
 *     annuncia: "Aspetto %d risultati" (size-1 worker)
 *
 *   - Ogni altro processo stampa il proprio rank E se il
 *     proprio rank è PARI o DISPARI
 *     es. "[rank=2] sono PARI"  oppure  "[rank=3] sono DISPARI"
 *
 * ESEGUIRE CON ALMENO 4 PROCESSI:
 *   mpicc -Wall -O2 -o hello_b ex00b_hello.c
 *   mpirun -np 6 ./hello_b
 *
 * OUTPUT ATTESO (ordine può variare):
 *   [ROOT rank=0] Ci sono 6 processi. Aspetto 5 risultati.
 *   [rank=1] sono DISPARI
 *   [rank=2] sono PARI
 *   [rank=3] sono DISPARI
 *   [rank=4] sono PARI
 *   [rank=5] sono DISPARI
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int rank, size;

    /* TODO 1: MPI_Init */


    /* TODO 2: MPI_Comm_rank e MPI_Comm_size */


    /* TODO 3: Controllo — se size < 2, il processo 0 stampa errore
     *         e tutti i processi chiamano MPI_Abort.
     *
     *  MPI_Abort(MPI_COMM_WORLD, codice_errore)
     *  termina immediatamente TUTTI i processi del comunicatore.
     *  Usare dopo aver stampato il messaggio di errore.
     *
     *  Struttura:
     *    if (size < 2) {
     *        if (rank == 0)
     *            fprintf(stderr, "...\n");
     *        MPI_Abort(MPI_COMM_WORLD, 1);
     *    }
     */


    /* TODO 4: Logica applicativa
     *   - rank 0 → stampa il messaggio del root
     *   - altri  → stampa rank e parità (rank % 2 == 0 → PARI)
     */


    /* TODO 5: MPI_Finalize */


    return 0;
}
