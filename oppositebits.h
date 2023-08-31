#ifndef OPPOSITE_BITS_
#define OPPOSITE_BITS_

struct oppositeBits { 
    int numBits;
    int ***array;
    int *secondArraySizes;
    int **thirdArraySizes;
};


struct oppositeBits* ob_makeOppositeBits( int numBits );
void ob_makeArray( struct oppositeBits*, int );
unsigned long binomial( unsigned long, unsigned long );
int* ob_getValidOppositeBits( struct oppositeBits*, long, int );

#endif
