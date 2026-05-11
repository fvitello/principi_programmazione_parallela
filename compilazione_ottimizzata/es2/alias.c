/*
 * =============================================================
 * LAB 2 — Memory Aliasing e qualificatore restrict
 * =============================================================
 *
 * OBIETTIVO:
 *   Verificare sperimentalmente il guadagno di restrict e
 *   confrontare il codice assembly scalare vs. vettorizzato.
 *
 * COMPILAZIONE:
 *   gcc -O2              -o alias alias.c
 *   gcc -O2 -fopt-info-vec -o alias_vec alias.c 2>&1 | grep vectorized
 *   gcc -O2 -S -fverbose-asm -o alias.s alias.c
 *
 * ASSEMBLY — cercare le differenze:
 *   grep -A5 "scale_r:"  alias.s    # versione restrict
 *   grep -A5 "scale:"    alias.s    # versione senza restrict
 *   grep -c "ymm"        alias.s    # conta istruzioni AVX
 *
 * DOMANDE GUIDA:
 *   1. Con -O2 e -fopt-info-vec, quale funzione riceve
 *      il messaggio "vectorized"?
 *   2. Nell'assembly, scale_r usa ymm* (AVX, 4 double/iteraz.)?
 *      scale invece usa solo xmm* o mov scalare?
 *   3. Quanto è più veloce scale_r rispetto a scale?
 *   4. Con -fno-strict-aliasing la differenza sparisce?
 *   5. Con -O3 restrict è ancora necessario?
 *
 * NOTA MAC (clang):
 *   clang -O2 -o alias alias.c && ./alias
 *   clang -O2 -Rpass=loop-vectorize alias.c   # come -fopt-info-vec
 * =============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N     10000000   /* 10 milioni di double — ben oltre la L3 cache */
#define REPS  10

/* ------------------------------------------------------------- *
 * Versione SENZA restrict
 *
 * Il compilatore non sa se a[] e b[] si sovrappongono in memoria.
 * Deve assumere che a[i] e b[i] potrebbero essere la stessa cella
 * → non può vettorizzare → accessi scalari uno per volta.
 * ------------------------------------------------------------- */
void scale(double *a, const double *b, double s, int n)
{
    for (int i = 0; i < n; i++)
        a[i] = s * b[i];
}

/* ------------------------------------------------------------- *
 * Versione CON restrict  (C99)
 *
 * restrict = promessa al compilatore: "a e b non si sovrappongono".
 * Il compilatore può vettorizzare liberamente con istruzioni
 * AVX/SSE: elabora 4 double per iterazione invece di 1.
 *
 * RISULTATO ATTESO: 2×–4× più veloce di scale()
 * ------------------------------------------------------------- */
void scale_r(double * restrict a, const double * restrict b,
             double s, int n)
{
    for (int i = 0; i < n; i++)
        a[i] = s * b[i];
}

/* ------------------------------------------------------------- *
 * Versione con copia locale — alternativa a restrict:
 * leggere b[i] in un temporaneo "rompe" l'aliasing visibile
 * al compilatore su alcune versioni/configurazioni.
 * Confronta con scale_r: stesso assembly?
 * ------------------------------------------------------------- */
void scale_tmp(double *a, const double *b, double s, int n)
{
    for (int i = 0; i < n; i++) {
        double tmp = b[i];   /* carica b[i] in registro */
        a[i] = s * tmp;
    }
}

/* ---- utilità ---- */
static double wtime(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static double bench(void (*fn)(double*, const double*, double, int),
                    double *a, const double *b, double s, int n,
                    int reps)
{
    fn(a, b, s, n);                  /* warm-up */
    double t0 = wtime();
    for (int r = 0; r < reps; r++)
        fn(a, b, s, n);
    return (wtime() - t0) / reps;
}

int main(void)
{
    double *a = malloc(N * sizeof(double));
    double *b = malloc(N * sizeof(double));
    if (!a || !b) { perror("malloc"); return 1; }

    for (int i = 0; i < N; i++) b[i] = (double)i / N;

    const double s = 2.71828;

    double t_scale   = bench(scale,     a, b, s, N, REPS);
    double t_scale_r = bench(scale_r,   a, b, s, N, REPS);
    double t_scale_t = bench(scale_tmp, a, b, s, N, REPS);

    printf("scale     (no restrict): %.4f s\n", t_scale);
    printf("scale_r   (restrict)   : %.4f s  speedup vs scale: %.2fx\n",
           t_scale_r, t_scale / t_scale_r);
    printf("scale_tmp (local copy) : %.4f s  speedup vs scale: %.2fx\n",
           t_scale_t, t_scale / t_scale_t);

    /*
     * Risultati attesi con gcc -O2 su Linux x86-64 con AVX2:
     *   scale     : ~0.025–0.040 s  (scalare, un double per volta)
     *   scale_r   : ~0.008–0.015 s  (vettorizzato, 4 double AVX)
     *   speedup   : 2×–4×
     *
     * Con -fno-strict-aliasing:
     *   scale e scale_r hanno prestazioni simili
     *   (il compilatore rinuncia all'ottimizzazione per sicurezza)
     *
     * Su Mac con clang -O2:
     *   risultati analoghi ma con istruzioni SSE/AVX del chip Apple M-series
     */

    /* verifica correttezza: confronta output delle tre versioni */
    double *ref = malloc(N * sizeof(double));
    scale(ref, b, s, N);
    scale_r(a, b, s, N);
    double diff = 0.0;
    for (int i = 0; i < N; i++) diff += (a[i] - ref[i]) * (a[i] - ref[i]);
    printf("diff scale vs scale_r: %.2e (deve essere 0)\n", diff);

    free(a); free(b); free(ref);
    return 0;
}
