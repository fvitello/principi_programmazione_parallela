/*
 * =============================================================
 * LAB 1A — Confronto livelli di ottimizzazione -O
 * =============================================================
 *
 * OBIETTIVO:
 *   Osservare come i flag -O0, -O1, -O2, -O3 e -Ofast
 *   modificano le prestazioni di un kernel HPC reale.
 *
 * COMPILAZIONE ED ESECUZIONE (vedi Makefile oppure manuale):
 *
 *   gcc -O0 -o matrix_O0 matrix.c && time ./matrix_O0
 *   gcc -O1 -o matrix_O1 matrix.c && time ./matrix_O1
 *   gcc -O2 -o matrix_O2 matrix.c && time ./matrix_O2
 *   gcc -O3 -o matrix_O3 matrix.c && time ./matrix_O3
 *   gcc -Ofast -o matrix_Ofast matrix.c && time ./matrix_Ofast
 *
 * ASSEMBLY (Lab 1B):
 *   gcc -O0 -S -fverbose-asm -o matrix_O0.s matrix.c
 *   gcc -O2 -S -fverbose-asm -o matrix_O2.s matrix.c
 *   grep -E "vmovsd|vfmadd|ymm|xmm" matrix_O2.s | head -20
 *
 * DOMANDE GUIDA:
 *   - Quale livello produce il codice più veloce?
 *   - -O3 batte sempre -O2? Perché potrebbe non farlo?
 *   - Con -fopt-info-vec quanti loop vengono vettorizzati?
 *
 * NOTA MAC (clang):
 *   clang -O0 -o matrix_O0 matrix.c && time ./matrix_O0
 *   (clang usa gli stessi flag -O; assembly con: clang -S -O2 matrix.c)
 * =============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N     2048      /* dimensione matrice (riduci a 512 se lento) */
#define REPS  20        /* numero di ripetizioni per misure stabili   */

/* Matrici globali — evitano overflow dello stack per N grande */
static double A[N][N];
static double b[N];
static double c[N];

/* ------------------------------------------------------------- *
 * Kernel: prodotto matrice-vettore  c = A * b
 * Questo è il kernel da ottimizzare.
 * ------------------------------------------------------------- */
void matvec(void)
{
    for (int i = 0; i < N; i++) {
        c[i] = 0.0;
        for (int j = 0; j < N; j++)
            c[i] += A[i][j] * b[j];
    }
}

/* ------------------------------------------------------------- *
 * Inizializzazione con valori deterministici (non rand())
 * per risultati riproducibili tra le run.
 * ------------------------------------------------------------- */
static void init(void)
{
    for (int i = 0; i < N; i++) {
        b[i] = (double)i / N;
        for (int j = 0; j < N; j++)
            A[i][j] = (double)(i + j) / (N * N);
    }
}

/* ------------------------------------------------------------- *
 * Misura del tempo wall-clock in secondi (POSIX)
 * ------------------------------------------------------------- */
static double wtime(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(void)
{
    init();

    /* warm-up: una prima chiamata per "scaldare" la cache */
    matvec();

    double t0 = wtime();
    for (int r = 0; r < REPS; r++)
        matvec();
    double elapsed = wtime() - t0;

    /* checksum per evitare che il compilatore elimini il kernel */
    double sum = 0.0;
    for (int i = 0; i < N; i++) sum += c[i];

    printf("N=%d  reps=%d  time=%.4f s  avg=%.4f s  checksum=%.6e\n",
           N, REPS, elapsed, elapsed / REPS, sum);

    return 0;
}
