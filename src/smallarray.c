#include "smallarray.h"
#include <stdio.h>
#include <stdlib.h>


struct smallarray* createSmallArray( int size ) {
    struct smallarray* temp = ( struct smallarray* ) malloc( sizeof( struct smallarray ) );
   
    int convertedSize = size / 32 + 1;

    temp->size = convertedSize;

    int32_t *array = ( int32_t* ) malloc( 4 * convertedSize );
    if ( array == NULL ) {
        printf( "Could not allocate smallarray of that size!\n" );
        return NULL;
    }
    for ( int i = 0; i < convertedSize; i++ ){
        array[i] = 0;
    }
    temp->contents = array;
    temp->numElements = 0;
    return temp;
}

int8_t getSmallArrayValue( struct smallarray* smallarray, int number ){
    int8_t modulo = number % 32;
    int    index  = number / 32; 

    if ( index > smallarray->size - 1 ) {
        return 0;
    }

    return ( smallarray->contents[index] >> modulo ) & 1;


}

void setSmallArrayValue( struct smallarray* smallarray, int number ) {
    int8_t modulo = number % 32;
    int    index  = number / 32;

    if ( index > smallarray->size - 1 ) {
        return;
    }

    smallarray->contents[index] = smallarray->contents[index] | ( 1 << modulo ); 
}
