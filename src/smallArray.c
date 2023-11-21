#include "smallArray.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


struct SmallArray* createSmallArray( const int numberOfElements ) {
    struct SmallArray* smallArray = ( struct SmallArray* ) malloc( sizeof( struct SmallArray ) );
   
    if ( smallArray == NULL ) {
        fprintf(stderr, "Could not allocate a SmallArray!\n" );
        exit( 1 );
    }

    int convertedSize = numberOfElements / 32 + 1;
    smallArray->size = convertedSize;
    smallArray->numElements = 0;
    smallArray->contents = ( int32_t* ) malloc( 4 * convertedSize );

    if ( smallArray->contents == NULL ) {
        printf( "Could not allocate SmallArray of size (%i)!\n", numberOfElements );
        exit( 1 );
    }

    for ( int i = 0; i < convertedSize; i++ ){
        smallArray->contents[i] = 0;
    }
    return smallArray;
}

int8_t getSmallArrayValue( struct SmallArray* SmallArray, const int element ){
    int    index  = element / 32; 
    int8_t modulo = element % 32;

    if ( index > SmallArray->size - 1 ) {
        return 0;
    }

    return ( SmallArray->contents[index] >> modulo ) & 1;


}

void setSmallArrayValue( struct SmallArray* SmallArray, const int element ) {
    int8_t modulo = element % 32;
    int    index  = element / 32;

    if ( index > SmallArray->size - 1 ) {
        return;
    }

    SmallArray->contents[index] = SmallArray->contents[index] | ( 1 << modulo ); 
}
