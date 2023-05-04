#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Roll {
    char* id;
    float length;
};


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

    int numberOfRolls = 3;
    int maxSplices = 4;
    int maxRollsPerGroups = 4;
    int maxFileLineLength = 100;
    struct Roll Rolls[numberOfRolls];

    char fileLine[maxFileLineLength];

    while ( fgets( fileLine, maxFileLineLength, p_rollFile ) ) {

    }


    return 0;
}
