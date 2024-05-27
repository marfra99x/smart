/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012  Simone Faro and Thierry Lecroq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 * 
 * contact the authors at: faro@dmi.unict.it, thierry.lecroq@univ-rouen.fr
 * download the tool at: http://www.dmi.unict.it/~faro/smart/
 *
 * This is an implementation of the Boyer-Moore algorithm
 * in R. S. Boyer and J. S. Moore. 
 * A fast string searching algorithm. Commun. ACM, vol.20, n.10, pp.762--772, (1977).
 */

/*
 * An instrumented implementation of Boyer Moore.
 * This should not be used for performance profiling, it is used to gather run-time algorithm statistics.
 *
 * EXTRA FIELDS
 * ============
 *
 * Field extra[0] tracks the number of entries in the shift table whose value is less than m.
 * This is the number of distinct characters which received a smaller value when pre-processing.
 */

#include "include/define.h"
#include "include/main.h"


void preBmBc(unsigned char *x, int m, int bmBc[]) {
    int i;
    for (i = 0; i < SIGMA; ++i)
        bmBc[i] = m;
    for (i = 0; i < m - 1; ++i)
        bmBc[x[i]] = m - i - 1;
}

void suffixes(unsigned char *x, int m, int *suff) {
    int f, g, i;
    suff[m - 1] = m;
    g = m - 1;
    for (i = m - 2; i >= 0; --i) {
        if (i > g && suff[i + m - 1 - f] < i - g)
            suff[i] = suff[i + m - 1 - f];
        else {
            if (i < g)
                g = i;
            f = i;
            while (g >= 0 && x[g] == x[g + m - 1 - f])
                --g;
            suff[i] = f - g;
        }
    }
}

void preBmGs(unsigned char *x, int m, int bmGs[]) {
    int i, j, suff[m];
    suffixes(x, m, suff);
    for (i = 0; i < m; ++i) bmGs[i] = m;
    j = 0;
    for (i = m - 1; i >= 0; --i)
        if (suff[i] == i + 1)
            for (; j < m - 1 - i; ++j)
                if (bmGs[j] == m)
                    bmGs[j] = m - 1 - i;
    for (i = 0; i <= m - 2; ++i)
        bmGs[m - 1 - suff[i]] = m - 1 - i;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, bmGs[m], bmBc[SIGMA], count;
 
   /* Preprocessing */
   BEGIN_PREPROCESSING
   preBmGs(x, m, bmGs);
   preBmBc(x, m, bmBc);
   END_PREPROCESSING

   _stats.memory_used = (SIGMA * sizeof(int)) + (m * sizeof(int));
   _stats.num_lookup_entries1 = SIGMA;
   _stats.num_lookup_entries2 = m;
   _stats.extra[0] = count_smaller_entries_int_table(bmBc, SIGMA, m);
    set_extra_name("#small", 0);

    /* Searching */
   BEGIN_SEARCHING
   j = 0;
   count = 0;
   _stats.num_writes += 2;
   _stats.num_branches++;
   while (j <= n - m) {

       _stats.num_branches++;

       if (i >= 0) {
           _stats.pattern_bytes_read++;
           _stats.text_bytes_read++;
       }

      for (i = m - 1; i >= 0 && x[i] == y[i + j]; --i) {

          // Count next time in loop condition.
          if (i >= 0) {
              _stats.pattern_bytes_read++;
              _stats.text_bytes_read++;
          }
          _stats.num_writes++;
          _stats.num_branches++;
      };

      _stats.num_branches++;
      _stats.num_verifications++;
      if (i < 0) {
         OUTPUT(j);
         j += bmGs[0];
         _stats.num_lookups++;
         _stats.num_writes++;
      }
      else {
          _stats.num_branches++;
          j += MAX(bmGs[i], bmBc[y[i + j]] - m + 1 + i);
          _stats.num_computations++;
          _stats.num_lookups += 2;
          _stats.num_writes++;
      }
       _stats.num_jumps++;
       _stats.num_branches++;
   }
   END_SEARCHING
   return count;
}
