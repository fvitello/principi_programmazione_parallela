
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
#include <omp.h>

int main( int argc, char **argv )
{

  /* try to understand what compiler is being used */
#ifdef __clang__
    printf("Compiled with Clang %d.%d.%d\n",
           __clang_major__,
           __clang_minor__,
           __clang_patchlevel__);
#elif defined(__GNUC__)
    printf("Compiled with GCC %d.%d.%d\n",
           __GNUC__,
           __GNUC_MINOR__,
           __GNUC_PATCHLEVEL__);
#elif defined(__INTEL_COMPILER)
    printf("Compiled with Intel Compiler %d\n",
           __INTEL_COMPILER);
#else
    printf("Unknown compiler\n");
#endif
  
#if defined(_OPENMP)

#pragma omp parallel               
#pragma omp single
  {
#include "omp_versions.h"           // this .h is not a standard one, it has
				    // been porpusely written for you
    
    int i;
    for( i = 0; i < _OPENMP_KNOWN_VERSIONS; i++ )
      if ( strncmp( _OMPv_STR(_OPENMP), omp_versions[i]._openmp_value, 6) == 0 )
	break;
    if ( i < _OPENMP_KNOWN_VERSIONS )
      printf("OpenMP supported version is %d.%d\n",
	     omp_versions[i].major, omp_versions[i].minor );
    else
      printf("Oh gawsh!! this (%s) is an unknown OpenMP version!!\n",
	     _OMPv_STR(_OPENMP) );
  }
#endif
  
  return 0;
}
