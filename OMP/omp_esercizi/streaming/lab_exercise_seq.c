#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAXDATA 1000
#define THRESHOLD 2000

int    more_data_arriving( int );
int    getting_data( int, int * );
double heavy_work( int );

int main() {
    
    srand48(time(NULL));
    int *data;
    int ndata = 0;
    int iteration = 0;
    int data_are_arriving = 1;
    int nblocks = 12;

    data = (int*)calloc(nblocks, sizeof(int));

    while (data_are_arriving) {

        /* Ricezione dati */
        ndata = getting_data(nblocks, data);
        printf("iteration %d: got %d data\n",
           iteration, ndata );

        /* Elaborazione sequenziale */
        for (int i = 0; i < ndata; i++) {
            heavy_work(data[i]);
        }

        /* Controllo fine dati */
        data_are_arriving = more_data_arriving(iteration);
        if(!data_are_arriving)
            printf("\t>>> iteration %d :  just got the news that no more data will arrive\n",
               iteration);
        iteration++;
    }

    free(data);
    return 0;
}

/* Simula arrivo dati */
int getting_data( int n, int *data )
{
 #define MIN  1000
 #define MAX 10000
  
  // produces no more than n-1
  // data
  int howmany = lrand48() % n;
  howmany = ( howmany == 0 ? 1 : howmany);

  // be sure that the data
  // array has enough room
  // to host up to n-1 data
  for( int j = 0; j < howmany; j++ )
    data[j] = 1024 + lrand48() % (MAX-MIN);  // values will range
                             // from MIN up to MAX
  
  return howmany;
}

/* Decide se arrivano ancora dati */
int more_data_arriving( int i )
{
  // it is increasingly probable that
  // no more data arrive when i approaches
  // THRESHOLD
  //
  double p = (double)(THRESHOLD - i) / THRESHOLD;
  return (drand48() < p);
}

/* Lavoro pesante */
double heavy_work( int N )
{
  double guess = 3.141572 / 3 * N;

  for( int i = 0; i < N; i++ )
    {
      guess = exp( guess );
      guess = sin( guess );

    }
  return guess;
}
