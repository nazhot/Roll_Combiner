#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_ID_LEN 15 //how long the id in each roll can be
                      //guarded by strncpy


struct Roll {
    char id[MAX_ID_LEN];
    float length;
};


/*
 * increment an array through all possible numbers up to maxValue
 * array starts at [0, 1, 2, 3, ..., n]
 * array ends at   [maxValue - 3, maxValue - 2, maxValue - 1, maxValue]
 *      array:       array to increment
 *      arrayLength: how long the array is
 *      maxValue:    highest value the final element in the array will go to (inclusive)
*/
int incrementArray( unsigned int array[], int arrayLength, int maxValue ) {
    for ( int i = arrayLength - 1; i >= 0; i-- ) {
        int difference = (arrayLength - 1) - i;
        if ( array[i] < maxValue - difference ) {
            array[i] += 1;
            for ( int j = 1; i + j < arrayLength; j++ ) {
                array[i + j] = array[i] + j;
            }
            return 1;
        }
    }
    return 0;
}

int incrementInt( unsigned int *p_integer, int numRolls, int totalNumRolls ) {
    int integer = *p_integer;
    for ( int i = totalNumRolls - 1; i >= 0; i-- ) {
        if ( ! ( integer >> i & 1 ) ) {
            continue;
        }
        if ( i == totalNumRolls ) {
            for ( int j = 0; j < totalNumRolls; j++ ) {
                if ( integer >> j & 1 ) {
                    continue;
                }
                if ( j == totalNumRolls - numRolls ) { //array is at the end, can exit
                    return 0;
                }
            }
        }
    }
    return 0;
}


int groupsMakeUpOrder( unsigned int order, unsigned int groups[], unsigned int groupIndexes[], int groupIndexesLength ) {
    for ( int groupIndex = 0; groupIndex < groupIndexesLength; groupIndex++ ) {
        unsigned int group = groups[groupIndexes[groupIndex]];
       if ( ( order & group ) != group ) {
        return 0;
       }
       order = order ^ group;
    }
    return order == 0;
}

void printArray( unsigned int array[], int arrayLength ) {
    printf( "[" );
    for ( int i = 0; i < arrayLength; i++ ) {
       printf( "%u", array[i] ); 
       if ( i < arrayLength - 1 ) {
        printf( ", " );
       }
    }
    printf( "]\n" );
}

void printNumberBits( unsigned int num ) {
    printf( "%u: ", num );
    int numBits = sizeof( unsigned int ) * 8;
    for ( int i = 0; i < numBits; i++ ) {
        printf( "%i", num >> i & 1 );
    }
    printf( "\n" );
}

int numCombosThatMakeUpOrder( unsigned int order, unsigned int groups[], int minGroups, int maxGroups, int totalNumGroups ) {
    printf( "Before filtering: %u\n", totalNumGroups );
    int numCombos = 0;
    unsigned int filteredGroups[totalNumGroups];
    int          numFilteredGroups = 0;
    for ( int i = 0; i < totalNumGroups; i++ ) {
        unsigned int group = groups[i];
        if ( ( order & group ) == group ) {
            filteredGroups[numFilteredGroups] = group;
            numFilteredGroups++;
        }
    }
    printf( "After filtering: %u\n", numFilteredGroups ); 

    for ( int numGroups = minGroups; numGroups <= maxGroups; numGroups++ ) {
        printf( "Starting with numGroups = %i\n", numGroups );
        unsigned int groupIndexes[numGroups];
        for ( int i = 0; i < numGroups; i++ ) {
            groupIndexes[i] = i;
        }
        
        do {
            numCombos += groupsMakeUpOrder( order, filteredGroups, groupIndexes, numGroups );
            //printArray( groupIndexes, numGroups );
        } while ( incrementArray( groupIndexes, numGroups, numFilteredGroups ) );
    }
    return numCombos;
}


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
    float        minGroupLength        = 250;  //meters
    float        maxGroupLength        = 350;  //meters
    unsigned int maxNumber             = 0;    //number where all bits are 1, with the number of bits being equal to the number of rolls
    float        tempLengthSum         = 0;    //accumulates the length
    int          minRollsInGroup       = 0;    //minimum number of rolls needed to form a group
    int          minRollsInOrder       = 0;    //minimum number of rolls needed to form an order
    int          minGroupsInOrder      = ceil( minOrderLength / maxGroupLength );
    int          maxGroupsInOrder      = floor( maxOrderLength / minGroupLength );
    unsigned int numberOfGroups        = 0;    //how many total groups are found
    unsigned int numberOfOrders        = 0;    //how many total orders are found (doens't check for valid groups)
    int          currentMaxRoll        = 0;    //the current highest roll in the loop
    unsigned int *groupArray           = malloc( sizeof(unsigned int) * 1024 );
    int          groupArraySize        = 1024;

    unsigned int groupsContainRoll[numberOfRolls]; 
    unsigned int **groupsWithXRolls = malloc( (maxSplices + 1 ) * sizeof( int* ) );
    unsigned int groupsWithXRollsCount[maxSplices + 1];
    for ( int i = 0; i < maxSplices + 1; i++ ) {
        groupsWithXRollsCount[i] = 0;
        groupsWithXRolls[i] = malloc( sizeof( unsigned int ) * 1024 );
    }

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
    printf( "Minimum number of groups to make an order: %i\n", minGroupsInOrder );
    printf( "Maximum number of groups to make an order: %i\n", maxGroupsInOrder );


    clock_t start = clock(), diff; 
    //changed so that it is only gathering groups now
    for ( unsigned int i = 1; i <= maxNumber; i++ ) {
        //if the bit at currentMaxRoll + 1 is 1, that means we've gone up a further spot
        if ( i >> ( currentMaxRoll + 1 ) & 1 ) {
            printf( "Updating current max roll from %i to %i, on %u\n", currentMaxRoll, currentMaxRoll + 1, i );
            currentMaxRoll++;
        }

        int numRollsInNumber = rollsCount( i, currentMaxRoll );

        //if there aren't even enough rolls to make a group, no need to check
        if ( numRollsInNumber < minRollsInGroup ) { 
            continue;
        }

        //too many rolls to be a group, too little to be an full order
        if ( numRollsInNumber > maxSplices + 1 ) { // && numRollsInNumber < minRollsInOrder ) {
            continue;
        }

        //total length of the rolls within the current number
        float length = rollsLength( rollList, i, currentMaxRoll );

        if ( length >= minGroupLength && length <= maxGroupLength ) {
            for ( int j = 0; j <= currentMaxRoll; j++ ) { 
                if ( i >> j & 1) {
                    groupsContainRoll[j]++; 
                }
            }
            groupsWithXRollsCount[numRollsInNumber]++;
            if ( numberOfGroups == groupArraySize ) {
                groupArraySize *= 2;
                groupArray = realloc( groupArray, sizeof(unsigned int) * groupArraySize );
            }
            groupArray[numberOfGroups] = i;
            numberOfGroups++;
        }
    }
    diff = clock() - start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    printf( "Total Number of Possible Combos: %u\n", maxNumber );
    printf( "Total Number of Actual Groups: %u\n", numberOfGroups );
    printf( "Total Number of Actual Orders: %u\n", numberOfOrders );

    printf( "-------------------------------------------\n" );

    unsigned int totalGroupsCount = 0;
    for ( int i = 0; i < numberOfRolls; i++ ){
        printf( "Number of groups that contain roll %i: %u\n", i, groupsContainRoll[i] );
        totalGroupsCount += groupsContainRoll[i];
        if (i < maxSplices + 1) {
            printf( "Number of groups with %i rolls: %u\n", i, groupsWithXRollsCount[i] );
        }
    }

    printf( "Total Groups when seperated: %u\n", totalGroupsCount );
    printf( "%d second %d milliseconds\n", msec/1000, msec%1000 );


    free(groupArray);

    return 0;
}
