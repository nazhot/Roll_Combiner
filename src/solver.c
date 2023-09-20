#include <stdio.h>
#include <stdlib.h>
#include "group.h"
#include "intArray.h"
#include "smallArray.h"
#include "roll.h"
#include "solver.h"


void recursiveSolve( const unsigned int currentGroup, const int currentArrayIndex, const int numGroupsInOrder, struct GroupArray **groupsWithRoll, struct OrderStats *orderStats, struct SmallArray *alreadyFound, int *numFound, const int numPotentialOrders, int *ordersWithRoll, int *ordersWithRollBitMask ) {
    //Id, Length, Number of Groups, Number of Rolls, Order Groups,Remaining Rolls, Average Remaining Roll Length
    if ( currentGroup & *ordersWithRollBitMask || getSmallArrayValue( alreadyFound, currentGroup ) ) {
        return;
    }

    setSmallArrayValue( alreadyFound, currentGroup );

    if ( numGroupsInOrder >= orderStats->minGroupsPerOrder ) {
        const float currentLength = rollsLength( currentGroup, orderStats->numberOfRolls, orderStats->rollList );
        if ( currentLength >= orderStats->minOrderLength && currentLength <= orderStats->maxOrderLength ) {
            *numFound += 1;
            for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
                ordersWithRoll[i] -= ( currentGroup >> i & 1 );
                if ( ordersWithRoll[i] == 0 ) {
                    *ordersWithRollBitMask |= 1 << i; //|= to guard against this happening multiple times, which it will everytime a new order is found when ordersWithRoll[i] is already 0
                }
            }
            printf( "\rOrders found: %'i/%'i (%.2f%%)", *numFound, numPotentialOrders, *numFound * 1.0 / numPotentialOrders * 100 );
            fflush( stdout );
        }
        if ( currentLength + orderStats->minGroupLength > orderStats->maxOrderLength ) {
            return;
        }
    }

    struct GroupArray **newGroupsWithRoll = malloc( sizeof( struct GroupArray* ) * orderStats->numberOfRolls );
    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || ordersWithRoll[i] == 0) {
            continue;
        }
        newGroupsWithRoll[i] = createGroupArray( groupsWithRoll[i]->length, 0, 1.1 ); //will only be smaller than groupsWithRoll[i]->size
        for ( int j = 0; j < groupsWithRoll[i]->length; ++j ) {
            if ( currentGroup & groupsWithRoll[i]->content[j].rolls || groupsWithRoll[i]->content[j].rolls & *ordersWithRollBitMask ) {
                continue;
            }
            //bit i will always be set, definition of group being in groupsWithRoll[i]
            //always trying to fill in the lowest roll number first, and since a group being in groupsWithRoll[i] means no bit < i is set, i is the lowest
            //newGroupsWithRoll[i] = addToIntArray( newGroupsWithRoll[i], groupsWithRoll[i]->content[j] );
            addToGroupArrayNoResize( newGroupsWithRoll[i], groupsWithRoll[i]->content[j] );
        }
        //shrinkIntArray( newGroupsWithRoll[i] );
    }

    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || ordersWithRoll[i] == 0 ) {
            continue;
        }
        for ( int j = 0; j < newGroupsWithRoll[i]->length; ++j ) {
            if ( currentGroup & newGroupsWithRoll[i]->content[j].rolls ) { //could include || groupsWithRoll[i]->content[j] & *ordersWithRollBitMask, but this would only happen if that bitmask
                continue;                                            //changes during the run, since it is already checked before
            }
            recursiveSolve( currentGroup | newGroupsWithRoll[i]->content[j].rolls, i, numGroupsInOrder + 1, newGroupsWithRoll, orderStats, alreadyFound, numFound, numPotentialOrders, ordersWithRoll, ordersWithRollBitMask );
        }
    }

    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || ordersWithRoll[i] == 0 ) {
            continue;
        }
        freeGroupArray( newGroupsWithRoll[i] );
    }
    free( newGroupsWithRoll );
}
