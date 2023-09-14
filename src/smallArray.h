#ifndef SMALLARRAY_H_
#define SMALLARRAY_H_
#include <stdint.h>

//going to move all unsigned int to unsigned size 32, to ensure size is consistent
//also because I can't even get it running with 29, let alone more than that
struct SmallArray{
    int size;
    int32_t *contents;
    int numElements;
};


struct SmallArray* createSmallArray( int size );
int8_t getSmallArrayValue( struct SmallArray*, int );
void   setSmallArrayValue( struct SmallArray*, int );

#endif
