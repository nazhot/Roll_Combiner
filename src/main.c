#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <locale.h>
#include "fileReaders.h"
#include "smallarray.h"
#include "intArray.h"
#include "testMethods.h"
#include "roll.h"
                      //guarded by strncpy
#define MAX_NUM_ROLLS 32;

FILE *g_outputFile;

void printArray( unsigned int array[], int arrayLength ) {
    printf( "[" );
    for ( int i = 0; i < arrayLength; i++ ) {
       printf( "%u", array[i] ); 
       if ( i < arrayLength - 1 ) {
        printf( ", " );
       }
    }
    printf( "]\n" );
}


void printNumberBits( unsigned int num ) {
    printf( "%u: ", num );
    int numBits = sizeof( unsigned int ) * 8;
    for ( int i = 0; i < numBits; i++ ) {
        printf( "%i", num >> i & 1 );
    }
    printf( "\n" );
}


/*
 * go from an array where elements are the rolls that make up a group to the 
 *  equivalent unsigned int
 *      array:     elements are the rolls that make up a group
 *      arraySize: how many elements are in the array
*/
unsigned int arrayToInt( int array[], int arraySize ) {
    unsigned int integer = 0;
    for ( int i = 0; i < arraySize; i++ ) {
        int rollNumber = array[i];
        integer += 1 << rollNumber;
    }
    return integer;
}


unsigned nextSetOfNBits( unsigned x ) {
    unsigned smallest, ripple, new_smallest, ones;

    if ( x == 0 ) return 0;
    smallest     = ( x & -x );
    ripple       = x + smallest;
    new_smallest = ( ripple & -ripple );
    ones         = ( ( new_smallest / smallest ) >> 1 ) - 1;
    return ripple | ones;

}

//void setGroupsString( unsigned int *groups, int numGroups ) {
//    fputc( '\"', g_outputFile );
//    for ( int i = 0; i < numGroups; i++ ){
//        unsigned int group = groups[i];
//        float groupLength = rollsLength( group );
//        char groupLengthString[256];
//        sprintf( groupLengthString, "%.2f", groupLength );
//
//        fputs( "-\n", g_outputFile );
//
//        for ( int j = 0; j < numberOfRolls; j++ ) {
//            if ( !(  group >> j & 1  ) ) {
//               continue; 
//            }
//            char rollLength[256];
//            sprintf( rollLength, "%.2f", rollList[j].length ); 
//            fputs( rollList[j].id, g_outputFile );
//            fputs( ": ", g_outputFile );
//            fputs( rollLength, g_outputFile );
//            fputc( '\n', g_outputFile );
//        }
//        fputs( "-\n", g_outputFile ); 
//    }
//    fputc( '\"', g_outputFile );
//}

//void setRemainingRollsString( unsigned int order ) {
//    int numRemainingRolls = 0;
//    float remainingRollsLength = 0;
//    float average;
//    fputc( '\"', g_outputFile );
//    for ( int i = 0; i < numberOfRolls; i++ ) {
//        if ( order >> i & 1 ) {
//            continue;
//        }
//        numRemainingRolls++;
//        remainingRollsLength += rollList[i].length;
//        char rollLength[256];
//        sprintf( rollLength, "%.2f", rollList[i].length ); 
//        fputs( rollList[i].id, g_outputFile );
//        fputs( ": ", g_outputFile );
//        fputs( rollLength, g_outputFile );
//        fputc( '\n', g_outputFile );
//
//    }
//    fputs( "\",", g_outputFile );
//
//    if ( numRemainingRolls == 0 ) {
//        average = 0;
//    } else {
//        average = remainingRollsLength / numRemainingRolls;
//    }
//
//    char averageLengthRemaining[256];
//    sprintf( averageLengthRemaining, "%.2f", average);
//    fputs( averageLengthRemaining, g_outputFile );
//    fputc( '\n', g_outputFile );
//}


//void recursiveSolve( unsigned int currentGroup, unsigned int *groups, int numGroupsInOrder, struct IntArray **groupsWithRoll, int minGroupsInOrder, int minOrderLength, int maxOrderLength, struct smallarray *alreadyFound, int *numFound, int numPotentialOrders ) {
//    //Id, Length, Number of Groups, Number of Rolls, Order Groups,Remaining Rolls, Average Remaining Roll Length
//    if ( getSmallArrayValue( alreadyFound, currentGroup ) ) {
//        return;
//    }
//
//    setSmallArrayValue( alreadyFound, currentGroup );
//
//    if ( numGroupsInOrder >= minGroupsInOrder ) {
//        float currentLength = rollsLength( currentGroup );
//        if ( currentLength >= minOrderLength && currentLength <= maxOrderLength ) {
//            *numFound += 1;
//            printf( "\rOrders found: %'i/%'i (%.2f%%)", *numFound, numPotentialOrders, *numFound * 1.0 / numPotentialOrders * 100 );
//            fflush( stdout );
//            //printf( "Found %i\n", *numFound );
//
//            char orderNumber[256];
//            sprintf( orderNumber, "%d", currentGroup );
//            fputs( orderNumber, g_outputFile );
//            fputc( ',', g_outputFile );
//
//            char orderLength[256];
//            sprintf( orderLength, "%f", currentLength );
//            fputs( orderLength, g_outputFile );
//            fputc( ',', g_outputFile ); 
//
//            char numGroups[256];
//            sprintf( numGroups, "%d", numGroupsInOrder );
//            fputs( numGroups, g_outputFile );
//            fputc( ',', g_outputFile );
//
//            char numberOfRolls[256];
//            sprintf( numberOfRolls, "%d", __builtin_popcount( currentGroup ) );
//            fputs( numberOfRolls, g_outputFile );
//            fputc( ',', g_outputFile );
//
//            setGroupsString( groups, numGroupsInOrder );
//            //add Order Groups
//            //add Remaining Rolls
//            //add Average Remaining Roll Length
//            fputc( ',', g_outputFile );
//            setRemainingRollsString( currentGroup );
//        }
//        if ( currentLength >= maxOrderLength ) {
//            return;
//        }
//    }
//
//    int numValidGroups = 0; //how many groups don't share any rolls with currentGroup
//    int numValidRolls  = 0; //how many rolls aren't already a part of currentGroup
//                            //
//    for ( int i = 0; i < numberOfRolls; i++ ) {
//        if ( !( currentGroup >> i & 1 ) ) {
//            numValidGroups += groupsWithRoll[i]->length;
//            numValidRolls++;
//        }
//    }
//
//    if ( numValidRolls == 0 || numValidGroups == 0 ) { //prevents divide by 0 error, and is also a valid end point for the function
//        return;
//    }
//
//    int averageGroupsPerRoll             = numValidGroups / numValidRolls + 1;
//    struct IntArray **newGroupsWithRoll = malloc( sizeof( struct IntArray* ) * numberOfRolls ); //holds the groups that can go with currentGroup
//
//    for ( int i = 0; i < numberOfRolls; i++ ) { //initialize the IntArrays
//        newGroupsWithRoll[i] = createIntArray( averageGroupsPerRoll, 0, 1.1 );
//    }
//
//    for ( int i = 0; i < numberOfRolls; i++ ) {
//        if ( currentGroup >> i & 1 ) {
//            continue; 
//        }
//        for ( int j = 0; j < groupsWithRoll[i]->length; j++ ) {
//            if ( currentGroup & groupsWithRoll[i]->content[j] ) { //make sure group could be added
//                continue;
//            }
//            int index = getSmallestIntArrayIndex( groupsWithRoll[i]->content[j], newGroupsWithRoll );
//            newGroupsWithRoll[index] = addToIntArray( newGroupsWithRoll[index], groupsWithRoll[i]->content[j] );
//        }
//    }
//
//    for ( int i = 0; i < numberOfRolls; i++ ) {
//        if ( currentGroup >> i & 1  ) {
//            continue;
//        }
//        for ( int j = 0; j < newGroupsWithRoll[i]->length; j++ ) {
//            groups[numGroupsInOrder] = newGroupsWithRoll[i]->content[j];
//            recursiveSolve( currentGroup | newGroupsWithRoll[i]->content[j], groups, numGroupsInOrder + 1, newGroupsWithRoll, minGroupsInOrder, minOrderLength, maxOrderLength, alreadyFound, numFound, numPotentialOrders );
//        }
//    }
//
//    for ( int i = 0; i < numberOfRolls; i++ ) {
//        freeIntArray( newGroupsWithRoll[i] );
//    }
//
//    free( newGroupsWithRoll );
//}

void printCheck( char title[], struct check_t *check ) {
   printf( "--------%s-------\n", title ); 
   printf( "Time: %i sec, %i msecs\n", check->msecs / 1000, check->msecs % 1000 );
   printf( "Num checks: %'lu\n", check->numChecks );
   printf( "Num groups: %'lu\n", check->numGroups );
}


int main( int argc, char* argv[] ) {

    if ( argc != 2 ) {
        printf( "Not the correct number of arguments (1 expected)\n" );
        return 1;
    }
    setlocale(LC_NUMERIC, "");

    g_outputFile = fopen( "outputs/output.csv", "w" );
    fputs( "Id,Length,Number of Groups,Number of Rolls,Order Groups,Remaining Rolls,Average Remaining Roll Length\n", g_outputFile );
    char *fileName = argv[1];

    struct OrderStats *orderStats = malloc( sizeof( struct OrderStats ) );
    readRollFileIntoOrderStats( fileName, orderStats );

    orderStats->minOrderLength   = 1800;
    orderStats->maxOrderLength   = 2000;
    orderStats->maxRollsPerGroup = 8;
    orderStats->minGroupLength   = 250;
    orderStats->maxGroupLength   = 350;
    orderStats->minGroupsPerOrder = ceil( orderStats->minOrderLength / orderStats->maxGroupLength );
    orderStats->maxGroupsPerOrder = floor( orderStats->maxOrderLength / orderStats->minGroupLength );

    setMinMaxRollStats( orderStats ); //rolls are now sorted in descending length order
    sortRollsAscending( orderStats->rollList, orderStats->numberOfRolls);

    printf( "Number of rolls inputted: %i\n", orderStats->numberOfRolls );
    printf( "Minimum number of rolls needed to make group: %i\n", orderStats->minRollsPerGroup );
    printf( "Minimum number of rolls needed to make order: %i\n", orderStats->minRollsPerOrder );
    printf( "Maximum number of rolls needed to make order: %i\n", orderStats->maxRollsPerOrder );
    printf( "Minimum number of groups to make an order: %i\n", orderStats->minGroupsPerOrder );
    printf( "Maximum number of groups to make an order: %i\n", orderStats->maxGroupsPerOrder );
    printf( "Generating list of groups..." );

    struct IntArray *groupArray            = createIntArray( 1024, 0, 2 ); //malloc( sizeof(unsigned int) * 1024 );
    struct IntArray **groupsWithRollBySize = malloc( sizeof( struct IntArray* ) * orderStats->numberOfRolls );
    
    clock_t start = clock(), diff; 
    setNumGroupsPerRoll( orderStats );
    sortRollsByNumGroups( orderStats );
    groupArray           = setGroupArray( orderStats, groupArray );
    groupsWithRollBySize = setGroupsWithRollBySize( groupsWithRollBySize, groupArray, orderStats->numberOfRolls );

    struct check_t *normalPairsCheck = checkNormalPairs( groupArray );
    struct check_t *biasedBySizeCheck  = checkUnbiasedIntArray( groupArray, groupsWithRollBySize, orderStats->numberOfRolls );

    printCheck( "Normal Pairs", normalPairsCheck );
    printCheck( "Biased by Size", biasedBySizeCheck );

    diff = clock() - start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    printf( "Took %i seconds, %i millis\n", msec/1000, msec%1000 );
    printf( "Done!\nFound %'d groups\nGenerating potential orders...", groupArray->length );
    fflush( stdout );

    int numPotentialOrders = getNumPotentialOrders( orderStats );

    printf( "Done!\nFound %'d potential orders\n", numPotentialOrders );

    //recursiveSolve( 0, groups, 0, groupsWithRoll, minGroupsInOrder, minOrderLength, maxOrderLength, smallArray, &numFound, numPotentialOrders );

    fclose( g_outputFile );
    freeIntArray( groupArray );
    for ( int i = 0; i < orderStats->numberOfRolls; i++ ) {
        freeIntArray( groupsWithRollBySize[i] );
    }

    return 0;
}
