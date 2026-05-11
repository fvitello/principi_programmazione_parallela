/*
 * =============================================================
 * LAB 4 — Target per perf stat e perf record
 * =============================================================
 *
 * OBIETTIVO:
 *   Un programma con hotspot intenzionale per esercitare
 *   perf stat (conteggio) e perf record (campionamento).
 *
 * NOTA MAC:
 *   perf è disponibile solo su Linux.
 *   Su macOS usare Instruments (Xcode) oppure:
 *     - sample ./profiling_target 10   (campionamento BSD)
 *     - leaks  ./profiling_target       (analisi memoria)
 *   Le istruzioni perf nel lab funzionano su Linux/VM.
 *
 * COMPILAZIONE:
 *   gcc -O2 -g -o profiling_target profiling_target.c
 *   #            ↑ -g fondamentale per correlare campioni al sorgente
 *
 * USO CON perf (Linux):
 *
 *   # 1. Setup permessi (una volta sola):
 *   sudo bash -c 'echo -1 > /proc/sys/kernel/perf_event_paranoid'
 *   sudo bash -c 'echo 0  > /proc/sys/kernel/kptr_restrict'
 *
 *   # 2. perf stat — panoramica globale:
 *   perf stat ./profiling_target
 *   perf stat -r 5 ./profiling_target
 *   perf stat -e cycles,instructions,cache-misses,\
 *                L1-dcache-loads,LLC-load-misses ./profiling_target
 *
 *   # 3. perf record — trovare gli hotspot:
 *   perf record -g --call-graph dwarf ./profiling_target
 *   perf report --stdio --no-children
 *
 *   # 4. Flame Graph:
 *   git clone https://github.com/brendangregg/FlameGraph /tmp/fg
 *   perf script | /tmp/fg/stackcollapse-perf.pl \
 *              | /tmp/fg/flamegraph.pl > perf.svg
 *   firefox perf.svg
 *
 * DOMANDE GUIDA:
 *   - Quale funzione appare prima nel perf report?
 *   - Qual è il valore di IPC? (ideale: 2–4)
 *   - Qual è il % di cache miss LLC?
 *   - Il Flame Graph mostra chiaramente l'hotspot?
 * =============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* ── Parametri ────────────────────────────────────────────── */
#define M       512     /* righe matrice A */
#define K       512     /* colonne A / righe B */
#define P       512     /* colonne B */
#define REPS    5       /* ripetizioni moltiplicazione */
#define VN      8000000 /* dimensione vettori per dot product */

/* ── Matrici globali ──────────────────────────────────────── */
static double A[M][K], B[K][P], C[M][P];

/* ------------------------------------------------------------- *
 * HOTSPOT #1 — Moltiplicazione matrice-matrice naive
 *
 * Questo kernel ha cattivo accesso alla cache su B (column-major).
 * È intenzionalmente non-ottimizzato per avere un hotspot visibile.
 * perf report dovrebbe mostrarlo come funzione dominante.
 * ------------------------------------------------------------- */
void matmul_naive(void)
{
    for (int i = 0; i < M; i++)
        for (int j = 0; j < P; j++) {
            double s = 0.0;
            for (int k = 0; k < K; k++)
                s += A[i][k] * B[k][j];   /* B[k][j]: stride P → cache miss */
            C[i][j] = s;
        }
}

/* ------------------------------------------------------------- *
 * HOTSPOT #2 — Funzione "spesa poco" — dot product su array grande
 *
 * Contrasto: access pattern sequenziale, ottimo per la cache.
 * perf report dovrebbe mostrarla molto meno di matmul_naive.
 * ------------------------------------------------------------- */
static double gx[VN], gy[VN];

double dot_product(void)
{
    double sum = 0.0;
    for (int i = 0; i < VN; i++)
        sum += gx[i] * gy[i];
    return sum;
}

/* ------------------------------------------------------------- *
 * Funzione "rumore" — chiamata molte volte ma poco costosa
 * Serve a rendere il call graph più interessante.
 * ------------------------------------------------------------- */
static void do_work(int n)
{
    volatile double x = 1.0;
    for (int i = 0; i < n; i++)
        x = x * 1.0000001 + 0.0000001;
    (void)x;
}

static void initialize(void)
{
    for (int i = 0; i < M; i++)
        for (int k = 0; k < K; k++)
            A[i][k] = (double)(i * K + k) / (M * K);

    for (int k = 0; k < K; k++)
        for (int j = 0; j < P; j++)
            B[k][j] = (double)(k * P + j) / (K * P);

    for (int i = 0; i < VN; i++) {
        gx[i] = (double)i / VN;
        gy[i] = (double)(VN - i) / VN;
    }
}

static double wtime(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(void)
{
    initialize();

    double t0 = wtime();

    /* La maggior parte del tempo sarà qui */
    for (int r = 0; r < REPS; r++) {
        matmul_naive();
        do_work(1000);
    }

    /* Meno tempo qui */
    double dp = dot_product();

    double elapsed = wtime() - t0;

    /* checksum per evitare ottimizzazioni "troppo aggressive" */
    double chk = 0.0;
    for (int i = 0; i < M; i++)
        for (int j = 0; j < P; j++)
            chk += C[i][j];

    printf("matmul %dx%dx%d reps=%d + dot(N=%d)\n", M, K, P, REPS, VN);
    printf("total time : %.3f s\n", elapsed);
    printf("checksum   : %.6e\n", chk);
    printf("dot result : %.6f\n", dp);

    /*
     * ATTESO nel perf report:
     *   ~70–90%  matmul_naive       ← hotspot principale
     *   ~5–20%   dot_product
     *   ~1–5%    do_work
     *
     * Se matmul non domina, aumenta REPS o M/K/P.
     */
    return 0;
}
