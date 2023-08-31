#ifndef OPPOSITE_BITS_
#define OPPOSITE_BITS_

struct oppositeBits { 
    int numBits;
    int ***array;
};


struct oppositeBits* ob_makeOppositeBits( int numBits );
void ob_makeFirstArray( struct oppositeBits*, int );
int* ob_makeSecondArray( struct oppositeBits*, int );
int* ob_makeThirdArray( struct oppositeBits*, int, int );
unsigned long binomial( unsigned long, unsigned long );


#endif
