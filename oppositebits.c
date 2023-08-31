#include "oppositebits.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int countBits( int num, int numBits ) {
    int total = 0;
    for ( int i = 0; i < numBits; i++ ) {
        total += ( num >> i ) & 1;
    }
    return total;
}

unsigned long binomial(unsigned long n, unsigned long k) {
  unsigned long c = 1, i;

  if (k > n-k) // take advantage of symmetry
    k = n-k;

  for (i = 1; i <= k; i++, n--) {
    if (c/i > ULONG_MAX/n) // return 0 on potential overflow
      return 0;

    c = c / i * n + c % i * n / i; // split c * n / i into (c / i * i + c % i) * n / i
  }

  return c;
}

void ob_makeFirstArray( struct oppositeBits *ob, int maxRollsInGroup ) {
    int  firstArraySize  = 1 << ob->numBits;
    int  ***totalArray = malloc( sizeof( int ) * firstArraySize );
    for ( int i = 0; i < firstArraySize; i++ ) {
        int secondArraySize = maxRollsInGroup;
        int **secondArray = malloc( sizeof( int ) * secondArraySize );        
        for ( int j = 0; j < secondArraySize; j++ ) {
            int  thirdArraySize = 2 << ( maxRollsInGroup - countBits( i, maxRollsInGroup ) );
            int *thirdArray = malloc( sizeof( int ) * thirdArraySize );
        }

        totalArray[i] = secondArray;
    }
}



struct oppositeBits* ob_makeOppositeBits( int numBits ) {
    struct oppositeBits *ob = malloc( sizeof( struct oppositeBits ) );
    ob->numBits = numBits; 
    ob_makeFirstArray( ob );
    return ob;
}
