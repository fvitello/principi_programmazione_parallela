/*
 * ╔══════════════════════════════════════════════════════════╗
 * ║  Lab MPI  ·  Esercizio 01C  ·  Latenza verso tutti      ║
 * ╚══════════════════════════════════════════════════════════╝
 *
 * PUNTO DI PARTENZA: ex01a_pingpong.c
 * ────────────────────────────────────
 * Il processo 0 misura la latenza (1 byte) verso ogni altro
 * processo e stampa una tabella comparativa.
 *
 * TECNICA DI MISURA
 * ─────────────────
 * Un singolo ping-pong è troppo veloce per il timer (1 µs in
 * locale). Soluzione: cronometrare NREP ping-pong consecutivi
 * in un unico blocco e dividere per NREP.
 *
 *   MPI_Barrier          ← sincronizza tutti prima di partire
 *   t_start = MPI_Wtime()
 *   for rep = 0..NREP-1:
 *       ping-pong
 *   RTT_medio = (MPI_Wtime() - t_start) * 1e6 / NREP   [µs]
 *   latenza   = RTT_medio / 2
 *
 * SCHEMA (con 4 processi)
 * ───────────────────────
 *   Round dest=1:  P0 ↔ P1  × NREP  (misura lat_1)
 *   Round dest=2:  P0 ↔ P2  × NREP  (misura lat_2)
 *   Round dest=3:  P0 ↔ P3  × NREP  (misura lat_3)
 *   P0 stampa la tabella.
 *
 * STRUTTURA DEL CODICE
 * ────────────────────
 * Tutti i processi eseguono il loop esterno su dest — non solo
 * P0 e il dest corrente — perché la MPI_Barrier è collettiva
 * e richiede partecipazione di tutti.
 *
 * Nel loop interno solo rank==0 e rank==dest comunicano.
 * Gli altri processi non fanno nulla in quel round.
 *
 * COMPILARE:  mpicc -Wall -O2 -o lat_b ex01c_latency.c
 * ESEGUIRE:   mpirun -np 6 ./lat_b
 *
 * OUTPUT ATTESO:
 *   Risoluzione timer: 1.00e-06 s
 *   Latenze da rank 0 verso tutti gli altri:
 *     dest=1    lat=   0.85 µs
 *     dest=2    lat=   0.90 µs
 *     ...
 */

#include <mpi.h>
#include <stdio.h>

#define NREP 1000   /* ping-pong consecutivi per ogni destinatario */

int main(int argc, char **argv)
{
    int  rank, size;
    char buf = 'x';   /* 1 byte — sufficiente per misurare α */

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) fprintf(stderr, "Servono almeno 2 processi.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        printf("Risoluzione timer: %.2e s\n", MPI_Wtick());
        printf("Latenze da rank 0 verso tutti gli altri:\n");
    }


        /* TODO 1: Loop esterno su dest = 1, 2, ..., size-1.
        * TUTTI i processi eseguono questo loop.
        *
        *   for dest = 1..size-1:
        *     MPI_Barrier     ← sincronizza tutti prima del round
        *     t_start = MPI_Wtime()   ← solo rank==0 lo usa
        */


        /* TODO 2: Loop interno su rep = 0..NREP-1.
         * Nessuna Barrier qui dentro — NREP ping-pong consecutivi
         * vengono cronometrati come un unico blocco.
         *
         *   if (rank == 0):
         *       Send(&buf, 1, MPI_BYTE, dest, 0, comm)
         *       Recv(&buf, 1, MPI_BYTE, dest, 0, comm, IGNORE)
         *   else if (rank == dest):
         *       Recv(&buf, 1, MPI_BYTE, 0, 0, comm, IGNORE)
         *       Send(&buf, 1, MPI_BYTE, 0, 0, comm)
         *   // altri processi: nulla
         */


        /* TODO 3: Solo rank==0 calcola e stampa il risultato.
         *
         *   RTT_medio = (MPI_Wtime() - t_start) * 1e6 / NREP
         *   printf("  dest=%-3d  lat=%7.2f µs\n", dest, RTT_medio/2.0)
         */


    MPI_Finalize();
    return 0;
}
