#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define DEFAULT 1000000

double f(double x)
{
   return (4.0 / (1.0 + x*x));
}


int main ( int argc, char **argv)
{
    
    
    int           nthreads=1;
    double        pi;
    #if defined(_OPENMP)
    #pragma omp parallel
    #pragma omp single
    nthreads = omp_get_num_threads();
    #endif

    long long int N = (argc > 1 ? atoll(argv[1]) : DEFAULT ) ;
    printf("omp calculation with %d threads\nN=%lld\n",
	   nthreads ,N);
    
    const double fH    = 1.0 / (double) N;
    double fSum = 0.0;
    double fX;
    double timing = 0.0;
    int i;
    #if defined(_OPENMP)
    timing = omp_get_wtime();
    #pragma omp parallel for
    #endif
        for (i = 0; i < N; i++)
        {
        fX = fH * ((double)i + 0.5);
        fSum += f(fX);
        }
    pi=fH*fSum;
    #if defined(_OPENMP)
    timing = omp_get_wtime() - timing;
    #endif
    printf("Estimation of pi: %1.9f\n Walltime:%g\n", pi, timing );
      
    return 0;
}
