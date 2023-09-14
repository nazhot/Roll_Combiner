#include <stdio.h>
#include <stdlib.h>
#include "smallArray.h"
#include "roll.h"


void recursiveSolve( unsigned int currentGroup, int currentArrayIndex, int numGroupsInOrder, struct IntArray **groupsWithRoll, struct OrderStats *orderStats, struct SmallArray *alreadyFound, int *numFound, int numPotentialOrders ) {
    //Id, Length, Number of Groups, Number of Rolls, Order Groups,Remaining Rolls, Average Remaining Roll Length
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

    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; i++ ) {
        if ( currentGroup >> i & 1 ) {
            continue;
        }
        for ( int j = 0; j < groupsWithRoll[i]->length; j++ ) {
            if ( currentGroup & groupsWithRoll[i]->content[j] ) {
                continue;
            }
            recursiveSolve( currentGroup | groupsWithRoll[i]->content[j], i, numGroupsInOrder + 1, groupsWithRoll, orderStats, alreadyFound, numFound, numPotentialOrders );
        }
    }
}
