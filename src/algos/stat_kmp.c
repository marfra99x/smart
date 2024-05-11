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
 * This is an implementation of the Knuth Morris Pratt algorithm
 * in D. E. Knuth and J. H. Morris and V. R. Pratt. 
 * Fast pattern matching in strings. SIAM J. Comput., vol.6, n.1, pp.323--350, (1977).
 */

#include "include/define.h"
#include "include/main.h"

void preKmp(unsigned char *x, int m, int kmpNext[]) {
   int i, j;
   i = 0;
   j = kmpNext[0] = -1;
   while (i < m) {
      while (j > -1 && x[i] != x[j])
         j = kmpNext[j];
      i++;
      j++;
      if (i<m && x[i] == x[j])
         kmpNext[i] = kmpNext[j];
      else
         kmpNext[i] = j; // i == m here, so kmpNext needs m + 1 elements.
   }
}


int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, kmpNext[m + 1], count;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   preKmp(x, m, kmpNext);
   END_PREPROCESSING

    _stats.memory_used = (m + 1) * sizeof(int);

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   i = j = 0;
   _stats.num_writes+=2;
    _stats.num_branches++;
    while (j < n) {
       _stats.num_verifications++;

       _stats.num_branches++;
       if (i > -1) {
           _stats.pattern_bytes_read++;
           _stats.text_bytes_read++;
       }

       _stats.num_branches++;
       while (i > -1 && x[i] != y[j]) {
          i = kmpNext[i];
          _stats.num_writes++;
          _stats.num_lookups++;

          _stats.num_branches++;
           if (i > -1) {
               _stats.pattern_bytes_read++;
               _stats.text_bytes_read++;
           }
           _stats.num_branches++;
        }

      i++;
      j++;
      _stats.num_jumps++;
      _stats.num_writes+=2;

      _stats.num_branches++;
      if (i >= m) {
         OUTPUT(j - i);
          i = kmpNext[i]; // i == m here so kmpNext needs m + 1 elements.
          _stats.num_writes++;
          _stats.num_lookups++;
      }

      _stats.num_branches++;
    }
   END_SEARCHING
   return count;
}


