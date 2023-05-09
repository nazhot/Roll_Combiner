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


void printRollsFromInt( struct Roll *rolls, unsigned int integer, int numberOfRolls ) {
    printf( "----------GROUP----------\n" );
    float totalLength = 0;
    for ( int i = 0; i < numberOfRolls; i++ ) {
       if ( integer >> i & 1 ) {
           struct Roll roll = rolls[i];
           printf( "Roll %d: %s, %.2f\n", i, roll.id, roll.length );
           totalLength += roll.length;
       }
    }
    printf( "Total Length: %.2f\n", totalLength );
    printf( "-------------------------\n" );
}


/*
 * increment an array through all possible numbers up to maxValue
 * array starts at [0, 1, 2, 3, ..., n]
 * array ends at   [maxValue - 3, maxValue - 2, maxValue - 1, maxValue]
 *      array:       array to increment
 *      arrayLength: how long the array is
 *      maxValue:    highest value the final element in the array will go to (inclusive)
*/
int incrementArray( int array[], int arrayLength, int maxValue ) {
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

/*
 * go from an array where elements are the rolls that make up a group to the 
 *  equivalent unsigned int
 *      array:     elements are the rolls that make up a group
 *      arraySize: how many elements are in the array
*/
unsigned int arrayToInt( int array[], int arraySize ) {
    unsigned int integer = 0;
    for ( int i = 0; i < arraySize; i++ ) {
        int rollNumber = array[i];
        integer += 1 << rollNumber;
    }
    return integer;
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
    float        tempLengthSum         = 0;    //accumulates the length
    int          minRollsInGroup       = 0;    //minimum number of rolls needed to form a group
    int          minRollsInOrder       = 0;    //minimum number of rolls needed to form an order
    int          maxRollsInOrder       = 0;    //maximum number of rolls needed to form an order
    int          minGroupsInOrder      = ceil( minOrderLength / maxGroupLength );
    int          maxGroupsInOrder      = floor( maxOrderLength / minGroupLength );
    unsigned int numberOfGroups        = 0;    //how many total groups are found
    unsigned int *groupArray           = malloc( sizeof(unsigned int) * 1024 );
    int          groupArraySize        = 1024;

    unsigned int groupsContainRoll[numberOfRolls]; 
    unsigned int **groupsWithXRolls = malloc( (maxSplices + 2 ) * sizeof( int* ) );
    unsigned int groupsWithXRollsCount[maxSplices + 2];
    unsigned int groupsWithXRollsSize[maxSplices + 2];
    for ( int i = 0; i <= maxSplices + 1; i++ ) {
        groupsWithXRollsCount[i] = 0;
        groupsWithXRolls[i] = malloc( sizeof( unsigned int ) * 1024 );
        groupsWithXRollsSize[i] = 1024;
    }

    //set up maxNumber, and initialize groupContainsRoll array
    for ( int i = 0; i < numberOfRolls; i++ ) { 
        tempLengthSum += ascendingLengthsArray[i];
        if ( tempLengthSum <= maxOrderLength ) {
            maxRollsInOrder++;
        }
        groupsContainRoll[i] = 0;
    }

    tempLengthSum = 0;
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
    
    printf( "Number of rolls inputted: %i\n", numberOfRolls );
    printf( "Minimum number of rolls needed to make group: %i\n", minRollsInGroup );
    printf( "Minimum number of rolls needed to make order: %i\n", minRollsInOrder );
    printf( "Maximum number of rolls needed to make order: %i\n", maxRollsInOrder );
    printf( "Minimum number of groups to make an order: %i\n", minGroupsInOrder );
    printf( "Maximum number of groups to make an order: %i\n", maxGroupsInOrder );

    clock_t start = clock(), diff; 
    //changed so that it is only gathering groups now
    
    for ( int groupSize = minRollsInGroup; groupSize <= maxSplices + 1; groupSize++ ) {
        int rollsInGroupArray[groupSize];

        //initialize the array with each index = index
        for ( int i = 0; i < groupSize; i++ ){
            rollsInGroupArray[i] = i;
        }

        do {
            int currentMaxRoll      = rollsInGroupArray[groupSize - 1];
            unsigned int groupRolls = arrayToInt( rollsInGroupArray, groupSize );
            float groupLength       = rollsLength( rollList, groupRolls, currentMaxRoll );

            if ( groupLength >= minGroupLength && groupLength <= maxGroupLength ) {
                for ( int j = 0; j <= currentMaxRoll; j++ ) { 
                    if ( groupRolls >> j & 1) {
                        groupsContainRoll[j]++; 
                    }
                }
                
                if ( groupsWithXRollsCount[groupSize] == groupsWithXRollsSize[groupSize]) {
                    groupsWithXRollsSize[groupSize] *= 2;
                    groupsWithXRolls[groupSize] = realloc( groupsWithXRolls[groupSize], sizeof(unsigned int) * groupsWithXRollsSize[groupSize]);
                }
                groupsWithXRolls[groupSize][groupsWithXRollsCount[groupSize]] = groupRolls;
                groupsWithXRollsCount[groupSize]++;

                if ( numberOfGroups == groupArraySize ) {
                    groupArraySize *= 2;
                    groupArray = realloc( groupArray, sizeof(unsigned int) * groupArraySize );
                }

                groupArray[numberOfGroups] = groupRolls;
                numberOfGroups++;
            }
        } while ( incrementArray( rollsInGroupArray, groupSize, numberOfRolls - 1 ) );
    }

    diff = clock() - start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    printf( "Total Number of Actual Groups: %u\n", numberOfGroups );

    printf( "-------------------------------------------\n" );

    unsigned int totalGroupsCount = 0;
    for ( int i = 0; i < numberOfRolls; i++ ){
        printf( "Number of groups that contain roll %i: %u\n", i, groupsContainRoll[i] );
        totalGroupsCount += groupsContainRoll[i];
        if (i <= maxSplices + 1) {
            printf( "Number of groups with %i rolls: %u\n", i, groupsWithXRollsCount[i] );
        }
    }

    printf( "Total Groups when seperated: %u\n", totalGroupsCount );
    printf( "%d second %d milliseconds\n", msec/1000, msec%1000 );
    int totalOrders = 0;
    for ( int i = minRollsInOrder; i <= maxRollsInOrder; i++ ) {
        int numValidOrders = 0;
        int numRollsToTest = i;
        int orderIndexArray[numRollsToTest];
        for ( int i = 0; i < numRollsToTest; i++ ){
            orderIndexArray[i] = i;
        }


        do { 
            unsigned int order = arrayToInt( orderIndexArray, numRollsToTest ); 
            float orderLength = rollsLength( rollList, order, numberOfRolls );
            if ( orderLength >= minOrderLength && orderLength <= maxOrderLength ) {
                unsigned int *filteredGroups = malloc( sizeof( unsigned int ) * 1024 );
                int numberOfGroupsForOrder = 0;
                int filteredSize = 1024;
                for ( int j = 0; j < numberOfGroups; j++ ){
                   if ( ( order & groupArray[j] ) != groupArray[j] ) {
                        continue;
                   }
                   if ( numberOfGroupsForOrder == filteredSize ) {
                        filteredSize *= 2;
                        filteredGroups = realloc( filteredGroups, sizeof( unsigned int ) * filteredSize );
                   }
                   filteredGroups[numberOfGroupsForOrder] = groupArray[j];
                   numberOfGroupsForOrder++;
                }

                free(filteredGroups);
                //printf( "For first order with %i rolls, there are %i groups that could be in it\n", numRollsToTest, numberOfGroupsForOrder );
                //break;
                numValidOrders++;
            }

        } while ( incrementArray( orderIndexArray, numRollsToTest, numberOfRolls - 1 ) );

        printf( "Number of orders with %i rolls: %i\n", numRollsToTest, numValidOrders );
        totalOrders += numValidOrders;
    }
    printf( "Number of valid orders: %i\n", totalOrders );
    int numRepeatsToTest = 4;
    int numRollsToTest = 2;
    int testArray[numRepeatsToTest];
    int numGroupings = 0;
    unsigned int uniqueGroups[1024];
    int numUniqueGroups = 0;

    for ( int i = 0; i < numRepeatsToTest; i++ ) {
        testArray[i] = i;
    }

    do {
        unsigned int order = 0;
        int allUnique = 1;
        for ( int i = 0; i < numRepeatsToTest; i++ ) {
            unsigned int group = groupsWithXRolls[numRollsToTest][testArray[i]];
            if ( ( order & group ) != 0 ) {
                allUnique = 0;
                break;
            }
            order = order | group;
        }

        if ( !allUnique ) {
            continue;
        }
        numGroupings++;
        int alreadyMade = 0;
        for ( int i = 0; i < numUniqueGroups; i++ ) {
            if ( uniqueGroups[i] == order ) {
                alreadyMade = 1;
                break;
            }
        }

        if ( alreadyMade ) {
            continue;
        }
        uniqueGroups[numUniqueGroups] = order;
        numUniqueGroups++;

        //printRollsFromInt( rollList, order, numberOfRolls );
    } while ( incrementArray( testArray, numRepeatsToTest, groupsWithXRollsCount[numRollsToTest] - 1) );
    printf( "Number of valid groupings (not unique): %i\n", numGroupings );
    printf( "Number of valid groupings (unique): %i\n", numUniqueGroups );
    free(groupArray);

    return 0;
}
