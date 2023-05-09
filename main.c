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

struct AdjacencyNode {
    int rollNumber;
    struct AdjacencyNode *next;
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
/*
    unsigned int numEdges = 0;
    for ( int i = 0; i < numberOfGroups; i++ ) {
        unsigned int group1 = groupArray[i];
        for ( int j = i + 1; j < numberOfGroups; j++ ) {
            unsigned int group2 = groupArray[j];
            if ( ( group1 & group2 ) != 0 ) {
                continue;
            }
            numEdges++;
        }
    }   

    printf( "Total edges found: %u\n", numEdges );
*/


    unsigned int orderWith17Rolls = 330127329;
    unsigned int orderWith21Rolls = 11010047;
    unsigned int orderWith27Rolls = 503054335;
    int totalGroupsTest = 0;
    unsigned int *filteredGroups = malloc( sizeof( unsigned int ) * 1024 );
    int           filteredGroupSize = 1024;
    printf( "For an order with 27 rolls, the breakdown is:\n" );
    for ( int groupSize = minRollsInGroup; groupSize <= maxSplices + 1; groupSize++ ) {
        int numGroups = groupsWithXRollsCount[groupSize];
        int counter = 0;
        for ( int i = 0; i < numGroups; i++ ) {
            unsigned int group = groupsWithXRolls[groupSize][i];
            if ( ( orderWith27Rolls & group ) != group ) {
                continue;
            }
            if ( filteredGroupSize == totalGroupsTest ) {
                filteredGroupSize *= 2;
                filteredGroups = realloc( filteredGroups, sizeof( unsigned int ) * filteredGroupSize );
            }
            filteredGroups[totalGroupsTest] = group;
            totalGroupsTest++;
            counter++;
        }
        printf( "\t%i: %i\n", groupSize, counter );
    }
    printf( "Which is a total of %i groups\n", totalGroupsTest );
    unsigned int numEdges = 0;
    unsigned int numNonEdges = 0;
    for ( int i = 0; i < totalGroupsTest; i++ ) {
        unsigned int group1 = filteredGroups[i];
        for ( int j = i + 1; j < totalGroupsTest; j++ ) {
            unsigned int group2 = filteredGroups[j];
            if ( ( group1 & group2 ) != 0 ) {
                numNonEdges++;
                continue;
            }
            numEdges++;
        }
    }

    printf( "Number of edges with the filter: %u\n", numEdges ); 
    printf( "Number of non-edges with the filter: %u\n", numNonEdges ); 


    diff = clock() - start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;

    printf( "Total Groups when seperated: %u\n", totalGroupsCount );
    printf( "%d second %d milliseconds\n", msec/1000, msec%1000 );

    free(groupArray);

    return 0;
}
