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
#define MAX_NUM_ROLLS 32;

FILE *g_outputFile;
int   g_numberOfRolls;
struct Roll *g_rollList;


struct Roll {
    char id[MAX_ID_LEN];
    float length;
};


void printRollsFromInt(  unsigned int integer ) {
    printf( "----------GROUP----------\n" );
    float totalLength = 0;
    for ( int i = 0; i < g_numberOfRolls; i++ ) {
       if ( integer >> i & 1 ) {
           struct Roll roll = g_rollList[i];
           printf( "Roll %d: %s, %.2f\n", i, roll.id, roll.length );
           totalLength += roll.length;
       }
    }
    printf( "Total Length: %.2f\n", totalLength );
    printf( "-------------------------\n" );
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
int rollsCount( unsigned int num ) {
    int count = 0;
    for ( int i = 0; i <= g_numberOfRolls; i++ ) {
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
float rollsLength(  unsigned int num ) {
    float totalLength = 0;
    for ( int i = 0; i <= g_numberOfRolls; i++ ) {
        if ( num >> i & 1 ) {
            totalLength += g_rollList[i].length;
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

int getSmallestIntArrayIndex( unsigned int currentGroup, struct int_array **groupsWithRoll ) {
    int smallestLength = INT32_MAX;
    int smallestLengthIndex = -1;
    for ( int i = 0; i < g_numberOfRolls; i++ ) {
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

char* getGroupsString( unsigned int *groups, int numGroups ) {
   char *string = malloc( sizeof( char ) * 1024 ); 

   return string;
}



void recursiveSolve( unsigned int currentGroup, unsigned int *groups, int numGroupsInOrder, struct int_array **groupsWithRoll, int minGroupsInOrder, int minOrderLength, int maxOrderLength, struct smallarray *alreadyFound, int *numFound ) {
    //Id, Length, Number of Groups, Number of Rolls, Order Groups,Remaining Rolls, Average Remaining Roll Length
    if ( getSmallArrayValue( alreadyFound, currentGroup ) ) {
        return;
    }

    setSmallArrayValue( alreadyFound, currentGroup );

    if ( numGroupsInOrder >= minGroupsInOrder ) {
        float currentLength = rollsLength( currentGroup );
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
            sprintf( numberOfRolls, "%d", rollsCount( currentGroup ) );
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
    for ( int i = 0; i < g_numberOfRolls; i++ ) {
        if ( !( currentGroup >> i & 1 ) ) {
            numValidGroups += groupsWithRoll[i]->length;
            numValidRolls++;
        }
    }

    if ( numValidRolls == 0 || numValidGroups == 0 ) { //prevents divide by 0 error, and is also a valid end point for the function
        return;
    }

    int averageGroupsPerRoll             = numValidGroups / numValidRolls + 1;
    struct int_array **newGroupsWithRoll = malloc( sizeof( struct int_array* ) * g_numberOfRolls ); //holds the groups that can go with currentGroup

    for ( int i = 0; i < g_numberOfRolls; i++ ) { //initialize the int_arrays
        newGroupsWithRoll[i] = createIntArray( averageGroupsPerRoll, 0, 1.1 );
    }

    for ( int i = 0; i < g_numberOfRolls; i++ ) {
        if ( currentGroup >> i & 1 ) {
            continue; 
        }
        for ( int j = 0; j < groupsWithRoll[i]->length; j++ ) {
            if ( currentGroup & groupsWithRoll[i]->content[j] ) { //make sure group could be added
                continue;
            }
            int index = getSmallestIntArrayIndex( groupsWithRoll[i]->content[j], newGroupsWithRoll );
            newGroupsWithRoll[index] = addToIntArray( newGroupsWithRoll[index], groupsWithRoll[i]->content[j] );
        }
    }

    for ( int i = 0; i < g_numberOfRolls; i++ ) {
        if ( !currentGroup ) {
            printf( "%d\n", i );
        }
        if ( currentGroup >> i & 1  ) {
            continue;
        }
        for ( int j = 0; j < newGroupsWithRoll[i]->length; j++ ) {
            groups[numGroupsInOrder] = newGroupsWithRoll[i]->content[j];
            recursiveSolve( currentGroup | newGroupsWithRoll[i]->content[j], groups, numGroupsInOrder + 1, newGroupsWithRoll, minGroupsInOrder, minOrderLength, maxOrderLength, alreadyFound, numFound );
        }
    }

    for ( int i = 0; i < g_numberOfRolls; i++ ) {
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
    g_numberOfRolls = 0;
    fputs( "Id,Length,Number of Groups,Number of Rolls,Order Groups,Remaining Rolls,Average Remaining Roll Length\n", g_outputFile );
    const char *fileName = argv[1];

    p_rollFile = fopen( fileName, "r" );

    if ( p_rollFile == NULL ) {
        printf( "Not able to find file: %s\n", fileName );
        return 2;
    }

    int         maxNumberOfRolls           = sizeof(int) * 8 ; //chosen to line up with int, 32 on my system
    int         maxFileLineLength          = 100;

    char        fileLine[maxFileLineLength];
    g_rollList = malloc( sizeof( struct Roll ) * maxNumberOfRolls );
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
                    if ( j == g_numberOfRolls ) {
                        ascendingLengthsArray[j] = length;
                        break;
                    }

                    if ( length < ascendingLengthsArray[j] ) {
                        for ( int k = g_numberOfRolls; k > j; k-- ) {
                            ascendingLengthsArray[k] = ascendingLengthsArray[k - 1];
                        }
                        ascendingLengthsArray[j] = length;
                        break;
                    }
                }
                roll.length = length;
            }
        }
        if ( g_numberOfRolls == maxNumberOfRolls ) {
            printf( "You attempted to add more than the maximum allowable rolls (%d)!\nRunning as if you only input %d.\n", maxNumberOfRolls, maxNumberOfRolls );
            break;
        }
        g_rollList[g_numberOfRolls++] = roll;
    }

    float              minOrderLength        = 1800; //meters
    float              maxOrderLength        = 2000; //meters
    int                maxSplices            = 7;    //how many splices allowed when putting rolls together for a group
    float              minGroupLength        = 250;  //meters
    float              maxGroupLength        = 350;  //meters
    float              tempLengthSum         = 0;    //accumulates the length
    int                minRollsInGroup       = 0;    //minimum number of rolls needed to form a group
    int                minRollsInOrder       = 0;    //minimum number of rolls needed to form an order
    int                maxRollsInOrder       = 0;    //maximum number of rolls needed to form an order
    int                minGroupsInOrder      = ceil( minOrderLength / maxGroupLength );
    int                maxGroupsInOrder      = floor( maxOrderLength / minGroupLength );
    struct int_array  *groupArray            = createIntArray( 1024, 0, 2 ); //malloc( sizeof(unsigned int) * 1024 );
    struct int_array **groupsWithRoll        = malloc( sizeof( struct int_array* ) * g_numberOfRolls );


    //set up maxNumber, and initialize groupContainsRoll array
    for ( int i = 0; i < g_numberOfRolls; i++ ) { 
        tempLengthSum += ascendingLengthsArray[i];
        if ( tempLengthSum <= maxOrderLength ) {
            maxRollsInOrder++;
        }
        groupsWithRoll[i] = createIntArray( 10000, 0, 1.1 );
    }

    tempLengthSum = 0;
    //figure out the minimum required rolls for a group/order
    for ( int i = g_numberOfRolls - 1; i >= 0; i-- ) {
        if ( tempLengthSum >= minOrderLength ) {
            break;
        }
        if ( tempLengthSum < minGroupLength ) {
            minRollsInGroup++;
        }
        tempLengthSum += ascendingLengthsArray[i];
        minRollsInOrder++;
    }
    
    printf( "Number of rolls inputted: %i\n", g_numberOfRolls );
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
        int group         = ( 1 << groupSize ) - 1; //starts at the smallest possible number for a group with groupSize bits set
        int largestNumber = group << ( g_numberOfRolls - groupSize ); //largest number that could represent a group with groupSize bits set

        do {
            float groupLength = rollsLength(  group ); 

            if ( groupLength < minGroupLength || groupLength > maxGroupLength ) {
                group = nextSetOfNBits( group );
                continue;
            }
            
            int minSizeIndex = getSmallestIntArrayIndex( group, groupsWithRoll );

            groupsWithRoll[minSizeIndex] = addToIntArray( groupsWithRoll[minSizeIndex], group );
            groupArray                   = addToIntArray( groupArray, group );
            group                        = nextSetOfNBits( group );
        } while( group <= largestNumber );
    }

//    diff = clock() - start;
//    int msec = diff * 1000 / CLOCKS_PER_SEC;
//
//    printf( "%d second %d milliseconds\n", msec/1000, msec%1000 );
    printf( "%i groups found\n", groupArray->length );

    int smallArraySize            = ( 1 << g_numberOfRolls ) - 1;
    int numFound                  = 0;
    struct smallarray *smallArray = createSmallArray( smallArraySize );
    unsigned int *groups          = malloc( sizeof( unsigned int ) * g_numberOfRolls );


    recursiveSolve( 0, groups, 0, groupsWithRoll, minGroupsInOrder, minOrderLength, maxOrderLength, smallArray, &numFound );

    fclose( g_outputFile );
    free( groupArray );
    freeIntArray( groupArray );
    for ( int i = 0; i < g_numberOfRolls; i++ ) {
        freeIntArray( groupsWithRoll[i] );
    }
    free( groupsWithRoll );

    return 0;
}
