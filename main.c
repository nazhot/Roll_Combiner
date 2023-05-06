#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ID_LEN 15 //how long the id in each roll can be
                      //guarded by strncpy

struct Roll {
    char id[MAX_ID_LEN];
    float length;
};

/*
 * get the number of rolls contained within the given number
 * a roll is represented by a bit being set to 1
 *      num:      number representation of the set of rolls
 *      numRolls: maximum number of bits to check
*/
int rollsCount( unsigned int num, int numRolls ) {
    int count = 0;
    for ( int i = 0; i <= numRolls; i++ ) {
        count += num >> i & 1;
    }
    return count;
}


/*
 * get the total length of a set of rolls
 *      rolls:    array of Rolls, that have length and id information
 *      num:      number representation of the set of rolls
 *      numRolls: maximum number of bits to check
*/
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

    int         maxNumberOfRolls           = sizeof(int) * 8 ; //chosen to line up with int, 32 on my system
    int         numberOfRolls              = 0;
    int         maxFileLineLength          = 100;
    
    char        fileLine[maxFileLineLength];
    struct Roll rollList[maxNumberOfRolls];
    float       ascendingLengthsArray[maxNumberOfRolls];

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

                //add to the list of roll lengths, in ascending order
                for ( int j = 0; j < maxNumberOfRolls; j++ ) {
                    if ( j == numberOfRolls ) {
                        ascendingLengthsArray[j] = length;
                        break;
                    }

                    if ( length < ascendingLengthsArray[j] ) {
                        for ( int k = numberOfRolls; k > j; k-- ) {
                            ascendingLengthsArray[k] = ascendingLengthsArray[k - 1];
                        }
                        ascendingLengthsArray[j] = length;
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

    float        minOrderLength        = 1800; //meters
    float        maxOrderLength        = 2000; //meters
    int          maxSplices            = 7;    //how many splices allowed when putting rolls together for a group
    int          minGroupLength        = 250;  //meters
    int          maxGroupLength        = 350;  //meters
    unsigned int maxNumber             = 0;    //number where all bits are 1, with the number of bits being equal to the number of rolls
    float        tempLengthSum         = 0;    //accumulates the length
    int          minRollsInGroup       = 0;    //minimum number of rolls needed to form a group
    int          minRollsInOrder       = 0;    //minimum number of rolls needed to form an order
    unsigned int numberOfGroups        = 0;    //how many total groups are found
    unsigned int numberOfOrders        = 0;    //how many total orders are found (doens't check for valid groups)
    int          currentMaxRoll        = 0;    //the current highest roll in the loop

    unsigned int groupsContainRoll[numberOfRolls]; 

    //set up maxNumber, and initialize groupContainsRoll array
    for ( int i = 0; i < numberOfRolls; i++ ) { 
        maxNumber            = maxNumber << 1; 
        maxNumber           += 1;
        groupsContainRoll[i] = 0;
    }

    //figure out the minimum required rolls for a group/order
    for ( int i = numberOfRolls - 1; i >= 0; i-- ) {
        if ( tempLengthSum >= minOrderLength ) {
            break;
        }
        if ( tempLengthSum < minGroupLength ) {
            minRollsInGroup++;
        }
        tempLengthSum += ascendingLengthsArray[i];
        minRollsInOrder++;
    }

    printf( "Minimum number of rolls needed to make group: %i\n", minRollsInGroup );
    printf( "Minimum number of rolls needed to make order: %i\n", minRollsInOrder );

    clock_t start = clock(), diff; 
    for ( unsigned int i = 1; i <= maxNumber; i++ ) {
        //if the bit at currentMaxRoll + 1 is 1, that means we've gone up a further spot
        if ( i >> ( currentMaxRoll + 1 ) & 1 ) {
            printf( "Updating current max roll from %i to %i, on %u\n", currentMaxRoll, currentMaxRoll + 1, i );
            printf( "Current group/order count: %u/%u\n", numberOfGroups, numberOfOrders );
            currentMaxRoll++;
        }

        int numRollsInNumber = rollsCount( i, currentMaxRoll );

        //if there aren't even enough rolls to make a group, no need to check
        if ( numRollsInNumber < minRollsInGroup ) { 
            continue;
        }

        //too many rolls to be a group, too little to be an full order
        if ( numRollsInNumber > maxSplices + 1 && numRollsInNumber < minRollsInOrder ) {
            continue;
        }

        //total length of the rolls within the current number
        float length = rollsLength( rollList, i, currentMaxRoll );

        if ( numRollsInNumber <= maxSplices + 1 ) {
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
    }
    printf( "%d second %d milliseconds\n", msec/1000, msec%1000 );
    return 0;
}
