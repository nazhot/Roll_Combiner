#include "intArray.h"
#include <stdio.h>
#include <stdlib.h>

struct IntArray* createIntArray( const int size, const int sizeAddConstant, const float sizeAddMultiplier ) {
    struct IntArray *intArray = malloc( sizeof( struct IntArray ) );
    if ( intArray == NULL ) {
        printf( "Couldn't allocate the memory for an IntArray\n" );
        exit(0);
    }
    intArray->content = malloc( sizeof( unsigned int ) * size );
    if ( intArray-> content == NULL ) {
        printf( "Couldn't allocate the memory for the array within IntArray, size %i\n", size );
        exit(0);
    }
    intArray->size = size;
    intArray->length = 0;
    intArray->sizeAddConstant = sizeAddConstant;
    intArray->sizeAddMultiplier = sizeAddMultiplier;

    return intArray;
}

struct IntArray* addToIntArray( struct IntArray *intArray, const unsigned int toAdd ) { 
    if ( intArray->length >= intArray->size ) {
        int newSize = intArray->size * intArray->sizeAddMultiplier + intArray->sizeAddConstant;
        if ( newSize == intArray->size ) {
            printf( "New size of IntArray is the same as the previous: %i\n", newSize );
            exit( 0 );
        }
        unsigned int *temp = realloc( intArray->content, sizeof( unsigned int ) * newSize );
        if ( temp == NULL ) {
            printf( "Couldn't re-allocate the memory needed for an IntArray, new size: %i\n", newSize );
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

void addToIntArrayNoResize( struct IntArray *intArray, const unsigned int toAdd ) {
    intArray->content[intArray->length] = toAdd;
    intArray->length++;
}

struct IntArray* shrinkIntArray( struct IntArray *intArray ) {
    if ( intArray->length == 0 ) {
        return intArray;
    }
    unsigned int *temp = realloc( intArray->content, sizeof( unsigned int ) * intArray->length );
    if ( temp == NULL ) {
        printf( "Couldn't re-allocate the memory to shrink the IntArray: %i\n", intArray->length );
        free( temp );
        exit( 0 );
    }
    intArray->content = temp;
    intArray->size = intArray->length;
    return intArray;
}

void freeIntArray( struct IntArray *intArray ) {
    free( intArray->content );
    free( intArray );
}
