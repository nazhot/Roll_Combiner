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
#include "fileWriter.h"
                      //guarded by strncpy
#define MAX_NUM_ROLLS 32;

int main( int argc, char* argv[] ) {
    if ( argc != 2 ) {
        printf( "Not the correct number of arguments (1 expected)\n" );
        return 1;
    }

    setlocale(LC_NUMERIC, "");

           char       *fileName   = argv[1];
    struct OrderStats *orderStats = readRollFile( fileName );
           FILE       *outputFile = createOutputFile( "outputs/output.csv" );

    orderStats->minOrderLength    = 1800;
    orderStats->maxOrderLength    = 2000;
    orderStats->maxRollsPerGroup  = 8;
    orderStats->minGroupLength    = 250;
    orderStats->maxGroupLength    = 350;
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
    struct IntArray **groupsWithRoll = getGroupsWithRollBySize( orderStats );

    printf( "Done!\nFound %'d groups\nGenerating potential orders...", orderStats->numberOfGroups );
    fflush( stdout );

    int *ordersWithRoll = getOrdersWithRoll( orderStats );
    printf( "Done!\nFound %'d potential orders\n", orderStats->numberOfPotentialOrders );

    nonRecursiveSolve( groupsWithRoll, orderStats, ordersWithRoll, outputFile );

    fclose( outputFile );
    for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
        freeIntArray( groupsWithRoll[i] );
    }

    free( orderStats->rollList );
    free( orderStats );
    free( groupsWithRoll );
    free( ordersWithRoll );

    return 0;
}
