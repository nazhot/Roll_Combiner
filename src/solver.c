#include <stdio.h>
#include <stdlib.h>
#include "intArray.h"
#include "smallArray.h"
#include "roll.h"


void recursiveSolve( unsigned int currentGroup, int currentArrayIndex, int numGroupsInOrder, struct IntArray **groupsWithRoll, struct OrderStats *orderStats, struct SmallArray *alreadyFound, int *numFound, int numPotentialOrders, unsigned long *numChecked ) {
    //Id, Length, Number of Groups, Number of Rolls, Order Groups,Remaining Rolls, Average Remaining Roll Length
    *numChecked += 1;
    if ( getSmallArrayValue( alreadyFound, currentGroup ) ) {
        return;
    }

    setSmallArrayValue( alreadyFound, currentGroup );

    if ( numGroupsInOrder >= orderStats->minGroupsPerOrder ) {
        float currentLength = rollsLength( currentGroup, orderStats->numberOfRolls, orderStats->rollList );
        if ( currentLength >= orderStats->minOrderLength && currentLength <= orderStats->maxOrderLength ) {
            *numFound += 1;
            printf( "\rOrders found: %'i/%'i (%.2f%%)", *numFound, numPotentialOrders, *numFound * 1.0 / numPotentialOrders * 100 );
            fflush( stdout );
        }
        if ( currentLength >= orderStats->maxOrderLength ) {
            return;
        }
    }

    struct IntArray **newGroupsWithRoll = malloc( sizeof( struct IntArray* ) * orderStats->numberOfRolls );
    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; i++ ) {
        if ( currentGroup >> i & 1 ) {
            continue;
        }
        newGroupsWithRoll[i] = createIntArray( groupsWithRoll[i]->size, 0, 1.1 ); //will only be smaller than groupsWithRoll[i]->size
        for ( int j = 0; j < groupsWithRoll[i]->length; j++ ) {
            if ( currentGroup & groupsWithRoll[i]->content[j] ) {
                continue;
            }
            //bit i will always be set, definition of group being in groupsWithRoll[i]
            //always trying to fill in the lowest roll number first, and since a group being in groupsWithRoll[i] means no bit < i is set, i is the lowest
            newGroupsWithRoll[i] = addToIntArray( newGroupsWithRoll[i], groupsWithRoll[i]->content[j] );
        }
        shrinkIntArray( newGroupsWithRoll[i] );
    }

    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; i++ ) {
        if ( currentGroup >> i & 1 ) {
            continue;
        }
        for ( int j = 0; j < newGroupsWithRoll[i]->length; j++ ) {
            if ( currentGroup & newGroupsWithRoll[i]->content[j] ) {
                continue;
            }
            recursiveSolve( currentGroup | newGroupsWithRoll[i]->content[j], i, numGroupsInOrder + 1, newGroupsWithRoll, orderStats, alreadyFound, numFound, numPotentialOrders, numChecked );
        }
    }

    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; i++ ) {
        if ( currentGroup >> i & 1 ) {
            continue;
        }
        freeIntArray( newGroupsWithRoll[i] );
    }
    free( newGroupsWithRoll );
}
