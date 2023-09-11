#include <stdio.h>
#include <stdlib.h>
#include "roll.h"
#include "intArray.h"

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

static unsigned nextSetOfNBits( unsigned x ) {
    unsigned smallest, ripple, new_smallest, ones;

    if ( x == 0 ) return 0;
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
float rollsLength(  unsigned int num, int numberOfRolls, struct Roll *rollList ) {
    float totalLength = 0;
    for ( int i = 0; i <= numberOfRolls; i++ ) {
        if ( num >> i & 1 ) {
            totalLength += rollList[i].length;
        }
    }
    return totalLength;
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

void setGroupArray( struct OrderStats *orderStats, struct int_array *groupArray ) {
    for ( int groupSize = orderStats->minRollsPerGroup; groupSize <= orderStats->maxRollsPerGroup; groupSize++ ) {
        int group         = ( 1 << groupSize ) - 1; //starts at the smallest possible number for a group with groupSize bits set
        int largestNumber = group << ( orderStats->numberOfRolls - groupSize ); //largest number that could represent a group with groupSize bits set

        do {
            float groupLength = rollsLength(  group, orderStats->numberOfRolls, orderStats->rollList ); 

            if ( groupLength < orderStats->minGroupLength || groupLength > orderStats->maxGroupLength ) {
                group = nextSetOfNBits( group );
                continue;
            }
            groupArray                   = addToIntArray( groupArray, group );
            group                        = nextSetOfNBits( group );
        } while ( group <= largestNumber );
    }
}
