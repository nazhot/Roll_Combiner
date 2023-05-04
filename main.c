#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct Roll {
    char id[15];
    float length;
};

int rollsCount( unsigned int num, int numRolls ) {
    int count = 0;
    for ( int i = 0; i < numRolls; i++ ) {
        count += num >> i & 1;
    }
    return count;
}

float rollsLength( struct Roll *rolls, unsigned int num, int numRolls ) {
    float totalLength = 0;
    for ( int i = 0; i < numRolls; i++ ) {
        if ( num >> i & 1 ) {
            totalLength += rolls[i].length;
        }
    }
    return totalLength;
}

int main( int argc, char* argv[] ) {

    if ( argc != 2 ) {
        printf( "Not the correct number of arguments (1 expected)\n" );
        return 1;
    }

    FILE *p_rollFile;
    const char *fileName = argv[1];

    p_rollFile = fopen( fileName, "r" );

    if ( p_rollFile == NULL ) {
        printf( "Not able to find file: %s\n", fileName );
        return 2;
    }

    int maxNumberOfRolls = sizeof(int) * 8 ; //chosen to line up with int, 32 on my system
    struct Roll rollList[maxNumberOfRolls];
    int numberOfRolls = 0;
    int maxFileLineLength = 100;
    char fileLine[maxFileLineLength];

    while ( fgets( fileLine, maxFileLineLength, p_rollFile ) ) {
        struct Roll roll;
        for ( int i = 0; fileLine[i] != '\0'; i++ ) {
            if ( fileLine[i] == ':' ) {
                int colonIndex = i;
                char id[colonIndex + 1];
                strncpy( id, fileLine, colonIndex );
                id[colonIndex] = '\0';
                strncpy( roll.id, id, colonIndex + 1 );
                char lengthString[100];
                strncpy( lengthString, fileLine + colonIndex + 1, 15 ); 
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

    
    unsigned int maxNumber = 0;
    for ( int i = 0; i < numberOfRolls; i++ ) { //used this in place of pow. maxNumber will be all 1's, with numberOfRolls being how many 1's there are
        maxNumber = maxNumber << 1;
        maxNumber += 1;
    }

    for ( unsigned int i = 1; i <= maxNumber; i++ ) {
        printf( "%u: %.2f\n", i, rollsLength( rollList, i, numberOfRolls ) );
    }

    return 0;
}
