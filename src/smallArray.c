#include "smallArray.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


struct SmallArray* createSmallArray( int size ) {
    struct SmallArray* temp = ( struct SmallArray* ) malloc( sizeof( struct SmallArray ) );
   
    int convertedSize = size / 32 + 1;

    temp->size = convertedSize;

    int32_t *array = ( int32_t* ) malloc( 4 * convertedSize );
    if ( array == NULL ) {
        printf( "Could not allocate SmallArray of that size!\n" );
        return NULL;
    }
    for ( int i = 0; i < convertedSize; i++ ){
        array[i] = 0;
    }
    temp->contents = array;
    temp->numElements = 0;
    return temp;
}

int8_t getSmallArrayValue( struct SmallArray* SmallArray, const int number ){
    int    index  = number / 32; 
    int8_t modulo = number % 32;

    if ( index > SmallArray->size - 1 ) {
        return 0;
    }

    return ( SmallArray->contents[index] >> modulo ) & 1;


}

void setSmallArrayValue( struct SmallArray* SmallArray, const int number ) {
    int8_t modulo = number % 32;
    int    index  = number / 32;

    if ( index > SmallArray->size - 1 ) {
        return;
    }

    SmallArray->contents[index] = SmallArray->contents[index] | ( 1 << modulo ); 
}
