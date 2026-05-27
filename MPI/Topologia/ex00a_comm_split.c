/*
 * Esercizio 00A  ·  MPI_Comm_split       
 *
 * COSA IMPARIAMO
 * ──────────────
 * MPI_Comm_split divide i processi di un communicator in
 * sottogruppi. Ogni sottogruppo ha il proprio communicator
 * indipendente. Le collettive su quel communicator coinvolgono
 * solo i processi del sottogruppo.
 *
 * SCENARIO
 * ────────
 * 8 processi disposti su una griglia 2×4 (2 righe, 4 colonne).
 * Vogliamo che ogni RIGA faccia una somma indipendente.
 *
 *   rank 0  rank 1  rank 2  rank 3   ← riga 0
 *   rank 4  rank 5  rank 6  rank 7   ← riga 1
 *
 * Con MPI_COMM_WORLD MPI_Reduce sommerebbe tutti e 8.
 * Con MPI_Comm_split creiamo un communicator per ogni riga
 * e facciamo Reduce solo all'interno della riga.
 *
 * COMPILARE:  mpicc -Wall -O2 -o comm_a ex00a_comm_split.c
 * ESEGUIRE:   mpirun -np 8 ./comm_a
 */

#include <mpi.h>
#include <stdio.h>

#define NCOLS 4   /* processi per riga */

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 8) {
        if (rank == 0) fprintf(stderr, "Servono esattamente 8 processi.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* ── Calcolare riga e colonna ────────────────────────────────
     * Con NCOLS=4:
     *   rank 0 → riga 0, col 0
     *   rank 1 → riga 0, col 1
     *   rank 4 → riga 1, col 0
     *   rank 7 → riga 1, col 3  */
    int row = rank / NCOLS;
    int col = rank % NCOLS;

    /* ── MPI_Comm_split ──────────────────────────────────────────
     * Firma: MPI_Comm_split(comm, color, key, newcomm)
     *
     * color: processi con lo stesso color finiscono nello stesso
     *        sottogruppo. Usiamo 'row' come color → tutti i
     *        processi della stessa riga hanno lo stesso color.
     *
     * key:   determina l'ordine dei rank nel nuovo communicator.
     *        Usare con 'col' il rank nel nuovo
     *        communicator coincide con la colonna.
     *
     * newcomm: il nuovo communicator per questo sottogruppo. */
    MPI_Comm row_comm;
    MPI_Comm_split(MPI_COMM_WORLD, row, col, &row_comm);

    /* Rank nel nuovo communicator */
    int row_rank, row_size;
    MPI_Comm_rank(row_comm, &row_rank);
    MPI_Comm_size(row_comm, &row_size);

    printf("rank %d: riga=%d col=%d  →  row_rank=%d (di %d)\n",
           rank, row, col, row_rank, row_size);

    /* ── Riduzione per riga ──────────────────────────────────────
     * Ogni processo contribuisce con il proprio rank globale.
     * MPI_Reduce su row_comm somma solo i processi della stessa
     * riga — non tutti gli 8 processi di COMM_WORLD. */
    double local_val = (double)rank;
    double row_sum   = 0.0;
    MPI_Reduce(&local_val, &row_sum, 1, MPI_DOUBLE,
               MPI_SUM, 0, row_comm);

    /* Il root di ogni riga (row_rank==0) stampa la somma */
    if (row_rank == 0)
        printf("riga %d: somma rank = %.0f  (atteso %.0f)\n",
               row,
               row_sum,
               row == 0 ? 0+1+2+3.0 : 4+5+6+7.0);

    /* ── Liberare il communicator ────────────────────────────────
     * I communicator custom vanno liberati con MPI_Comm_free.
     * MPI_COMM_WORLD non va mai liberato manualmente. */
    MPI_Comm_free(&row_comm);

    MPI_Finalize();
    return 0;
}
