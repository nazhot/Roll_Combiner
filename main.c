#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <locale.h>
#include "smallarray.h"
#include "intArray.h"

#define MAX_ID_LEN 15 //how long the id in each roll can be
                      //guarded by strncpy


FILE *g_outputFile;


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

unsigned nextSetOfNBits( unsigned x ) {
    unsigned smallest, ripple, new_smallest, ones;

    if ( x == 0 ) return 0;
    smallest     = ( x & -x );
    ripple       = x + smallest;
    new_smallest = ( ripple & -ripple );
    ones         = ( ( new_smallest / smallest ) >> 1 ) - 1;
    return ripple | ones;

}

int getSmallestIntArrayIndex( unsigned int currentGroup, int numRolls, struct int_array **groupsWithRoll ) {
    int smallestLength = INT32_MAX;
    int smallestLengthIndex = -1;
    for ( int i = 0; i < numRolls; i++ ) {
        if ( !( currentGroup >> i & 1 ) ) {
            continue;
        }
        if ( groupsWithRoll[i]->length < smallestLength ) {
            smallestLength = groupsWithRoll[i]->length;
            smallestLengthIndex = i;
        }
    }
    return smallestLengthIndex;
}

void recursiveSolve( unsigned int currentGroup, int numGroupsInOrder, int numRolls, struct int_array **groupsWithRoll, int minGroupsInOrder, int minOrderLength, int maxOrderLength, struct Roll *rolls, struct smallarray *alreadyFound, int *numFound ) {
    //Id, Length, Number of Groups, Number of Rolls, Order Groups,Remaining Rolls, Average Remaining Roll Length
    if ( getSmallArrayValue( alreadyFound, currentGroup ) ) {
        return;
    }

    setSmallArrayValue( alreadyFound, currentGroup );

    if ( numGroupsInOrder >= minGroupsInOrder ) {
        float currentLength = rollsLength( rolls, currentGroup, numRolls );
        if ( currentLength >= minOrderLength && currentLength <= maxOrderLength ) {
            *numFound += 1;
            //printf( "Found %i\n", *numFound );

            char orderNumber[256];
            sprintf( orderNumber, "%d", currentGroup );
            fputs( orderNumber, g_outputFile );
            fputc( ',', g_outputFile );

            char orderLength[256];
            sprintf( orderLength, "%f", currentLength );
            fputs( orderLength, g_outputFile );
            fputc( ',', g_outputFile ); 

            char numGroups[256];
            sprintf( numGroups, "%d", numGroupsInOrder );
            fputs( numGroups, g_outputFile );
            fputc( ',', g_outputFile );

            char numberOfRolls[256];
            sprintf( numberOfRolls, "%d", rollsCount( currentGroup, numRolls ) );
            fputs( numberOfRolls, g_outputFile );
            fputc( ',', g_outputFile );

            //add Order Groups
            //add Remaining Rolls
            //add Average Remaining Roll Length
            fputc( ',', g_outputFile );
            fputc( ',', g_outputFile );
            fputc( '\n', g_outputFile );
        }
        if ( currentLength >= maxOrderLength ) {
            return;
        }
    }

    int numValidGroups = 0; //how many groups don't share any rolls with currentGroup
    int numValidRolls  = 0; //how many rolls aren't already a part of currentGroup
                            //
    for ( int i = 0; i < numRolls; i++ ) {
        if ( !( currentGroup >> i & 1 ) ) {
            numValidGroups += groupsWithRoll[i]->length;
            numValidRolls++;
        }
    }

    if ( numValidRolls == 0 || numValidGroups == 0 ) { //prevents divide by 0 error, and is also a valid end point for the function
        return;
    }

    int averageGroupsPerRoll             = numValidGroups / numValidRolls + 1;
    struct int_array **newGroupsWithRoll = malloc( sizeof( struct int_array* ) * numRolls ); //holds the groups that can go with currentGroup

    for ( int i = 0; i < numRolls; i++ ) { //initialize the int_arrays
        newGroupsWithRoll[i] = createIntArray( averageGroupsPerRoll, 0, 1.1 );
    }

    for ( int i = 0; i < numRolls; i++ ) {
        if ( currentGroup >> i & 1 ) {
            continue; 
        }
        for ( int j = 0; j < groupsWithRoll[i]->length; j++ ) {
            if ( currentGroup & groupsWithRoll[i]->content[j] ) { //make sure group could be added
                continue;
            }
            int index = getSmallestIntArrayIndex( groupsWithRoll[i]->content[j], numRolls, newGroupsWithRoll );
            newGroupsWithRoll[index] = addToIntArray( newGroupsWithRoll[index], groupsWithRoll[i]->content[j] );
        }
    }

    for ( int i = 0; i < numRolls; i++ ) {
        if ( !currentGroup ) {
            printf( "%d\n", i );
        }
        if ( currentGroup >> i & 1  ) {
            continue;
        }
        for ( int j = 0; j < newGroupsWithRoll[i]->length; j++ ) {
            recursiveSolve( currentGroup | newGroupsWithRoll[i]->content[j], numGroupsInOrder + 1, numRolls, newGroupsWithRoll, minGroupsInOrder, minOrderLength, maxOrderLength, rolls, alreadyFound, numFound );
        }
    }

    for ( int i = 0; i < numRolls; i++ ) {
        freeIntArray( newGroupsWithRoll[i] );
    }

    free( newGroupsWithRoll );
}

int main( int argc, char* argv[] ) {

    if ( argc != 2 ) {
        printf( "Not the correct number of arguments (1 expected)\n" );
        return 1;
    }
    setlocale(LC_NUMERIC, "");

    FILE *p_rollFile;
    g_outputFile = fopen( "output.csv", "w" );
    fputs( "Id,Length,Number of Groups,Number of Rolls,Order Groups,Remaining Rolls,Average Remaining Roll Length\n", g_outputFile );
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
    struct int_array *groupArray       = createIntArray( 1024, 0, 2 ); //malloc( sizeof(unsigned int) * 1024 );
    struct int_array **groupsWithRoll  = malloc( sizeof( struct int_array* ) * numberOfRolls );


    //set up maxNumber, and initialize groupContainsRoll array
    for ( int i = 0; i < numberOfRolls; i++ ) { 
        tempLengthSum += ascendingLengthsArray[i];
        if ( tempLengthSum <= maxOrderLength ) {
            maxRollsInOrder++;
        }
        groupsWithRoll[i] = createIntArray( 10000, 0, 1.1 );
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
    //go through all possible combinations of rolls, from the minimum to maximum to create a group
    //when a valid group is found, it will add it to a general array for all groups,
    //as well as an array of arrays, with the array at index is made up of groups that contain (index) rolls
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

            if ( groupLength < minGroupLength || groupLength > maxGroupLength ) {
                continue;
            }

            int minBalancedLength = INT32_MAX;
            int minBalancedIndex = -1;
            for ( int j = 0; j <= currentMaxRoll; j++ ) { 
                if ( !( groupRolls >> j & 1 ) ) {
                    continue;
                }
                if ( groupsWithRoll[j]->length < minBalancedLength ) {
                    minBalancedLength = groupsWithRoll[j]->length;
                    minBalancedIndex = j;
                }
            }
            groupsWithRoll[minBalancedIndex] = addToIntArray( groupsWithRoll[minBalancedIndex], groupRolls );
            groupArray = addToIntArray( groupArray, groupRolls ); 
        } while ( incrementArray( rollsInGroupArray, groupSize, numberOfRolls - 1 ) );
    }

    int smallArraySize            = ( 1 << numberOfRolls ) - 1;
    int numFound                  = 0;
    struct smallarray *smallArray = createSmallArray( smallArraySize );

    recursiveSolve( 0, 1, numberOfRolls, groupsWithRoll, minGroupsInOrder, minOrderLength, maxOrderLength, rollList, smallArray, &numFound );

    fclose( g_outputFile );

    diff = clock() - start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;

    printf( "%d second %d milliseconds\n", msec/1000, msec%1000 );

    free( groupArray );
    freeIntArray( groupArray );
    for ( int i = 0; i < numberOfRolls; i++ ) {
        freeIntArray( groupsWithRoll[i] );
    }
    free( groupsWithRoll );

    return 0;
}
