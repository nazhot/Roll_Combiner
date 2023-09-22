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
    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || ordersWithRoll[i] == 0 ) {
            continue;
        }
        for ( int j = 0; j < groupsWithRoll[i]->length; ++j ) {
            if ( currentGroup & groupsWithRoll[i]->content[j] ) { //could include || groupsWithRoll[i]->content[j] & *ordersWithRollBitMask, but this would only happen if that bitmask
                continue;                                            //changes during the run, since it is already checked before
            }
            recursiveSolve( currentGroup | groupsWithRoll[i]->content[j], i, numGroupsInOrder + 1, groupsWithRoll, orderStats, alreadyFound, numFound, ordersWithRoll, ordersWithRollBitMask );
        }
    }
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

/*
 * increment an array through all possible numbers up to maxValue
 * array starts at [0, 1, 2, 3, ..., n]
 * array ends at   [maxValue - 3, maxValue - 2, maxValue - 1, maxValue]
 *      array:       array to increment
 *      arrayLength: how long the array is
 *      maxValue:    highest value the final element in the array will go to (inclusive)
*/
static int incrementArrayByMaxValues( int array[], int maxValues[], int arrayLength ) {
    for ( int i = arrayLength - 1; i >= 0; --i ) {
        if ( array[i] < maxValues[i] ) {
            array[i] += 1;
            return 1;
        }
        if ( i == 0 ) {
            return 0;
        }
        for ( int j = i - 1; j >= 0; --j ) {
            if ( array[j] < maxValues[j] ) {
                array[j] += 1;
                for ( int k = j + 1; k < arrayLength; ++k ) {
                    array[k] = 0;
                }
                return 1;
            }
        }
    }
    return 0;
}

static int incrementArray( int array[], int arrayLength, int maxValue ) {
    for ( int i = arrayLength - 1; i >= 0; i-- ) {
        int difference = (arrayLength - 1) - i;
        if ( array[i] < maxValue - difference ) {
            array[i] += 1;
            for ( int j = 1; i + j < arrayLength; j++ ) {
                array[i + j] = array[i] + j;
            }
            return 1;
        }
    }
    return 0;
}

void nonRecursiveSolve( struct IntArray **groupsWithRoll, struct OrderStats *orderStats, int *ordersWithRoll ) {

    for ( int numGroups = orderStats->minGroupsPerOrder; numGroups < orderStats->maxGroupsPerOrder; ++numGroups ) {
        printf( "Num groups: %i\n", numGroups );
        int firstRoll[numGroups];
        int groupStartingWithRoll[numGroups];
        for ( int i = 0; i < numGroups; ++i ) {
            firstRoll[i] = i;
            groupStartingWithRoll[i] = 0;
        }

        do {
            int maxValues[numGroups];
            for ( int i = 0; i < numGroups; ++i ) {
                maxValues[i] = groupsWithRoll[i]->length - 1;
            }
            do {

            } while ( incrementArrayByMaxValues( groupStartingWithRoll, maxValues, numGroups ) );
        } while( incrementArray( firstRoll, numGroups, orderStats->numberOfRolls - 1 ) );



    }

}
