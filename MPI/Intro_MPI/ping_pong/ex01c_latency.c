/*
 * ╔══════════════════════════════════════════════════════════╗
 * ║  Lab MPI  ·  Esercizio 1B  ·  Latenza verso tutti        ║
 * ╚══════════════════════════════════════════════════════════╝
 *
 * PUNTO DI PARTENZA: ex01a_pingpong.c
 * ────────────────────────────────────
 * Avete visto il ping-pong tra 2 processi. Ora generalizzate:
 * il processo 0 misura la latenza (messaggio da 1 byte, NREP rip.)
 * verso OGNI altro processo, e stampa una tabella comparativa.
 *
 * VARIANTE
 * ────────
 * • Funziona con qualsiasi numero di processi ≥ 2
 * • P0 esegue un ping-pong con P1, poi con P2, poi con P3, ...
 * • Ogni Pk (k ≥ 1) quando riceve fa echo e aspetta il prossimo
 *   ping (non sa in anticipo quando arriverà il suo turno)
 * • P0 stampa la tabella: rank destinazione | latenza (µs)
 *
 * SCHEMA (con 4 processi)
 * ───────────────────────
 *   Round k=1:  P0 ↔ P1  (misura lat_1)
 *   Round k=2:  P0 ↔ P2  (misura lat_2)
 *   Round k=3:  P0 ↔ P3  (misura lat_3)
 *   P0 stampa tabella.
 *   P1, P2, P3: ognuno ha fatto esattamente 1 Send + 1 Recv.
 *
 * HINT
 * ────
 * • Partite dalla struttura del loop di ex01a, ma il loop esterno
 *   è su 'dest' (1..size-1), non su msg_size.
 * • I worker (rank ≥ 1) eseguono un singolo Recv+Send ciascuno.
 *   Non sanno da chi arriva il ping — usate src=0 (è sempre P0).
 * • Usate un singolo byte (char buf = 'x') come messaggio.
 * • Tagliate NREP a 50 per tenere il laboratorio veloce.
 *
 * ESEGUIRE:
 *   mpicc -Wall -O2 -o lat_b ex01b_latency.c
 *   mpirun -np 6 ./lat_b
 *
 * OUTPUT ATTESO (valori indicativi su localhost):
 *   Latenze da rank 0 verso tutti gli altri:
 *   dest=1  lat=  3.21 µs
 *   dest=2  lat=  3.45 µs
 *   ...
 */

#include <mpi.h>
#include <stdio.h>
#include <float.h>

#define NREP 50

int main(int argc, char **argv)
{
    int  rank, size;
    char buf = 'x';     /* un singolo byte è sufficiente per misurare α */

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) fprintf(stderr, "Servono almeno 2 processi.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* ================================================================
     * PROCESSO 0: loop su tutti i destinatari
     * ================================================================ */
    if (rank == 0)
    {
        printf("Latenze da rank 0 verso tutti gli altri:\n");

        /* TODO 1: loop for dest = 1, 2, ..., size-1
         *
         * Per ogni dest:
         *   a) rtt_min = DBL_MAX
         *   b) loop NREP volte:
         *        MPI_Barrier
         *        t_start = MPI_Wtime()
         *        MPI_Send  (&buf, 1, MPI_BYTE, dest, 0, comm)
         *        MPI_Recv  (&buf, 1, MPI_BYTE, dest, 0, comm, IGNORE)
         *        t_end = MPI_Wtime()
         *        rtt = (t_end - t_start) * 1e6
         *        aggiornare rtt_min
         *   c) stampare: "dest=%d  lat=%6.2f µs\n", dest, rtt_min/2
         */

    }

    /* ================================================================
     * PROCESSI 1..size-1: echo singolo per ogni round
     * ================================================================ */
    else
    {
        /* TODO 2: ogni worker esegue NREP volte il ciclo:
         *   MPI_Barrier
         *   MPI_Recv  (&buf, 1, MPI_BYTE, 0, 0, comm, IGNORE)
         *   MPI_Send  (&buf, 1, MPI_BYTE, 0, 0, comm)
         *
         * ATTENZIONE: il worker non sa quale round sta eseguendo —
         * fa semplicemente Recv+Send per NREP volte, qualunque
         * sia la dimensione o il dest corrente di P0.
         * Ma P0 fa solo 1 ping con questo worker per tutti i NREP,
         * poi passa al prossimo. Come sincronizzate?
         *
         * HINT: anche i worker devono eseguire il loop dest esterno,
         * ma solo per rispettare le MPI_Barrier che P0 usa.
         * Struttura:
         *   for dest = 1..size-1:
         *     for rep = 0..NREP-1:
         *       MPI_Barrier
         *       if (rank == dest):
         *           Recv poi Send
         *       // se rank != dest: solo la Barrier, nient'altro
         */

    }

    MPI_Finalize();
    return 0;
}
