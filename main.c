#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

struct Roll {
    char id[15];
    float length;
};

int rollsCount( unsigned int num, int numRolls ) {
    int count = 0;
    for ( int i = 0; i <= numRolls; i++ ) {
        count += num >> i & 1;
    }
    return count;
}

float rollsLength( struct Roll *rolls, unsigned int num, int numRolls ) {
    float totalLength = 0;
    for ( int i = 0; i <= numRolls; i++ ) {
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
    float ascendingLengthsArray[maxNumberOfRolls];
    int ascendingLengthsArrayIndex = 0;
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

                //add to the list of roll lengths, in ascending order
                for ( int j = 0; j < maxNumberOfRolls; j++ ) {
                    if ( j == ascendingLengthsArrayIndex ) {
                        ascendingLengthsArray[j] = length;
                        ascendingLengthsArrayIndex++;
                        break;
                    }

                    if ( length < ascendingLengthsArray[j] ) {
                        for ( int k = ascendingLengthsArrayIndex; k > j; k-- ) {
                            ascendingLengthsArray[k] = ascendingLengthsArray[k - 1];
                        }
                        ascendingLengthsArray[j] = length;
                        ascendingLengthsArrayIndex++;
                        break;
                    }
                }
                roll.length = length;
            }
        }
        if ( numberOfRolls == maxNumberOfRolls ) {
            printf( "You attempted to add more than the maximum allowable rolls (%d)!\nRunning as if you only input %d.\n", maxNumberOfRolls, maxNumberOfRolls );
            break;
        }
        rollList[numberOfRolls++] = roll;
    }

    float minOrderLength = 1800;
    float maxOrderLength = 2000;
    int maxSplices = 7;
    int minGroupLength  = 250;
    int maxGroupLength  = 350;
    unsigned int groupsContainRoll[numberOfRolls];
    unsigned int groupsDontContainRoll[numberOfRolls];
    
    unsigned int maxNumber = 0;


    for ( int i = 0; i < numberOfRolls; i++ ) { //used this in place of pow. maxNumber will be all 1's, with numberOfRolls being how many 1's there are
        maxNumber = maxNumber << 1;
        maxNumber += 1;
        groupsContainRoll[i] = 0;
        groupsDontContainRoll[i] = 0;
    }

    unsigned int numberOfGroups = 0;
    unsigned int numberOfOrders = 0;
    int currentMaxRoll = 0;
    clock_t start = clock(), diff; 
    for ( unsigned int i = 1; i <= maxNumber; i++ ) {
        if ( i >> ( currentMaxRoll + 1 ) & 1 ) {
            printf( "Updating current max roll from %i to %i, on %u\n", currentMaxRoll, currentMaxRoll + 1, i );
            currentMaxRoll++;
        }
        float length = rollsLength( rollList, i, currentMaxRoll );
        if ( rollsCount( i , currentMaxRoll ) <= maxSplices + 1 ) {
            if ( length >= minGroupLength && length <= maxGroupLength ) {
                for ( int j = 0; j <= currentMaxRoll; j++ ) { 
                    if ( i >> j & 1) {
                        groupsContainRoll[j]++; 
                    }
                }
                numberOfGroups++;
            }
            continue;
        }
        if ( length >= minOrderLength && length <= maxOrderLength ) {
            numberOfOrders++;
        }
    }
    diff = clock() - start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    printf( "Total Number of Possible Combos: %u\n", maxNumber );
    printf( "Total Number of Actual Groups: %u\n", numberOfGroups );
    printf( "Total Number of Actual Orders: %u\n", numberOfOrders );

    printf( "-------------------------------------------\n" );

    for ( int i = 0; i < numberOfRolls; i++ ){
        printf( "Number of groups that contain roll %i: %u\n", i, groupsContainRoll[i] );
        printf( "Number of groups that DON'T contain roll %i: %u\n", i, numberOfGroups - groupsContainRoll[i] );
    }
    printf( "%d second %d milliseconds\n", msec/1000, msec%1000 );
    return 0;
}
