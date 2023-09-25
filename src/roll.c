#include <stdio.h>
#include <stdlib.h>
#include "roll.h"
#include "intArray.h"
#include "smallArray.h"

static int ascRollSort( const void *roll1, const void *roll2 ) {
    struct Roll *r1 = ( struct Roll* ) roll1;
    struct Roll *r2 = ( struct Roll* ) roll2;
    return r1->length - r2->length;
}

static int dscRollSort( const void *roll1, const void *roll2 ) {
    struct Roll *r1 = ( struct Roll* ) roll1;
    struct Roll *r2 = ( struct Roll* ) roll2;
    return r2->length - r1->length;
}

static int dscRollSortByNumGroups( const void *roll1, const void *roll2 ) {
    struct Roll *r1 = ( struct Roll* ) roll1;
    struct Roll *r2 = ( struct Roll* ) roll2;
    return r2->numGroups - r1->numGroups;
}

static unsigned nextSetOfNBits( const unsigned x ) {
    unsigned smallest, ripple, new_smallest, ones;

    smallest     = ( x & -x );
    ripple       = x + smallest;
    new_smallest = ( ripple & -ripple );
    ones         = ( ( new_smallest / smallest ) >> 1 ) - 1;
    return ripple | ones;

}

void sortRollsAscending( struct Roll *rollList, int numberOfRolls ) {
    qsort( rollList, numberOfRolls, sizeof( struct Roll ), ascRollSort );
}

void sortRollsDescending( struct Roll *rollList, int numberOfRolls ) {
    qsort( rollList, numberOfRolls, sizeof( struct Roll ), dscRollSort );
}

void printRollsFromInt(  unsigned int integer, int numberOfRolls, struct Roll *rollList ) {
    printf( "----------GROUP----------\n" );
    float totalLength = 0;
    for ( int i = 0; i < numberOfRolls; i++ ) {
       if ( integer >> i & 1 ) {
           struct Roll roll = rollList[i];
           printf( "Roll %d: %s, %.2f\n", i, roll.id, roll.length );
           totalLength += roll.length;
       }
    }
    printf( "Total Length: %.2f\n", totalLength );
    printf( "-------------------------\n" );
}

/*
 * get the total length of a set of rolls
 *      rolls:    array of Rolls, that have length and id information
 *      num:      number representation of the set of rolls
 *      numRolls: maximum number of bits to check
*/
float rollsLength( const unsigned int num, const int numberOfRolls, struct Roll *rollList ) {
    float totalLength = 0;
    for ( int i = 0; i <= numberOfRolls; i++ ) {
        totalLength += ( num >> i & 1 ) * rollList[i].length;
    }
    return totalLength;
}

float test_rollsLength( const unsigned int num, float rollLengths[4][256]) {
    return rollLengths[0][( num >> 0 ) & 255 ] + 
           rollLengths[1][( num >> 8 ) & 255 ] +
           rollLengths[2][( num >> 16 ) & 255 ] +
           rollLengths[3][( num >> 24 ) & 256 ];
}

void setMinMaxRollStats( struct OrderStats *orderStats ) {
    sortRollsDescending( orderStats->rollList, orderStats->numberOfRolls );

    orderStats->minRollsPerGroup = 1;
    orderStats->minRollsPerOrder = 1;
    orderStats->maxRollsPerOrder = 1;

    float minRollsPerGroupLength = 0;
    float minRollsPerOrderLength = 0;
    float maxRollsPerOrderLength = 0;

    for ( int i = 0; i < orderStats->numberOfRolls; i++ ) {
       int minIndex = i;
       int maxIndex = orderStats->numberOfRolls - 1 - i;
       minRollsPerGroupLength += orderStats->rollList[minIndex].length;
       minRollsPerOrderLength += orderStats->rollList[minIndex].length;
       maxRollsPerOrderLength += orderStats->rollList[maxIndex].length;
       if ( minRollsPerGroupLength <= orderStats->minGroupLength ) {
           orderStats->minRollsPerGroup++;
       }
       if ( minRollsPerOrderLength <= orderStats->minOrderLength ) {
           orderStats->minRollsPerOrder++;
       }
       if ( maxRollsPerOrderLength <= orderStats->maxOrderLength ) {
           orderStats->maxRollsPerOrder++;
       }
    }
}

struct IntArray** getGroupsWithRollBySize( struct OrderStats *orderStats ) {

    struct IntArray **groupsWithRoll = malloc( sizeof( struct IntArray* ) * orderStats->numberOfRolls );
    int numberOfGroups = 0;

    for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
        groupsWithRoll[i] = createIntArray( 500000, 0, 2 );
    }

    for ( int groupSize = orderStats->minRollsPerGroup; groupSize <= orderStats->maxRollsPerGroup; groupSize++ ) {
        int group         = ( 1 << groupSize ) - 1; //starts at the smallest possible number for a group with groupSize bits set
        const int largestNumber = group << ( orderStats->numberOfRolls - groupSize ); //largest number that could represent a group with groupSize bits set

        do {
            const float groupLength = rollsLength(  group, orderStats->numberOfRolls, orderStats->rollList ); 

            if ( groupLength < orderStats->minGroupLength || groupLength > orderStats->maxGroupLength ) {
                group = nextSetOfNBits( group );
                continue;
            }
            for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
                if ( group >> i & 1 ) {
                    groupsWithRoll[i] = addToIntArray( groupsWithRoll[i], group );
                    break;
                }
            }
            group = nextSetOfNBits( group );
            numberOfGroups++;
        } while ( group <= largestNumber );
    }
    
    for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
        shrinkIntArray( groupsWithRoll[i] );
    }
    orderStats->numberOfGroups = numberOfGroups;
    return groupsWithRoll;
}

void setNumGroupsPerRoll( struct OrderStats *orderStats ) {
    for ( int groupSize = orderStats->minRollsPerGroup; groupSize <= orderStats->maxRollsPerGroup; groupSize++ ) {
        int group         = ( 1 << groupSize ) - 1; //starts at the smallest possible number for a group with groupSize bits set
        const int largestNumber = group << ( orderStats->numberOfRolls - groupSize ); //largest number that could represent a group with groupSize bits set

        do {
            const float groupLength = rollsLength(  group, orderStats->numberOfRolls, orderStats->rollList ); 

            if ( groupLength < orderStats->minGroupLength || groupLength > orderStats->maxGroupLength ) {
                group = nextSetOfNBits( group );
                continue;
            }

            for ( int i = 0; i < orderStats->numberOfRolls; i++ ) {
                if ( group >> i & 1 ) {
                    orderStats->rollList[i].numGroups++;
                }
            }
            group = nextSetOfNBits( group );
        } while ( group <= largestNumber );
    }
}

void sortRollsByNumGroups( struct OrderStats *orderStats ) {
    qsort( orderStats->rollList, orderStats->numberOfRolls, sizeof( struct Roll ), dscRollSortByNumGroups );
}

void setRollLengthsArray( struct OrderStats *orderStats, struct IntArray **groupsWithRoll ) {
    for ( int i = 0; i < 4; ++i ) {
        for ( int j = 0; j < 256; j++ ) {
            unsigned int group = j << ( 8 * i );
            orderStats->rollLengths[i][j] = rollsLength( group, orderStats->numberOfRolls, orderStats->rollList );
        }
    }
}

int* getOrdersWithRoll( struct OrderStats *orderStats ) {
    int *ordersWithRoll = malloc( sizeof( int ) * orderStats->numberOfRolls );
    int numPotentialOrders = 0;

    for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
        ordersWithRoll[i] = 0;
    }

    for ( int orderSize = orderStats->minRollsPerOrder; orderSize <= orderStats->maxRollsPerOrder; ++orderSize ) {
        int order         = ( 1 << orderSize ) - 1;
        const int largestNumber = order << ( orderStats->numberOfRolls - orderSize );

        do {
            const float orderLength = rollsLength( order, orderStats->numberOfRolls, orderStats->rollList );

            if ( orderLength < orderStats->minOrderLength || orderLength > orderStats->maxOrderLength ) {
                order = nextSetOfNBits( order );
                continue;
            }
            for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
                if ( order >> i & 1  ) {
                    ordersWithRoll[i] += 1;
                }
            }
            numPotentialOrders++; 
            order = nextSetOfNBits( order );
        } while ( order <= largestNumber );
    }
    orderStats->numberOfPotentialOrders = numPotentialOrders;
    return ordersWithRoll;
}
