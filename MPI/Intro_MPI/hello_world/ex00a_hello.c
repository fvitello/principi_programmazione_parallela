/*
 * ESEMPIO 00a: HELLO WORLD IN MPI
 * COSA IMPARIAMO
 * ──────────────
 * La struttura obbligatoria di ogni programma MPI:
 *   Init → operazioni MPI → Finalize
 * e come un processo scopre la propria identità nel mondo parallelo.
 *
 * COME ESEGUIRE
 * ─────────────
 *   mpicc -Wall -O2 -o hello_a ex00a_hello.c
 *   mpirun -np 4 ./hello_a
 */

#include <mpi.h>    /* header MPI — sempre necessario                  */
#include <stdio.h>

int main(int argc, char **argv)
{
    int rank;   /* chi sono io?         (0, 1, 2, … size-1)         */
    int size;   /* quanti siamo in totale?                           */

    /* ── 1. INIZIALIZZAZIONE ────────────────────────────────────────
     * MPI_Init DEVE essere la prima chiamata MPI del programma.
     * Avvia il runtime, stabilisce le connessioni tra i processi,
     * alloca le strutture dati interne.
     *
    * Passiamo &argc e &argv così MPI può rimuovere i propri
    * argomenti interni dalla riga di comando prima che il nostro
    * codice li legga. Su implementazioni moderne non è strettamente
    * necessario, ma è la forma corretta e portabile.
    * ────────────────────────────────────────────────────────────── */
 
    MPI_Init(&argc, &argv);

    /* ── 2. IDENTITÀ DEL PROCESSO ───────────────────────────────────
     * MPI_COMM_WORLD è il comunicatore predefinito: contiene TUTTI
     * i processi lanciati da mpirun. Ogni comunicatore ha un
     * namespace di rank separato (partono sempre da 0).
     *
     * Dopo questa chiamata, ogni processo ha un valore diverso di
     * 'rank': se abbiamo 4 processi, rank vale 0, 1, 2 oppure 3.
     * ────────────────────────────────────────────────────────────── */
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* ── 3. LOGICA APPLICATIVA ──────────────────────────────────────
     * Il pattern if (rank == 0) / else è il modo idiomatico per
     * distinguere un processo "coordinatore" dagli altri.
     * Processo 0 è per convenzione il root, ma non è obbligatorio.
     * ────────────────────────────────────────────────────────────── */
    if (rank == 0) {
        printf("[ROOT   rank=0] Ci sono %d processi in totale.\n", size);
    } else {
        printf("[WORKER rank=%d] In attesa di istruzioni dal root.\n", rank);
    }

    /* OSSERVAZIONE: eseguendo più volte, l'ordine delle righe cambia.
     * Ogni processo è uno schedulato indipendentemente dal SO.
     * Non c'è garanzia sull'ordine di esecuzione tra processi MPI. */

    /* ── 4. FINALIZZAZIONE ──────────────────────────────────────────
     * MPI_Finalize DEVE essere l'ultima chiamata MPI.
     * Libera le risorse interne e chiude le connessioni.
     * Tutti i processi devono chiamarla — nessuno escluso.
     * Qualsiasi comunicazione dopo questa chiamata è un errore.
     * ────────────────────────────────────────────────────────────── */
    MPI_Finalize();
    return 0;
}
