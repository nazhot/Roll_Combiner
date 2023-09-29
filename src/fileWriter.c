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
    fprintf( outputFile, "%ui,", order );
    fprintf( outputFile, "%.1f,", rollsLength( order, orderStats->numberOfRolls, orderStats->rollList ) );
    fprintf( outputFile, "%i,", numGroups );
    fprintf( outputFile, "%i,", __builtin_popcount( order ) );
    
    fputc( '\"', outputFile );
    for ( int i = 0; i < numGroups; ++i ) {
        unsigned int group = groups[i];
        float groupLength  = rollsLength( group, orderStats->numberOfRolls, orderStats->rollList );
        fprintf( outputFile, "%.1f\n", groupLength );
        for ( int j = 0; j < orderStats->numberOfRolls; ++j ) {
            if ( !( group >> j & 1 ) ) {
                continue;
            }
            fprintf( outputFile, "%s: %.1f\n", orderStats->rollList[j].id, orderStats->rollList[j].length );
        }
        fputc( '\n', outputFile );
    }
    fputc( '\"', outputFile );


    //groups
    //remaining rolls
    //average remaining roll length
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
