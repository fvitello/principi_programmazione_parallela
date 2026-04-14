#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/*
 * hello_omp_load.c
 *
 * Obiettivo didattico:
 * simulare un carico CPU "puro" con OpenMP per osservare facilmente
 * da top/htop come varia l'uso delle CPU al variare di:
 * - OMP_NUM_THREADS
 * - parametri Slurm (ntasks, cpus-per-task)
 *
 * Idea:
 * ogni thread entra in un busy-loop e continua a fare operazioni floating-point
 * fino a una scadenza temporale comune.
 *
 * Build:
 *   gcc -O2 -fopenmp hello_omp_load.c -o build/hello_omp_load
 *
 * Run:
 *   OMP_NUM_THREADS=4 ./build/hello_omp_load 20
 */
int main(int argc, char **argv) {
    /* Durata di default del carico (secondi). */
    int seconds = 20;

    /*
     * Se l'utente passa un argomento, lo interpretiamo come durata.
     */
    if (argc > 1) {
        seconds = atoi(argv[1]);
        if (seconds <= 0) {
            fprintf(stderr, "Uso: %s [secondi>0]\n", argv[0]);
            return 1;
        }
    }

    /*
     * Istante di fine condiviso:
     * omp_get_wtime() restituisce un tempo ad alta risoluzione (in secondi).
     */
    double t_end = omp_get_wtime() + (double)seconds;

    /*
     * Variabile accumulata da tutti i thread.
     * reduction(+:checksum) evita race condition e combina i contributi in uscita.
     */
    double checksum = 0.0;

    /*
     * Regione parallela OpenMP:
     * il numero di thread e' controllato da OMP_NUM_THREADS
     * (o da altre policy OpenMP/runtime).
     */
    #pragma omp parallel reduction(+:checksum)
    {
        /* ID logico del thread (0..num_threads-1). */
        int tid = omp_get_thread_num();

        /* Contatore locale di iterazioni, utile per output didattico. */
        unsigned long long iters = 0;

        /*
         * Stato locale del calcolo.
         * Inizializzazione diversa per thread, cosi' ogni thread ha una traiettoria
         * numerica leggermente differente.
         */
        double x = 1.0 + (double)tid;

        /*
         * Busy loop temporizzato:
         * continua finche' non raggiungiamo la scadenza globale.
         */
        while (omp_get_wtime() < t_end) {
            /*
             * Kernel computazionale:
             * molte operazioni floating-point per tenere occupate le unita' di calcolo.
             */
            for (int i = 0; i < 200000; ++i) {
                x = x * 1.0000001192092896 + 0.0000001;
                x = x / 1.0000000596046448;
            }
            iters += 200000ULL;
        }

        /*
         * Usiamo x e iters nel checksum per impedire che il compilatore
         * elimini il lavoro come "calcolo inutile".
         */
        checksum += x + (double)iters;

        /* Stampa per thread: utile per vedere che tutti hanno lavorato. */
        printf("thread %d: iterazioni=%llu\n", tid, iters);
    }

    /* Riepilogo finale del run. */
    printf("Fine carico (%d s). checksum=%f\n", seconds, checksum);
    return 0;
}
