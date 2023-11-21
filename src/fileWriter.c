#include "fileWriter.h"
#include <stdio.h>
#include <stdlib.h>
#include "group.h"
#include "roll.h"

FILE* createOutputFile( char *fileName ) {
    FILE *outputFile = fopen( fileName, "w" );
    
    if ( !outputFile ) {
        printf( "Could not create/open %s as the output file, exiting\n", fileName ); 
        exit( 1 );
    }

    fputs( "Id,Length,Number of Groups,Number of Rolls,Order Groups,Remaining Rolls,Average Remaining Roll Length\n", outputFile );
    return outputFile;
}

void writeOrderToFile( FILE *outputFile, unsigned int order, unsigned int *groups, int numGroups, struct OrderStats *orderStats ) {
    fprintf( outputFile, "%u,", order );
    fprintf( outputFile, "%.1f,", rollsLength( order, orderStats->numberOfRolls, orderStats->rollList ) );
    fprintf( outputFile, "%i,", numGroups );
    fprintf( outputFile, "%i,", __builtin_popcount( order ) );
    fputc( '\"', outputFile ); //double quotes are used to allow for new lines

    for ( int i = 0; i < numGroups; ++i ) {
        unsigned int group = groups[i];
        float groupLength  = rollsLength( group, orderStats->numberOfRolls, orderStats->rollList );
        fprintf( outputFile, "%.1f (%i)\n", groupLength, __builtin_popcount( group ) );
        for ( int j = 0; j < orderStats->numberOfRolls; ++j ) {
            if ( !( group >> j & 1 ) ) {
                continue;
            }
            fprintf( outputFile, "%s: %.1f\n", orderStats->rollList[j].id, orderStats->rollList[j].length );
        }
        fputc( '\n', outputFile );
    }
    fputs( "\",", outputFile );

    int numberOfUnusedRolls = orderStats->numberOfRolls - __builtin_popcount( order );
    if ( numberOfUnusedRolls == 0 ) {
        fputs( "0\n", outputFile );
    } else {
        float unusedRollsLength = 0;

        fputc( '\"', outputFile );
        for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
            if ( order >> i & 1 ) {
                continue;
            }
            unusedRollsLength += orderStats->rollList[i].length;
            fprintf( outputFile, "%s: %.1f", orderStats->rollList[i].id, orderStats->rollList[i].length );
            if ( i < orderStats->numberOfRolls - 2 ) {
                fputc( '\n', outputFile );
            }
        }
        fprintf( outputFile, "\",%.1f\n", unusedRollsLength / numberOfUnusedRolls );
    }
}
