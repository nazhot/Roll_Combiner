#include <stdio.h>
#include <stdlib.h>
#include "group.h"


struct GroupArray* createGroupArray( const int size, const int sizeAddConstant, const float sizeAddMultiplier ) {
    struct GroupArray *groupArray = malloc( sizeof( struct GroupArray ) );
    if ( groupArray == NULL ) {
        printf( "Couldn't allocate the memory for an GroupArray\n" );
        exit(0);
    }
    groupArray->content = malloc( sizeof( struct Group ) * size );
    if ( groupArray-> content == NULL ) {
        printf( "Couldn't allocate the memory for the array within GroupArray, size %i\n", size );
        exit(0);
    }
    groupArray->size = size;
    groupArray->length = 0;
    groupArray->sizeAddConstant = sizeAddConstant;
    groupArray->sizeAddMultiplier = sizeAddMultiplier;

    return groupArray;
}


struct GroupArray* addToGroupArray( struct GroupArray *groupArray, struct Group toAdd ) { 
    if ( groupArray->length >= groupArray->size ) {
        int newSize = groupArray->size * groupArray->sizeAddMultiplier + groupArray->sizeAddConstant;
        if ( newSize == groupArray->size ) {
            printf( "New size of GroupArray is the same as the previous: %i\n", newSize );
            exit( 0 );
        }
        struct Group *temp = realloc( groupArray->content, sizeof( struct Group ) * newSize );
        if ( temp == NULL ) {
            printf( "Couldn't re-allocate the memory needed for an GroupArray, new size: %i\n", newSize );
            free( temp );
            exit( 0 );
        }
        groupArray->content = temp;
        groupArray->size = newSize;
    }

    groupArray->content[groupArray->length] = toAdd;
    groupArray->length++;
    
    return groupArray;
}

void addToGroupArrayNoResize( struct GroupArray *groupArray, const struct Group toAdd ) {
    groupArray->content[groupArray->length] = toAdd;
    groupArray->length++;
}

struct GroupArray* shrinkGroupArray( struct GroupArray *groupArray ) {
    if ( groupArray->length == 0 ) {
        return groupArray;
    }
    struct Group *temp = realloc( groupArray->content, sizeof( struct Group ) * groupArray->length );
    if ( temp == NULL ) {
        printf( "Couldn't re-allocate the memory to shrink the GroupArray: %i\n", groupArray->length );
        free( temp );
        exit( 0 );
    }
    groupArray->content = temp;
    groupArray->size = groupArray->length;
    return groupArray;
}

void freeGroupArray( struct GroupArray *groupArray ) {
    free( groupArray->content );
    free( groupArray );
}
