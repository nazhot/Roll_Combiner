#include "intArray.h"
#include <stdio.h>
#include <stdlib.h>


struct int_array* createIntArray( int size, int sizeAddConstant, float sizeAddMultiplier ) {
    struct int_array *intArray = malloc( sizeof( struct int_array ) );
    if ( intArray == NULL ) {
        printf( "Couldn't allocate the memory for an int_array\n" );
        exit(0);
    }
    intArray->content = malloc( sizeof( unsigned int ) * size );
    if ( intArray-> content == NULL ) {
        printf( "Couldn't allocate the memory for the array within int_array, size %i\n", size );
        exit(0);
    }
    intArray->size = size;
    intArray->length = 0;
    intArray->sizeAddConstant = sizeAddConstant;
    intArray->sizeAddMultiplier = sizeAddMultiplier;

    return intArray;
}


struct int_array* addToIntArray( struct int_array *intArray, unsigned int toAdd ) { 
    if ( intArray->length >= intArray->size ) {
        int newSize = intArray->size * intArray->sizeAddMultiplier + intArray->sizeAddConstant;
        if ( newSize == intArray->size ) {
            printf( "New size of int_array is the same as the previous: %i\n", newSize );
            exit( 0 );
        }
        unsigned int *temp = realloc( intArray->content, sizeof( unsigned int ) * newSize );
        if ( temp == NULL ) {
            printf( "Couldn't re-allocate the memory needed for an int_array, new size: %i\n", newSize );
            free( temp );
            exit( 0 );
        }
        intArray->content = temp;
        intArray->size = newSize;
    }

    intArray->content[intArray->length] = toAdd;
    intArray->length++;
    
    return intArray;
}

struct int_array* shrinkIntArray( struct int_array *intArray ) {
    if ( intArray->length == 0 ) {
        return intArray;
    }
    unsigned int *temp = realloc( intArray->content, sizeof( unsigned int ) * intArray->length );
    if ( temp == NULL ) {
        printf( "Couldn't re-allocate the memory to shrink the int_array: %i\n", intArray->length );
        free( temp );
        exit( 0 );
    }
    intArray->content = temp;
    intArray->size = intArray->length;
    return intArray;
}

void freeIntArray( struct int_array *intArray ) {
    free( intArray->content );
    free( intArray );
}