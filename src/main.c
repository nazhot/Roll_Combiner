#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <locale.h>
#include <argp.h>
#include "fileReaders.h"
#include "smallArray.h"
#include "intArray.h"
#include "testMethods.h"
#include "roll.h"
#include "solver.h"
#include "fileWriter.h"

                      //guarded by strncpy
#define MAX_NUM_ROLLS 32;

struct ProgramSettings {
    char *inputFilePath;
    char *outputFilePath;
    int threaded;
    int verbose;
};


static int parse_opt( int key, char *arg, struct argp_state *state ) {
    struct ProgramSettings *settings = ( struct ProgramSettings* ) state->input;
    switch ( key ) {
        case 'o':
            settings->outputFilePath = arg;
            break;
        case 0:
            settings->inputFilePath = arg;
            break;
        case ARGP_KEY_END:
            if ( !settings->inputFilePath ) { 
                argp_failure( state, 1, 0, "Input file path not specified!" );
            }
            break;
    }
    return 0;
}


int main( int argc, char **argv ) {
    struct ProgramSettings settings = { NULL, "outputs/output.csv", 1, 1, };
    struct argp_option options[] = {
        { "output", 'o', "PATH", 0, "Specify the path to the output file" },
        { 0 }
    };

    struct argp argp = { options, parse_opt };

    argp_parse( &argp, argc, argv, 0, 0, &settings );
    setlocale(LC_NUMERIC, "");

    struct OrderStats *orderStats = readRollFile( settings.inputFilePath );
           FILE       *outputFile = createOutputFile( settings.outputFilePath );

    orderStats->minOrderLength    = 1800;
    orderStats->maxOrderLength    = 2000;
    orderStats->maxRollsPerGroup  = 8;
    orderStats->minGroupLength    = 250;
    orderStats->maxGroupLength    = 350;
    orderStats->minGroupsPerOrder = ceil( orderStats->minOrderLength / orderStats->maxGroupLength );
    orderStats->maxGroupsPerOrder = floor( orderStats->maxOrderLength / orderStats->minGroupLength );

    setMinMaxRollStats( orderStats ); //rolls are now sorted in descending length order
    sortRollsByLengthAscending( orderStats->rollList, orderStats->numberOfRolls);

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
    putc( '\n', stdout );

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
