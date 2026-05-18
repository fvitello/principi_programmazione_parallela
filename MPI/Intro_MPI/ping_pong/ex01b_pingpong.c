/*
 *
 * ESEMPIO 01b: PING-PONG CON MPI
 * COSA IMPARIAMO
 * ──────────────
 * • MPI_Send / MPI_Recv bloccanti
 * • MPI_Wtime per misurare il tempo
 * • Il modello α-β su dati reali della nostra macchina
 *
 * SCHEMA
 * ──────
 *   P0 ──Send──► P1          t_start = MPI_Wtime()
 *   P0 ◄──Recv── P1    ×NREP (poi teniamo il minimo su NTRIALS misure)
 *
 *   RTT  = (t_end - t_start) / NREP * 1e6   [µs]
 *   α    = RTT(1 byte) / 2            [µs]   latenza one-way
 *   β    = size / (RTT/2 · 1e-6)     [MB/s] banda
 *
 * VERIFICA DEL MODELLO α-β
 * ─────────────────────────
 * Il modello predice che il tempo di una Send di dimensione N vale:
 *
 *   T = α + N/β
 *
 * dove α e β sono stimati dalle misure sui messaggi piccoli e grandi.
 * L'ultima colonna dell'output confronta T_misurato con T_previsto:
 * se il modello è buono, le due colonne devono essere vicine.
 *
 * ESEGUIRE:
 *   mpicc -Wall -O2 -o pp_a ex01a_pingpong.c
 *   mpirun -np 2 ./pp_a
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>      /* DBL_MAX */

#define NTRIALS 10              /* misure indipendenti per dimensione */
#define NREP    100             /* ping-pong consecutivi per misura   */
#define MAXSIZE (1 << 22)       /* messaggio massimo: 4 MB    */

int main(int argc, char **argv)
{
    int    rank, size;
    char  *buf;
    double t_start, t_end, rtt, rtt_min;

    /* Struttura per tenere i risultati di ogni dimensione */
    typedef struct { int msg; double rtt_min; } Row;
    int  nrows = 0, cap = 0;
    Row *rows  = NULL;

    MPI_Init(&argc, &argv);
        
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    printf("Risoluzione timer: %.2e s  (%.0f ns)\n",
           MPI_Wtick(), MPI_Wtick() * 1e9);

    if (size != 2) {
        if (rank == 0)
            fprintf(stderr, "Servono esattamente 2 processi.\n"
                            "Uso: mpirun -np 2 ./pp_a\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* Allocare e toccare il buffer per forzare l'allocazione fisica
     * delle pagine — evita che la prima iterazione misuri il fault
     * di pagina invece del tempo di comunicazione */
    buf = (char *)malloc(MAXSIZE);
    /* Tocca ogni byte per forzare l'allocazione fisica delle pagine
    * (lazy allocation: il kernel alloca RAM solo al primo accesso).
    * Valori non-zero evitano ottimizzazioni copy-on-write del kernel. */
    for (int i = 0; i < MAXSIZE; i++) buf[i] = (char)(i & 0xFF);

    /* ── PASSO 1: misurare RTT per ogni dimensione ───────────────── */
    /* Raddoppia la dimensione del messaggio ad ogni iterazione: */

    for (int msg = 1; msg <= MAXSIZE; msg *= 2)
    {
        rtt_min = DBL_MAX;

        for (int trial = 0; trial < NTRIALS; trial++)
        {
            /* Barriera prima di cronometrare: garantisce che entrambi i
             * processi siano pronti prima della misura. Non misuriamo un
             * singolo ping-pong, perché su una macchina locale può durare
             * meno della risoluzione del timer (per esempio 1 us).
             * Cronometriamo quindi NREP ping-pong consecutivi e dividiamo:
             * l'intervallo totale è più lungo e l'errore relativo del timer
             * diventa molto più piccolo. */
            MPI_Barrier(MPI_COMM_WORLD);
            t_start = MPI_Wtime();

            for (int rep = 0; rep < NREP; rep++) {
                if (rank == 0) {
                /* ── SEND bloccante: ritorna quando il buffer può essere riutilizzato */
                    MPI_Send(buf, msg, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
                                    /* ── RECV bloccante: blocca finché msg byte arrivano   */
                    MPI_Recv(buf, msg, MPI_BYTE, 1, 0,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                } else {
                /* P1: Recv PRIMA di Send — evita deadlock su messaggi
                 * grandi fuori dal buffer eager di MPI. */               
                    MPI_Recv(buf, msg, MPI_BYTE, 0, 0,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Send(buf, msg, MPI_BYTE, 0, 0, MPI_COMM_WORLD);

                }
            }

            t_end = MPI_Wtime();
            rtt = (t_end - t_start) / NREP * 1e6;  /* RTT medio in µs */


            /* Perché il minimo e non la media?
             * Il minimo approssima le condizioni meno disturbate: cache calde,
             * nessun interrupt del sistema operativo, poco jitter. La media
             * include eventi rari ma costosi (context switch, rumore di rete),
             * utili da osservare ma non rappresentativi del costo base della
             * comunicazione. */
            if (rtt < rtt_min) rtt_min = rtt;
        }

        /* Salviamo i risultati per il passo 2 */
        if (rank == 0) {
            if (nrows >= cap) {
                cap = cap ? cap * 2 : 32;
                rows = realloc(rows, cap * sizeof(Row));
            }
            rows[nrows].msg     = msg;
            rows[nrows].rtt_min = rtt_min;
            nrows++;
        }
    }

    /* ── PASSO 2: stimare α e β, poi verificare T = α + size/β ─────── */
    if (rank == 0)
    {
        /* α: latenza one-way = RTT del messaggio più piccolo / 2
         * Con 1 byte il tempo è quasi tutto latenza, quasi niente
         * trasferimento dati. */
        double alpha_us = rows[0].rtt_min / 2.0;

        /* β: banda stimata dal messaggio più grande.
         * Con N grande α è trascurabile rispetto a N/β, quindi:
         *   T(N) = α + N/β  ≈  N/β
         *   RTT  = 2 * T(N) ≈  2 * N/β
         *   β    = N / (RTT/2)
         */
        double last_size = rows[nrows-1].msg;
        double last_rtt  = rows[nrows-1].rtt_min;
        double beta_Bps  = last_size / (last_rtt / 2.0 * 1e-6);
        double beta_MBs  = beta_Bps / 1048576.0;

        printf("\n=== Parametri del modello α-β ===\n");
        printf("  α (latenza)  = %.2f µs\n", alpha_us);
        printf("  β (banda)    = %.1f MB/s\n\n", beta_MBs);

        printf("%-10s  %-10s  %-12s  %-12s  %-12s  %s\n",
               "Size [B]", "RTT [µs]", "T_mis [µs]",
               "Banda [MB/s]", "T_prev [µs]", "Errore");
        printf("%s\n", "----------------------------------------------------------------------");

        for (int i = 0; i < nrows; i++) {
            double N      = rows[i].msg;
            double rtt_u  = rows[i].rtt_min;
            double t_mis  = rtt_u / 2.0;          /* one-way misurato: RTT/2       */
            double bw_MBs = (N / 1048576.0)        /* byte → MB                     */
                        / (t_mis * 1e-6);        /* µs → s  →  MB/s               */

            /* T = α + N/β  in µs:
            *   alpha_us  in µs
            *   N/beta_Bps in secondi → *1e6 per convertire in µs               */
            double t_prev  = alpha_us + N / beta_Bps * 1e6;

            /* Errore relativo: positivo = misurato > previsto (modello ottimista)
            *                 negativo = misurato < previsto (modello pessimista) */
            double err_pct = (t_mis - t_prev) / t_prev * 100.0;

            printf("%-10d  %-10.2f  %-12.2f  %-12.1f  %-12.2f  %+.1f%%\n",
                rows[i].msg, rtt_u, t_mis, bw_MBs, t_prev, err_pct);
        }

        printf("\nLegenda:\n");
        printf("  T_mis  = RTT_min / 2     (tempo misurato one-way)\n");
        printf("  T_prev = α + size/β      (previsione del modello)\n");
        printf("  Errore = (T_mis-T_prev)/T_prev × 100\n");
        printf("\n");
        printf("  Interpretazione dell'errore:\n");
        printf("  +%%  → la comunicazione è più lenta del previsto\n");
        printf("  -%%  → la comunicazione è più veloce del previsto\n");
        printf("\n");
        printf("  Errore atteso:\n");
        printf("  • Messaggi piccoli  (< 100 B)  : errore basso, α domina\n");
        printf("  • Messaggi medi     (100B–1MB) : errore alto, zona di transizione\n");
        printf("                                   tra buffering interno e trasferimento\n");
        printf("                                   diretto — non modellata da α-β\n");
        printf("  • Messaggi grandi   (> 1MB)    : errore basso, β domina\n");
        printf("\n");
        printf("  Nota: α e β sono stimati dai messaggi più piccolo e più grande.\n");
        printf("        Il modello è esatto per definizione ai due estremi.\n");

        free(rows);
    }

    free(buf);
    MPI_Finalize();
    return 0;
}
