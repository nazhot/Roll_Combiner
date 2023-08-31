#ifndef OPPOSITE_BITS_
#define OPPOSITE_BITS_

struct oppositeBits { 
    int numBits;
    int ***array;
};


struct oppositeBits* makeOppositeBits( int numBits );
int* ob_makeFirstArray( struct oppositeBits* );
int* ob_makeSecondArray( struct oppositeBits*, int );
int* ob_makeThirdArray( struct oppositeBits*, int, int );

#endif
