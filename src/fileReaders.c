#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "roll.h"
#include "fileReaders.h"

struct OrderStats* readRollFile( char *rollFilePath ) {
           FILE *p_rollFile;
           int   maxNumberOfRolls  = sizeof(int) * 8 ;
           int   maxFileLineLength = 100;
           char  fileLine[maxFileLineLength];
    struct Roll *rollList = malloc( sizeof( struct Roll ) * maxNumberOfRolls );
           int   numberOfRolls = 0;
    struct OrderStats *orderStats = malloc( sizeof( struct OrderStats ) );

    p_rollFile = fopen( rollFilePath, "r" );

    if ( p_rollFile == NULL ) {
        printf( "Not able to find file: %s\n", rollFilePath );
        exit( 2 );
    }

    while ( fgets( fileLine, maxFileLineLength, p_rollFile ) ) {
        struct Roll roll;
        for ( int i = 0; fileLine[i] != '\0'; i++ ) {
            if ( fileLine[i] == ':' ) {
                char id[i + 1]; //+1 because of null terminator
                char lengthString[MAX_ID_LEN];

                strncpy( id, fileLine, i );
                id[i] = '\0';
                strncpy( roll.id, id, i + 1 );

                strncpy( lengthString, fileLine + i + 1, MAX_ID_LEN ); 
                lengthString[MAX_ID_LEN - 1] = '\0';
                float length = atof( lengthString );

                roll.length = length;
            }
        }
        if ( numberOfRolls == maxNumberOfRolls ) {
            printf( "You attempted to add more than the maximum allowable rolls (%d)!\nRunning as if you only input %d.\n", maxNumberOfRolls, maxNumberOfRolls );
            break;
        }
        rollList[numberOfRolls++] = roll;
    }

    orderStats->rollList = rollList;
    orderStats->numberOfRolls = numberOfRolls;
    return orderStats;
}
