#include <stdio.h>
#include <stdlib.h>
#include "intArray.h"
#include "smallArray.h"
#include "roll.h"
#include "solver.h"


static void recursiveSolve( const unsigned int currentGroup, const int currentArrayIndex, const int numGroupsInOrder, struct IntArray **groupsWithRoll, struct OrderStats *orderStats, struct SmallArray *alreadyFound, int *numFound, int *ordersWithRoll, int *ordersWithRollBitMask ) {
    //Id, Length, Number of Groups, Number of Rolls, Order Groups,Remaining Rolls, Average Remaining Roll Length
    if ( currentGroup & *ordersWithRollBitMask || getSmallArrayValue( alreadyFound, currentGroup ) ) {
        return;
    }

    setSmallArrayValue( alreadyFound, currentGroup );

    if ( numGroupsInOrder >= orderStats->minGroupsPerOrder && __builtin_popcount( currentGroup ) >= orderStats->minRollsPerOrder ) {
        const float currentLength = rollsLength( currentGroup, orderStats->numberOfRolls, orderStats->rollList );
        if ( currentLength >= orderStats->minOrderLength && currentLength <= orderStats->maxOrderLength ) {
            *numFound += 1;
            for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
                ordersWithRoll[i] -= ( currentGroup >> i & 1 );
                if ( ordersWithRoll[i] == 0 ) {
                    *ordersWithRollBitMask |= 1 << i; //|= to guard against this happening multiple times, which it will everytime a new order is found when ordersWithRoll[i] is already 0
                }
            }
            printf( "\rOrders found: %'i/%'i (%.2f%%)", *numFound, orderStats->numberOfPotentialOrders, *numFound * 1.0 / orderStats->numberOfPotentialOrders * 100 );
            fflush( stdout );
        }
        if ( currentLength + orderStats->minGroupLength > orderStats->maxOrderLength ) {
            return;
        }
    }

    struct IntArray **newGroupsWithRoll = malloc( sizeof( struct IntArray* ) * orderStats->numberOfRolls );
    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || ordersWithRoll[i] == 0) {
            continue;
        }
        newGroupsWithRoll[i] = createIntArray( groupsWithRoll[i]->length, 0, 1.1 ); //will only be smaller than groupsWithRoll[i]->size
        for ( int j = 0; j < groupsWithRoll[i]->length; ++j ) {
            if ( currentGroup & groupsWithRoll[i]->content[j] || groupsWithRoll[i]->content[j] & *ordersWithRollBitMask ) {
                continue;
            }
            //bit i will always be set, definition of group being in groupsWithRoll[i]
            //always trying to fill in the lowest roll number first, and since a group being in groupsWithRoll[i] means no bit < i is set, i is the lowest
            //newGroupsWithRoll[i] = addToIntArray( newGroupsWithRoll[i], groupsWithRoll[i]->content[j] );
            addToIntArrayNoResize( newGroupsWithRoll[i], groupsWithRoll[i]->content[j] );
        }
        //shrinkIntArray( newGroupsWithRoll[i] );
    }

    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || ordersWithRoll[i] == 0 ) {
            continue;
        }
        for ( int j = 0; j < newGroupsWithRoll[i]->length; ++j ) {
            if ( currentGroup & newGroupsWithRoll[i]->content[j] ) { //could include || groupsWithRoll[i]->content[j] & *ordersWithRollBitMask, but this would only happen if that bitmask
                continue;                                            //changes during the run, since it is already checked before
            }
            recursiveSolve( currentGroup | newGroupsWithRoll[i]->content[j], i, numGroupsInOrder + 1, newGroupsWithRoll, orderStats, alreadyFound, numFound, ordersWithRoll, ordersWithRollBitMask );
        }
    }

    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || ordersWithRoll[i] == 0 ) {
            continue;
        }
        freeIntArray( newGroupsWithRoll[i] );
    }
    free( newGroupsWithRoll );
}

static void testRecursiveSolve( const unsigned int currentGroup, const int currentArrayIndex, const int numGroupsInOrder, struct IntArray ***groupsWithRoll, struct OrderStats *orderStats, struct SmallArray *alreadyFound, int *numFound, int *ordersWithRoll, int *ordersWithRollBitMask ) {
    //Id, Length, Number of Groups, Number of Rolls, Order Groups,Remaining Rolls, Average Remaining Roll Length
    if ( currentGroup & *ordersWithRollBitMask || getSmallArrayValue( alreadyFound, currentGroup ) ) {
        return;
    }

    setSmallArrayValue( alreadyFound, currentGroup );

    if ( numGroupsInOrder >= orderStats->minGroupsPerOrder && __builtin_popcount( currentGroup ) >= orderStats->minRollsPerOrder ) {
        const float currentLength = rollsLength( currentGroup, orderStats->numberOfRolls, orderStats->rollList );
        if ( currentLength >= orderStats->minOrderLength && currentLength <= orderStats->maxOrderLength ) {
            *numFound += 1;
            for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
                ordersWithRoll[i] -= ( currentGroup >> i & 1 );
                if ( ordersWithRoll[i] == 0 ) {
                    *ordersWithRollBitMask |= 1 << i; //|= to guard against this happening multiple times, which it will everytime a new order is found when ordersWithRoll[i] is already 0
                }
            }
            printf( "\rOrders found: %'i/%'i (%.2f%%)", *numFound, orderStats->numberOfPotentialOrders, *numFound * 1.0 / orderStats->numberOfPotentialOrders * 100 );
            fflush( stdout );
        }
        if ( currentLength + orderStats->minGroupLength > orderStats->maxOrderLength ) {
            return;
        }
    }

    struct IntArray ***newGroupsWithRoll = malloc( sizeof( struct IntArray* ) * orderStats->numberOfRolls );
    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || ordersWithRoll[i] == 0) {
            continue;
        }
        newGroupsWithRoll[i] = malloc( sizeof( struct IntArray* ) * orderStats->numberOfRolls );
        for ( int j = i + 1; j < orderStats->numberOfRolls; ++j ) {
            if ( currentGroup >> j & 1 || ordersWithRoll[j] == 0 ) {
                continue;
            }
            newGroupsWithRoll[i][j] = createIntArray( groupsWithRoll[i][j]->length, 0, 1.1 ); //will only be smaller than groupsWithRoll[i]->size
            for ( int k = 0; k < groupsWithRoll[i][j]->length; ++k ) {
                if ( currentGroup & groupsWithRoll[i][j]->content[k] || groupsWithRoll[i][j]->content[k] & *ordersWithRollBitMask ) {
                    continue;
                }
                addToIntArrayNoResize( newGroupsWithRoll[i][j], groupsWithRoll[i][j]->content[k] );
                //bit i will always be set, definition of group being in groupsWithRoll[i]
                //always trying to fill in the lowest roll number first, and since a group being in groupsWithRoll[i] means no bit < i is set, i is the lowest
                //newGroupsWithRoll[i] = addToIntArray( newGroupsWithRoll[i], groupsWithRoll[i]->content[j] );
            }
        }
        //shrinkIntArray( newGroupsWithRoll[i] );
    }

    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || ordersWithRoll[i] == 0 ) {
            continue;
        }
        for ( int j = i + i; j < orderStats->numberOfRolls; ++j ) {
            if ( currentGroup >> j & 1 || ordersWithRoll[j] == 0 ) {
                continue;
            }
            for ( int k = 0; k < newGroupsWithRoll[i][j]->length; ++k ) {
                if ( currentGroup & newGroupsWithRoll[i][j]->content[k] ) { //could include || groupsWithRoll[i]->content[j] & *ordersWithRollBitMask, but this would only happen if that bitmask
                    continue;                                            //changes during the run, since it is already checked before
                }
                testRecursiveSolve( currentGroup | newGroupsWithRoll[i][j]->content[k], i, numGroupsInOrder + 1, newGroupsWithRoll, orderStats, alreadyFound, numFound, ordersWithRoll, ordersWithRollBitMask );
            }
    
        }
    }

    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || ordersWithRoll[i] == 0 ) {
            continue;
        }
        for ( int j = i + 1; j < orderStats->numberOfRolls; ++j ) {
            if ( currentGroup >> j & 1 || ordersWithRoll[j] == 0 ) {
                continue;
            }
            freeIntArray( newGroupsWithRoll[i][j] );
        }
        free( newGroupsWithRoll[i] );
    }
    free( newGroupsWithRoll );
}


void orderSolve( struct IntArray **groupsWithRoll, struct OrderStats *orderStats, int *ordersWithRoll ) {
    int numFound = 0;
    int ordersWithRollBitMask = 0;
    int alreadyFoundSize = 1 << orderStats->numberOfRolls;
    struct SmallArray *alreadyFound = createSmallArray( alreadyFoundSize );

    for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
        printf(" Starting solving for index %i\n", i );
        for ( int j = 0; j < groupsWithRoll[i]->length; ++j ) {
            unsigned int group = groupsWithRoll[i]->content[j];
            recursiveSolve( group, i, 1, groupsWithRoll, orderStats, alreadyFound, &numFound, ordersWithRoll, &ordersWithRollBitMask );
        }
    }

}

void testOrderSolve( struct IntArray ***groupsWithRoll, struct OrderStats *orderStats, int *ordersWithRoll ) {
    int numFound = 0;
    int ordersWithRollBitMask = 0;
    int alreadyFoundSize = 1 << orderStats->numberOfRolls;
    struct SmallArray *alreadyFound = createSmallArray( alreadyFoundSize );

    for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
        printf(" Starting solving for index %i\n", i );
        for ( int j = i + 1; j < orderStats->numberOfRolls; ++j ) {
            for ( int k = 0; k < groupsWithRoll[i][j]->length; ++k ) {
                unsigned int group = groupsWithRoll[i][j]->content[k];
                testRecursiveSolve( group, i, 1, groupsWithRoll, orderStats, alreadyFound, &numFound, ordersWithRoll, &ordersWithRollBitMask );
            }
        }
    }

}
