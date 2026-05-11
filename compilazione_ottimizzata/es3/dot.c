/*
 * =============================================================
 * LAB 3 — Esplorazione con Godbolt Compiler Explorer
 * =============================================================
 *
 * OBIETTIVO:
 *   Leggere l'assembly generato e identificare le ottimizzazioni:
 *   vettorizzazione, FMA, loop unrolling, inlining.
 *
 * ISTRUZIONI GODBOLT:
 *   1. Aprire https://godbolt.org
 *   2. Selezionare linguaggio: C
 *   3. Incollare il codice qui sotto (o solo le funzioni)
 *   4. Scegliere compilatore: x86-64 gcc (es. 13.2) o clang
 *   5. Aggiungere flag nel campo "Compiler options"
 *
 * ESPERIMENTI (in ordine):
 *
 *   A) gcc -O2
 *      → Cercate: vmovsd, vfmadd213sd, xmm*, ymm*
 *      → Il loop usa istruzioni scalari o SIMD?
 *
 *   B) gcc -O2  poi gcc -O3
 *      → Con -O3 appaiono istruzioni ymm* (AVX, 4 double)?
 *      → Quante istruzioni ha il loop con -O2 vs -O3?
 *
 *   C) Rimuovere "restrict" da dot_r e ricompilare con -O2
 *      → Il compilatore vettorizza ancora?
 *      → Compaiono load ridondanti prima/dopo ogni store?
 *
 *   D) gcc -O3 -Ofast
 *      → Compaiono istruzioni vfmadd213pd (FMA, fused multiply-add)?
 *      → Questo è più veloce ma non-IEEE 754: perché?
 *
 *   E) Cambiare compilatore: clang 17 con -O3
 *      → L'assembly è diverso? Più compatto?
 *      → Clang usa una struttura di loop diversa?
 *
 * NOTA MAC (locale):
 *   clang -O2 -o dot dot.c && ./dot
 *   clang -O3 -S -o dot.s dot.c   (assembly in Intel syntax: -masm=intel)
 * =============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N    1000000
#define REPS 50

/* ------------------------------------------------------------- *
 * dot_r — prodotto scalare CON restrict
 *
 * Da incollare su Godbolt per gli esperimenti A–E.
 * restrict dice al compilatore che a e b non si sovrappongono.
 * ------------------------------------------------------------- */
double dot_r(const double * restrict a,
             const double * restrict b,
             int n)
{
    double sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += a[i] * b[i];
    return sum;
}

/* ------------------------------------------------------------- *
 * dot — prodotto scalare SENZA restrict
 *
 * Da confrontare con dot_r su Godbolt (esperimento C).
 * ------------------------------------------------------------- */
double dot(const double *a, const double *b, int n)
{
    double sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += a[i] * b[i];
    return sum;
}

/* ------------------------------------------------------------- *
 * dot_manual — vettorizzazione manuale con accumulatori separati
 *
 * Tecnica: 4 accumulatori riduce le dipendenze di dati (RAW)
 * e permette al processore di eseguire più operazioni in pipeline.
 * Confrontare su Godbolt: produce più istruzioni ma è più veloce?
 * ------------------------------------------------------------- */
double dot_manual(const double * restrict a,
                  const double * restrict b,
                  int n)
{
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    int i;
    for (i = 0; i <= n - 4; i += 4) {
        s0 += a[i+0] * b[i+0];
        s1 += a[i+1] * b[i+1];
        s2 += a[i+2] * b[i+2];
        s3 += a[i+3] * b[i+3];
    }
    for (; i < n; i++) s0 += a[i] * b[i];   /* resto */
    return s0 + s1 + s2 + s3;
}

/* ---- utilità ---- */
static double wtime(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(void)
{
    double *a = malloc(N * sizeof(double));
    double *b = malloc(N * sizeof(double));
    if (!a || !b) { perror("malloc"); return 1; }

    for (int i = 0; i < N; i++) {
        a[i] = (double)i / N;
        b[i] = (double)(N - i) / N;
    }

    /* warm-up */
    volatile double discard = dot_r(a, b, N);
    (void)discard;

    /* benchmark */
    double t0;
    double result;

    t0 = wtime();
    for (int r = 0; r < REPS; r++) result = dot(a, b, N);
    printf("dot        (no restrict): %.4f s  result=%.6f\n",
           (wtime()-t0)/REPS, result);

    t0 = wtime();
    for (int r = 0; r < REPS; r++) result = dot_r(a, b, N);
    printf("dot_r      (restrict)   : %.4f s  result=%.6f\n",
           (wtime()-t0)/REPS, result);

    t0 = wtime();
    for (int r = 0; r < REPS; r++) result = dot_manual(a, b, N);
    printf("dot_manual (4 accum.)   : %.4f s  result=%.6f\n",
           (wtime()-t0)/REPS, result);

    /*
     * Risultato atteso:
     *   dot_r e dot_manual spesso simili o dot_manual leggermente
     *   più veloce su architetture con pipeline numerica profonda.
     *   La differenza è più evidente con -O2 che con -O3.
     */

    free(a); free(b);
    return 0;
}
