/*
 * =============================================================
 * LAB 5 — Profiling con gperftools
 * =============================================================
 *
 * OBIETTIVO:
 *   Usare gperftools per profilare solo la sezione critica
 *   del codice (ProfilerStart/Stop), senza root e su Mac.
 *
 * INSTALLAZIONE:
 *   Linux:  sudo apt install google-perftools libgoogle-perftools-dev
 *   Mac:    brew install gperftools
 *
 * COMPILAZIONE:
 *   gcc  -O2 -g -o gperftools_target gperftools_target.c -lprofiler -lm
 *   clang -O2 -g -o gperftools_target gperftools_target.c -lprofiler -lm
 *
 *   Se la libreria non è trovata:
 *     gcc -O2 -g -o gperftools_target gperftools_target.c \
 *         -I$(brew --prefix gperftools)/include \
 *         -L$(brew --prefix gperftools)/lib -lprofiler -lm
 *
 * ESECUZIONE:
 *   ./gperftools_target
 *   # Genera: matmul.prof  (profilo del kernel critico)
 *   # Genera: full.prof    (profilo dell'intera esecuzione)
 *
 * ANALISI CON pprof:
 *   pprof --text  ./gperftools_target matmul.prof
 *   pprof --text  ./gperftools_target full.prof
 *
 *   pprof --pdf   ./gperftools_target matmul.prof > matmul.pdf
 *   pprof --svg   ./gperftools_target matmul.prof > matmul.svg
 *   pprof --web   ./gperftools_target matmul.prof   # browser (richiede graphviz)
 *
 * LAB 5A — LD_PRELOAD (senza modificare il sorgente):
 *   Linux:
 *     CPUPROFILE=ldpreload.prof \
 *     LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libprofiler.so \
 *     ./gperftools_target
 *   Mac:
 *     CPUPROFILE=ldpreload.prof \
 *     DYLD_INSERT_LIBRARIES=$(brew --prefix gperftools)/lib/libprofiler.dylib \
 *     ./gperftools_target
 *   Poi: pprof --text ./gperftools_target ldpreload.prof
 *
 * DOMANDE GUIDA:
 *   - matmul.prof vs full.prof: quali funzioni cambiano %?
 *   - Confronta pprof con perf report (se su Linux): stesso hotspot?
 *   - Con -O0 vs -O2: come cambiano le percentuali?
 * =============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <gperftools/profiler.h>

/* ── Parametri ────────────────────────────────────────────── */
#define M    512
#define K    512
#define P    512
#define REPS 5
#define VN   5000000

/* ── Allocazione globale ──────────────────────────────────── */
static double A[M][K], B[K][P], C[M][P];
static double gx[VN], gy[VN];

/* ------------------------------------------------------------- *
 * KERNEL 1 — Matrix multiply naive (cache-unfriendly su B)
 * Sarà l'hotspot principale nel profilo.
 * ------------------------------------------------------------- */
void matmul_naive(void)
{
    for (int i = 0; i < M; i++)
        for (int j = 0; j < P; j++) {
            double s = 0.0;
            for (int k = 0; k < K; k++)
                s += A[i][k] * B[k][j];
            C[i][j] = s;
        }
}

/* ------------------------------------------------------------- *
 * KERNEL 2 — Dot product (sequenziale, cache-friendly)
 * Dovrebbe apparire molto meno di matmul nel profilo.
 * ------------------------------------------------------------- */
double dot_product(void)
{
    double sum = 0.0;
    for (int i = 0; i < VN; i++)
        sum += gx[i] * gy[i];
    return sum;
}

/* ------------------------------------------------------------- *
 * FUNZIONE DI SETUP — non profilata (fuori da ProfilerStart)
 * Questo è il vantaggio di ProfilerStart/Stop:
 * l'inizializzazione non inquina il profilo.
 * ------------------------------------------------------------- */
static void initialize(void)
{
    for (int i = 0; i < M; i++)
        for (int k = 0; k < K; k++)
            A[i][k] = (double)(i + k) / (M * K);

    for (int k = 0; k < K; k++)
        for (int j = 0; j < P; j++)
            B[k][j] = (double)(k + j) / (K * P);

    for (int i = 0; i < VN; i++) {
        gx[i] = (double)i / VN;
        gy[i] = 1.0 - gx[i];
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
    /* Inizializzazione — NON profilata */
    initialize();

    double t0, elapsed;

    /* =========================================================
     * PROFILO 1: solo il kernel critico (matmul + dot)
     *
     * ProfilerStart apre il file di output e inizia a campionare.
     * ProfilerStop ferma il campionamento e flushes su disco.
     * Tutto il codice fuori da Start/Stop non viene conteggiato.
     * ========================================================= */
    printf("Profilazione sezione critica → matmul.prof\n");
    ProfilerStart("matmul.prof");

    t0 = wtime();
    for (int r = 0; r < REPS; r++)
        matmul_naive();
    double dp = dot_product();
    elapsed = wtime() - t0;

    ProfilerStop();
    printf("  tempo: %.3f s  dot=%.6f\n\n", elapsed, dp);

    /* =========================================================
     * PROFILO 2: intero programma (incluso re-init e variante)
     *
     * Confronta con matmul.prof per vedere l'effetto del rumore
     * dell'inizializzazione e di funzioni meno costose.
     * ========================================================= */
    printf("Profilazione programma completo → full.prof\n");
    ProfilerStart("full.prof");

    initialize();   /* ora anche questo viene profilato */
    t0 = wtime();
    for (int r = 0; r < REPS; r++)
        matmul_naive();
    dp = dot_product();
    elapsed = wtime() - t0;

    ProfilerStop();
    printf("  tempo: %.3f s  dot=%.6f\n\n", elapsed, dp);

    /* checksum */
    double chk = 0.0;
    for (int i = 0; i < M; i++)
        for (int j = 0; j < P; j++)
            chk += C[i][j];
    printf("checksum: %.6e\n\n", chk);

    printf("Analisi:\n");
    printf("  pprof --text ./gperftools_target matmul.prof\n");
    printf("  pprof --text ./gperftools_target full.prof\n");
    printf("  pprof --svg  ./gperftools_target matmul.prof > matmul.svg\n");

    return 0;
}
