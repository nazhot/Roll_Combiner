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

void ob_makeArray( struct oppositeBits *ob, int maxRollsInGroup ) {
    //Go through all of the possible numbers based on number of bits
    //First, make the second array (size: maxRollsInGroup)
    //Second, for 0 - maxRollsInGroup, make each of the third arrays
    //
    //After all arrays are initiallized, go through all of the possible numbers and actually do comparisons
    int firstArraySize = 1 << ob->numBits;
    ob->secondArraySizes = malloc( sizeof( int ) * firstArraySize );
    ob->thirdArraySizes  = malloc( sizeof( int* ) * firstArraySize );
    int ***totalArray = malloc( sizeof( int* ) * firstArraySize );

    for ( int i = 0; i < firstArraySize; i++ ) {
        //initialize the second array
        int numBitsInNumber = countBits( i, ob->numBits );
        int secondArraySize = maxRollsInGroup - numBitsInNumber + 1;
        int **secondArray = malloc( sizeof( int* ) * secondArraySize );        
        int thirdArrayIndexes[secondArraySize];
        int *thirdArraySizes = malloc ( sizeof( int ) * secondArraySize );

        ob->secondArraySizes[i] = secondArraySize;

        //initialize all of the third arrays
        for ( int j = 0; j < secondArraySize; j++ ) {
            int  thirdArraySize = binomial( ob->numBits - numBitsInNumber, j ); //#unset bits choose #bits defined by third array index
            int *thirdArray = malloc( sizeof( int ) * thirdArraySize );
            secondArray[j] = thirdArray;
            thirdArrayIndexes[j] = 0;
            thirdArraySizes[j] = thirdArraySize;
        }


        ob->thirdArraySizes[i] = thirdArraySizes;

        totalArray[i] = secondArray;
        //end of setup
        
        for ( int j = 0; j < firstArraySize; j++ ) {
           if ( i & j ) {
               continue;
           }

           int numBits = countBits( j,  ob->numBits );
           if ( numBits >= secondArraySize ) {
               continue;
           }
           totalArray[i][numBits][thirdArrayIndexes[numBits]] = j;
           thirdArrayIndexes[numBits]++;
        }
        //printf( "Number: %i\n", i );
        //for ( int j = 0; j < secondArraySize; j++ ) {
        //    printf( " -%i: ", j );
        //    for ( int k = 0; k < thirdArraySizes[j]; k++ ) {
        //        printf( "%i, ", totalArray[i][j][k] );
        //    }
        //    printf( "\n" );
        //}
    }
    ob->array = totalArray;
}



struct oppositeBits* ob_makeOppositeBits( int numBits ) {
    struct oppositeBits *ob = malloc( sizeof( struct oppositeBits ) );
    ob->numBits = numBits; 
    ob_makeArray( ob, 5 );
    return ob;
}


int* ob_getValidOppositeBits( struct oppositeBits* ob, long num, int numBits ) {
    int* validOppositeBits;
    int numGroups = numBits / ob->numBits;
    int bitMask = ( 1 << ob->numBits ) - 1;
    if ( numGroups % ob->numBits ) {
        numGroups++;
    }
    int numbers[numGroups];
    int numValidOppositeBits = 1; 
    for ( int i = 0; i < numGroups; i++ ) {
        bitMask <<= ( i * ob->numBits );
        int comparisonNumber = ( num & bitMask ) >> ( i * ob->numBits ); 
        numbers[i] = comparisonNumber;
        int temp = 0;
        for ( int j = 0; j < ob->secondArraySizes[comparisonNumber]; j++ ) {
            temp += ob->thirdArraySizes[comparisonNumber][j];
        }
        numValidOppositeBits *= temp;
    }
    printf( "Number of valid options: %i\n", numValidOppositeBits );

    return validOppositeBits;
}
