#include "fasted.h"

#ifdef _OPENMP
#include <omp.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int levenshtein_distance(const int *s1, size_t len1, const int *s2, size_t len2)
{
    int th_id, nthreads;
	  int **bounds, n;

    /* strip common prefix */
    while (len1 > 0 && len2 > 0 && *s1 == *s2) {
        len1--;
        len2--;
        s1++; s2++;
    }

    /* strip common suffix */
    while (len1 > 0 && len2 > 0 && s1[len1 - 1] == s2[len2 - 1]) {
        len1--;
        len2--;
    }

    /* catch trivial cases */
    if (len1 == 0) return len2;
    if (len2 == 0) return len1;

    /* swap if s2 longer than s1 */
    if (len1 < len2)
    {
        const int *tmp;
        tmp = s1; s1 = s2; s2 = tmp;
        len1 ^= len2; len2 ^= len1; len1 ^= len2;
    }

	  #pragma omp parallel private(th_id) shared(bounds, n, nthreads)
    {
        size_t chunk_size;
        size_t idx;
        int i, j;
        int *r, *p;
        int x1, x2;

#ifdef _OPENMP
        th_id = omp_get_thread_num();
        #pragma omp master
        nthreads = omp_get_num_threads();
#else
        th_id = 0;
        nthreads = 1;
#endif
        #pragma omp barrier
        chunk_size = len1 / nthreads;
        idx = th_id * chunk_size;
        if (th_id == (nthreads - 1)) {
          chunk_size = len1 - idx;
        }

        #pragma omp master
        bounds = malloc(nthreads * sizeof(int *));
		    #pragma omp barrier
        bounds[th_id] = malloc((len2 + 1) * sizeof(int));
        for (j = 0; j <= len2; ++j) {
          bounds[th_id][j] = -1;
        }
		    #pragma omp barrier

        /* processed line */
        n = (*s1 != *s2) ? 1 : 0;
        r = (int *)malloc(sizeof(int) * (chunk_size + 1));
        for (i = 0, p = r; i <= chunk_size; ++i) *p++ = idx + i;

        /* calculate columnwise */
        for (j = 1; j <= len2; ++j) {
            x2 = r[0] + 1;
            do {
              #pragma omp critical
              x1 = th_id ? bounds[th_id - 1][j] : j;
            } while (x1 == -1);
            /* process line */
            r[0] = x1;
            for (i = 1; i <= chunk_size; ++i) {
                x2 -= s1[idx + i - 1] == s2[j - 1];
                if (++x1 > x2) {
                    x1 = x2;
                }
                x2 = r[i] + 1;
                if (x1 > x2) {
                    x1 = x2;
                }
                r[i] = x1;
            }
            #pragma omp critical
			      bounds[th_id][j] = r[chunk_size];
        }

        /* total edit distance */
        n = r[chunk_size];

        /* dispose the allocated memory */
        free(r);
		    #pragma omp barrier
        free(bounds[th_id]);
        #pragma omp master
        free(bounds);
    }
    
    return n;
}
