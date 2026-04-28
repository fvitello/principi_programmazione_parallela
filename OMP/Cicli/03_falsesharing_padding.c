
/* ────────────────────────────────────────────────────────────────────────── *
 │                                                                            │
 │ This file is part of the exercises for the Lectures on                     │
 │   "Foundations of High Performance Computing"                              │
 │ given at                                                                   │
 │   Master in HPC and                                                        │
 │   Master in Data Science and Scientific Computing                          │
 │ @ SISSA, ICTP and University of Trieste                                    │
 │                                                                            │
 │ contact: luca.tornatore@inaf.it                                            │
 │                                                                            │
 │     This is free software; you can redistribute it and/or modify           │
 │     it under the terms of the GNU General Public License as published by   │
 │     the Free Software Foundation; either version 3 of the License, or      │
 │     (at your option) any later version.                                    │
 │     This code is distributed in the hope that it will be useful,           │
 │     but WITHOUT ANY WARRANTY; without even the implied warranty of         │
 │     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          │
 │     GNU General Public License for more details.                           │
 │                                                                            │
 │     You should have received a copy of the GNU General Public License      │
 │     along with this program.  If not, see <http://www.gnu.org/licenses/>   │
 │                                                                            │
 * ────────────────────────────────────────────────────────────────────────── */


#if defined(__STDC__)
#  if (__STDC_VERSION__ >= 199901L)
#     define _XOPEN_SOURCE 700
#  endif
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <omp.h>


#define N_default 100

#define CPU_TIME_W (clock_gettime( CLOCK_REALTIME, &ts ), (double)ts.tv_sec +	\
		    (double)ts.tv_nsec * 1e-9)

#define CPU_TIME_T ({struct  timespec myts; clock_gettime( CLOCK_THREAD_CPUTIME_ID, &myts ),\
                         (double)myts.tv_sec +    \
                         (double)myts.tv_nsec * 1e-9;})

#define CPU_TIME_P (clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts ), (double)ts.tv_sec +	\
		   (double)ts.tv_nsec * 1e-9)




#define CPU_ID_ENTRY_IN_PROCSTAT 39
#define HOSTNAME_MAX_LENGTH      200

int read_proc__self_stat ( int, int * );
int get_cpu_id           ( void       );





int main( int argc, char **argv )
{

  int     N        = N_default;
  int     nthreads = 1;
  
  struct  timespec ts;
  double *array;

  // check whether some arg has been passed on
  if ( argc > 1 )
    N = atoi( *(argv+1) );


  // allocate memory
  if ( (array = (double*)calloc( N, sizeof(double) )) == NULL )
    {
      printf("I'm sorry, there is not enough memory to host %lu bytes\n", N * sizeof(double) );
      return 1;
    }

  // just give notice of what will happen and get the number of threads used
#pragma omp parallel
  {
#pragma omp master
    {
      nthreads = omp_get_num_threads();
      printf("omp summation with %d threads\n", nthreads );
    }
  }


  for ( int ii = 0; ii < N; ii++ )
    array[ii] = (double)ii;

  double S[ nthreads * 8 ];                                     // this will store the summation's chunks in a padded way
  double th_avg_time = 0;                                   // this will be the avg thread time
  double th_min_time = (1<<30);                             // this will be the min thread time
  
  double tstart  = CPU_TIME_W;  

  memset( S, 0, nthreads*8*sizeof(double) );
  
 #pragma omp parallel shared(S)
  {    
    int    me       = omp_get_thread_num();
    double mytstart = CPU_TIME_T; 
   #pragma omp for
    for ( int ii = 0; ii < N; ii++ )
      S[me*8] += array[ii];
    double myt      = CPU_TIME_T - mytstart;

   #pragma omp atomic update
    th_avg_time += myt;
   #pragma omp critical
    th_min_time  = (th_min_time > myt? myt : th_min_time);
  }

  if ( nthreads > 1 )                                       // is there any more efficient way
    for ( int ii = 1; ii < nthreads; ii++ )                 // to implement this reduction when
      S[0] += S[ii*8];                                        //   nthreads is large ?
  
  double tend = CPU_TIME_W;

  printf("Sum is %g, process took %g of wall-clock time\n\n"
	 "<%g> sec of avg thread-time\n"
	 "<%g> sec of min thread-time\n",
	 S[0], tend - tstart, th_avg_time/nthreads, th_min_time );
  
  free( array );
  return 0;
}
