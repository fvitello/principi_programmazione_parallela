#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static inline double seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void func1 ( int *restrict a, int *restrict b){
//void func1 ( int *a, int *b){
 *a += *b;
 *a += *b;
}


void func2 (int *a,int *b){
 *a += 2 * *b;
}
 
int main(void) {
    int a_val, b_val;
    int *a = &a_val;
    int *b = &b_val;

    double t0, t1;
    //const int iters = 1;
    const int iters = 100000000;  // large count so timing is visible

    // -------------------------------
    // 1. Non-aliasing case
    // -------------------------------
    a_val = 1;
    b_val = 1;

    t0 = seconds();
    for (int i = 0; i < iters; ++i)
        func1(a, b);
    t1 = seconds();
    printf("func1 non-alias:  a=%d  time=%.6f s\n", a_val, t1 - t0);

    a_val = 1;
    b_val = 1;
    t0 = seconds();
    for (int i = 0; i < iters; ++i)
        func2(a, b);
    t1 = seconds();
    printf("func2 non-alias:  a=%d  time=%.6f s\n", a_val, t1 - t0);

    // -------------------------------
    // 2. Aliasing case (a == b)
    // -------------------------------
    
    a = b = &a_val;
    
    
    t0 = seconds();
    for (int i = 0; i < iters; ++i){
        a_val = 1;
        func1(a, b);
    }
    t1 = seconds();
    printf("func1 alias:      a=%d  time=%.6f s\n", a_val, t1 - t0);

    
    t0 = seconds();
    for (int i = 0; i < iters; ++i){
        a_val = 1;
        func2(a, b);
    }
    t1 = seconds();
    printf("func2 alias:      a=%d  time=%.6f s\n", a_val, t1 - t0);

    return 0;
}
 

