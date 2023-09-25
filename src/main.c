#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <locale.h>
#include "fileReaders.h"
#include "smallArray.h"
#include "intArray.h"
#include "testMethods.h"
#include "roll.h"
#include "solver.h"
                      //guarded by strncpy
#define MAX_NUM_ROLLS 32;

FILE *g_outputFile;

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


//void recursiveSolve( unsigned int currentGroup, unsigned int *groups, int numGroupsInOrder, struct IntArray **groupsWithRoll, int minGroupsInOrder, int minOrderLength, int maxOrderLength, struct SmallArray *alreadyFound, int *numFound, int numPotentialOrders ) {
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

int main( int argc, char* argv[] ) {

    if ( argc != 2 ) {
        printf( "Not the correct number of arguments (1 expected)\n" );
        return 1;
    }
    setlocale(LC_NUMERIC, "");
    clock_t start = clock(), diff; 

    //g_outputFile = fopen( "outputs/output.csv", "w" );
    //fputs( "Id,Length,Number of Groups,Number of Rolls,Order Groups,Remaining Rolls,Average Remaining Roll Length\n", g_outputFile );
    char *fileName = argv[1];

    struct OrderStats *orderStats = readRollFile( fileName );

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
    
    setNumGroupsPerRoll( orderStats );
    sortRollsByNumGroups( orderStats );
    struct IntArray **groupsWithRollBySize = getGroupsWithRollBySize( orderStats );

    printf( "Done!\nFound %'d groups\nGenerating potential orders...", orderStats->numberOfGroups );
    fflush( stdout );

    int *ordersWithRoll = getOrdersWithRoll( orderStats );
    printf( "Done!\nFound %'d potential orders\n", orderStats->numberOfPotentialOrders );

    int recursiveStart = clock();
    //orderSolve( groupsWithRollBySize, orderStats, ordersWithRoll );
    nonRecursiveSolve( groupsWithRollBySize, orderStats, ordersWithRoll );
    int recursiveDiff = clock() - recursiveStart;
    int recursiveMsec = recursiveDiff * 1000 / CLOCKS_PER_SEC;
    printf( "Completed recursive loop, took %i seconds, %i millis\n", recursiveMsec/1000, recursiveMsec%1000 );

    diff = clock() - start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    printf( "Completed total program, took %i seconds, %i millis\n", msec/1000, msec%1000 );

    //fclose( g_outputFile );
    for ( int i = 0; i < orderStats->numberOfRolls; i++ ) {
        freeIntArray( groupsWithRollBySize[i] );
    }

    free( orderStats );
    free( groupsWithRollBySize );
    free( ordersWithRoll );

    return 0;
}
