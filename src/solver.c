#include <stdio.h>
#include <stdlib.h>
#include "intArray.h"
#include "smallArray.h"
#include "roll.h"
#include "solver.h"

struct OrderStats *g_orderStats;
struct IntArray **g_groupsWithRoll;
struct SmallArray *g_alreadyFound;
int *g_numFound;
int *g_ordersWithRoll;
int *g_ordersWithRollBitMask;

static void recursiveSolve( const unsigned int currentGroup, const int currentArrayIndex, const int numGroupsInOrder, struct IntArray **groupsWithRoll ) {
    //Id, Length, Number of Groups, Number of Rolls, Order Groups,Remaining Rolls, Average Remaining Roll Length
    if ( currentGroup & *g_ordersWithRollBitMask || getSmallArrayValue( g_alreadyFound, currentGroup ) ) {
        return;
    }

    setSmallArrayValue( g_alreadyFound, currentGroup );

    if ( numGroupsInOrder >= g_orderStats->minGroupsPerOrder ) {
        const float currentLength = rollsLength( currentGroup, g_orderStats->numberOfRolls, g_orderStats->rollList );
        if ( currentLength >= g_orderStats->minOrderLength && currentLength <= g_orderStats->maxOrderLength ) {
            *g_numFound += 1;
            for ( int i = 0; i < g_orderStats->numberOfRolls; ++i ) {
                g_ordersWithRoll[i] -= ( currentGroup >> i & 1 );
                if ( g_ordersWithRoll[i] == 0 ) {
                    *g_ordersWithRollBitMask |= 1 << i; //|= to guard against this happening multiple times, which it will everytime a new order is found when g_ordersWithRoll[i] is already 0
                }
            }
            printf( "\rOrders found: %'i/%'i (%.2f%%)", *g_numFound, g_orderStats->numberOfPotentialOrders, *g_numFound * 1.0 / g_orderStats->numberOfPotentialOrders * 100 );
            fflush( stdout );
        }
        if ( currentLength + g_orderStats->minGroupLength > g_orderStats->maxOrderLength ) {
            return;
        }
    }

    struct IntArray **newGroupsWithRoll = malloc( sizeof( struct IntArray* ) * g_orderStats->numberOfRolls );
    for ( int i = currentArrayIndex + 1; i < g_orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || g_ordersWithRoll[i] == 0) {
            continue;
        }
        newGroupsWithRoll[i] = createIntArray( g_groupsWithRoll[i]->length, 0, 1.1 ); //will only be smaller than g_groupsWithRoll[i]->size
        for ( int j = 0; j < g_groupsWithRoll[i]->length; ++j ) {
            if ( currentGroup & g_groupsWithRoll[i]->content[j] || g_groupsWithRoll[i]->content[j] & *g_ordersWithRollBitMask ) {
                continue;
            }
            //bit i will always be set, definition of group being in g_groupsWithRoll[i]
            //always trying to fill in the lowest roll number first, and since a group being in g_groupsWithRoll[i] means no bit < i is set, i is the lowest
            //newGroupsWithRoll[i] = addToIntArray( newGroupsWithRoll[i], g_groupsWithRoll[i]->content[j] );
            addToIntArrayNoResize( newGroupsWithRoll[i], g_groupsWithRoll[i]->content[j] );
        }
        //shrinkIntArray( newGroupsWithRoll[i] );
    }

    for ( int i = currentArrayIndex + 1; i < g_orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || g_ordersWithRoll[i] == 0 ) {
            continue;
        }
        for ( int j = 0; j < newGroupsWithRoll[i]->length; ++j ) {
            if ( currentGroup & newGroupsWithRoll[i]->content[j] ) { //could include || g_groupsWithRoll[i]->content[j] & *g_ordersWithRollBitMask, but this would only happen if that bitmask
                continue;                                            //changes during the run, since it is already checked before
            }
            recursiveSolve( currentGroup | newGroupsWithRoll[i]->content[j], i, numGroupsInOrder + 1, newGroupsWithRoll );
        }
    }

    for ( int i = currentArrayIndex + 1; i < g_orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || g_ordersWithRoll[i] == 0 ) {
            continue;
        }
        freeIntArray( newGroupsWithRoll[i] );
    }
    free( newGroupsWithRoll );
}

void rollSolve( struct OrderStats *orderStats, struct IntArray **groupsWithRoll, int *ordersWithRoll ) {
    int smallArraySize = 1 << orderStats->numberOfRolls;
    g_alreadyFound = createSmallArray( smallArraySize );
    g_orderStats = orderStats;
    g_groupsWithRoll = groupsWithRoll;
    g_ordersWithRoll = ordersWithRoll;
    g_numFound = malloc( sizeof( int ) );
    *g_numFound = 0;
    g_ordersWithRollBitMask = malloc( sizeof( int ) );
    *g_ordersWithRollBitMask = 0;

    for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
        printf(" Starting solving for index %i\n", i );
        for ( int j = 0; j < groupsWithRoll[i]->length; ++j ) {
            unsigned int group = groupsWithRoll[i]->content[j];
            recursiveSolve( group, i, 1, groupsWithRoll  );
        }
    }

}
