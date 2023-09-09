#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <locale.h>
#include "smallarray.h"
#include "intArray.h"
#include "testMethods.h"

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

struct sortedSize_t {
    int size;
    int rollNumber;
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

int getSmallestIntArrayIndexWithBias( unsigned int currentGroup, int rollBias, struct int_array **groupsWithRoll ) {
    if ( currentGroup >> rollBias & 1 ) {
        return rollBias;
    }
    return getSmallestIntArrayIndex( currentGroup, groupsWithRoll );
}

void setGroupsString( unsigned int *groups, int numGroups ) {
    fputc( '\"', g_outputFile );
    for ( int i = 0; i < numGroups; i++ ){
        unsigned int group = groups[i];
        float groupLength = rollsLength( group );
        char groupLengthString[256];
        sprintf( groupLengthString, "%.2f", groupLength );

        fputs( "-\n", g_outputFile );

        for ( int j = 0; j < g_numberOfRolls; j++ ) {
            if ( !(  group >> j & 1  ) ) {
               continue; 
            }
            char rollLength[256];
            sprintf( rollLength, "%.2f", g_rollList[j].length ); 
            fputs( g_rollList[j].id, g_outputFile );
            fputs( ": ", g_outputFile );
            fputs( rollLength, g_outputFile );
            fputc( '\n', g_outputFile );
        }
        fputs( "-\n", g_outputFile ); 
    }
    fputc( '\"', g_outputFile );
}

void setRemainingRollsString( unsigned int order ) {
    int numRemainingRolls = 0;
    float remainingRollsLength = 0;
    float average;
    fputc( '\"', g_outputFile );
    for ( int i = 0; i < g_numberOfRolls; i++ ) {
        if ( order >> i & 1 ) {
            continue;
        }
        numRemainingRolls++;
        remainingRollsLength += g_rollList[i].length;
        char rollLength[256];
        sprintf( rollLength, "%.2f", g_rollList[i].length ); 
        fputs( g_rollList[i].id, g_outputFile );
        fputs( ": ", g_outputFile );
        fputs( rollLength, g_outputFile );
        fputc( '\n', g_outputFile );

    }
    fputs( "\",", g_outputFile );

    if ( numRemainingRolls == 0 ) {
        average = 0;
    } else {
        average = remainingRollsLength / numRemainingRolls;
    }

    char averageLengthRemaining[256];
    sprintf( averageLengthRemaining, "%.2f", average);
    fputs( averageLengthRemaining, g_outputFile );
    fputc( '\n', g_outputFile );
}


void recursiveSolve( unsigned int currentGroup, unsigned int *groups, int numGroupsInOrder, struct int_array **groupsWithRoll, int minGroupsInOrder, int minOrderLength, int maxOrderLength, struct smallarray *alreadyFound, int *numFound, int numPotentialOrders ) {
    //Id, Length, Number of Groups, Number of Rolls, Order Groups,Remaining Rolls, Average Remaining Roll Length
    if ( getSmallArrayValue( alreadyFound, currentGroup ) ) {
        return;
    }

    setSmallArrayValue( alreadyFound, currentGroup );

    if ( numGroupsInOrder >= minGroupsInOrder ) {
        float currentLength = rollsLength( currentGroup );
        if ( currentLength >= minOrderLength && currentLength <= maxOrderLength ) {
            *numFound += 1;
            printf( "\rOrders found: %'i/%'i (%.2f%%)", *numFound, numPotentialOrders, *numFound * 1.0 / numPotentialOrders * 100 );
            fflush( stdout );
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

            setGroupsString( groups, numGroupsInOrder );
            //add Order Groups
            //add Remaining Rolls
            //add Average Remaining Roll Length
            fputc( ',', g_outputFile );
            setRemainingRollsString( currentGroup );
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
        if ( currentGroup >> i & 1  ) {
            continue;
        }
        for ( int j = 0; j < newGroupsWithRoll[i]->length; j++ ) {
            groups[numGroupsInOrder] = newGroupsWithRoll[i]->content[j];
            recursiveSolve( currentGroup | newGroupsWithRoll[i]->content[j], groups, numGroupsInOrder + 1, newGroupsWithRoll, minGroupsInOrder, minOrderLength, maxOrderLength, alreadyFound, numFound, numPotentialOrders );
        }
    }

    for ( int i = 0; i < g_numberOfRolls; i++ ) {
        freeIntArray( newGroupsWithRoll[i] );
    }

    free( newGroupsWithRoll );
}

void printCheck( char title[], struct check_t *check ) {
   printf( "--------%s-------\n", title ); 
   printf( "Time: %i sec, %i msecs\n", check->msecs / 1000, check->msecs % 1000 );
   printf( "Num checks: %'lu\n", check->numChecks );
   printf( "Num groups: %'lu\n", check->numGroups );
}

int sortCompare( const void *a, const void *b ) {
    struct sortedSize_t tempA = *( struct sortedSize_t* ) a;
    struct sortedSize_t tempB = *( struct sortedSize_t* ) b;
    return tempB.size - tempA.size;
}


int main( int argc, char* argv[] ) {

    if ( argc != 2 ) {
        printf( "Not the correct number of arguments (1 expected)\n" );
        return 1;
    }
    setlocale(LC_NUMERIC, "");

    FILE *p_rollFile;
    g_outputFile = fopen( "outputs/output.csv", "w" );
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
    int                numPotentialOrders    = 0;
    struct int_array  *groupArray            = createIntArray( 1024, 0, 2 ); //malloc( sizeof(unsigned int) * 1024 );
    struct int_array **groupsWithRoll        = malloc( sizeof( struct int_array* ) * g_numberOfRolls );
    struct sortedSize_t allGroupsWithRoll[g_numberOfRolls];
    int                 allGroupsWithRollIndexes[g_numberOfRolls];
    struct int_array **biasedGroupsWithRoll[g_numberOfRolls];
    struct int_array **doubleBiasedGroupsWithRoll[g_numberOfRolls * g_numberOfRolls];
    struct int_array  *groupsWithRollBySize[g_numberOfRolls];


    //set up maxNumber, and initialize groupContainsRoll array
    for ( int i = 0; i < g_numberOfRolls; i++ ) { 
        allGroupsWithRoll[i] = ( struct sortedSize_t ){ 0, i };
        tempLengthSum += ascendingLengthsArray[i];
        if ( tempLengthSum <= maxOrderLength ) {
            maxRollsInOrder++;
        }
        groupsWithRoll[i] = createIntArray( 10000, 0, 1.1 );
        biasedGroupsWithRoll[i] = malloc( sizeof( struct int_array* ) * g_numberOfRolls );
        for ( int j = 0; j < g_numberOfRolls; j++ ) {
            biasedGroupsWithRoll[i][j] = createIntArray( 10000, 0, 1.1 );
         }
    }

    for ( int i = 0; i < g_numberOfRolls; i++ ) {
        for ( int j = 0; j < g_numberOfRolls; j++ ) {
            if ( j == i ) {
                continue;
            }
            int index = i * g_numberOfRolls + j;
            doubleBiasedGroupsWithRoll[index] = malloc( sizeof( struct int_array* ) * g_numberOfRolls );
            for ( int k = 0; k < g_numberOfRolls; k++ ) {
                doubleBiasedGroupsWithRoll[index][k] = createIntArray( 10000, 0, 1.1 );
            }
        }
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
    printf( "Generating list of groups..." );
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

            for ( int i = 0; i < g_numberOfRolls; i++ ) {
                for ( int j = 0; j < g_numberOfRolls; j++ ) {
                    if ( j == i ) {
                        continue;
                    }
                    int index = i * g_numberOfRolls + j;
                    int bmsi1 = getSmallestIntArrayIndexWithBias( group, i, doubleBiasedGroupsWithRoll[index] );
                    if ( bmsi1 == i ) {
                        doubleBiasedGroupsWithRoll[index][bmsi1] = addToIntArray( doubleBiasedGroupsWithRoll[index][bmsi1], group );
                        continue;
                    }
                    int bmsi2 = getSmallestIntArrayIndexWithBias( group, j, doubleBiasedGroupsWithRoll[index] );

                    doubleBiasedGroupsWithRoll[index][bmsi2] = addToIntArray( doubleBiasedGroupsWithRoll[index][bmsi2], group );
                }
                int biasedMinSizeIndex = getSmallestIntArrayIndexWithBias( group, i, biasedGroupsWithRoll[i] );
                biasedGroupsWithRoll[i][biasedMinSizeIndex] = addToIntArray( biasedGroupsWithRoll[i][biasedMinSizeIndex], group );
                if ( group >> i & 1 ) {
                    allGroupsWithRoll[i].size++;
                }
            }
            int minSizeIndex = getSmallestIntArrayIndex( group, groupsWithRoll );

            groupsWithRoll[minSizeIndex] = addToIntArray( groupsWithRoll[minSizeIndex], group );
            groupArray                   = addToIntArray( groupArray, group );
            group                        = nextSetOfNBits( group );
        } while ( group <= largestNumber );
    }

    qsort( allGroupsWithRoll, g_numberOfRolls, sizeof( struct sortedSize_t ), sortCompare );

    for ( int i = 0; i < g_numberOfRolls; i++ ) {
        allGroupsWithRollIndexes[i] = allGroupsWithRoll[i].rollNumber;
        groupsWithRollBySize[i] = createIntArray( groupArray->size / g_numberOfRolls, 0, 1.1 );
        //printf( "%i: %i\n", allGroupsWithRoll[i].rollNumber, allGroupsWithRoll[i].size );
    }

    for ( int i = 0; i < groupArray->size; i++ ) {
        unsigned int group = groupArray->content[i];
        for ( int j = 0; j < g_numberOfRolls; j++ ) {
            int index = allGroupsWithRollIndexes[j];
            if ( group >> index & 1 ) {
                groupsWithRollBySize[index] = addToIntArray( groupsWithRollBySize[index], group );
                break;
            }   
        }
    }

    for ( int i = 0; i < g_numberOfRolls; i++ ) {
        shrinkIntArray( groupsWithRollBySize[i] );
    }

    for ( int i = 0; i < g_numberOfRolls; i++ ) {
        //printf( "Biased towards group %i\n", i );
        groupsWithRoll[i] = shrinkIntArray( groupsWithRoll[i] );
        for ( int j = 0; j < g_numberOfRolls; j++ ) {
        biasedGroupsWithRoll[i][j] = shrinkIntArray( biasedGroupsWithRoll[i][j] );
            if ( i == j ) {
                continue;
            }
            int index = i * g_numberOfRolls + j;
            for ( int k = 0; k < g_numberOfRolls; k++ ) {
                doubleBiasedGroupsWithRoll[index][k] = shrinkIntArray( doubleBiasedGroupsWithRoll[index][k] );
            }
            //printf( "Group %i: length of %i, size of %i\n", j, biasedGroupsWithRoll[i][j]->length, biasedGroupsWithRoll[i][j]->size );
        }
    }

    start = clock();

//    struct int_array **biasedWithMostRolls = biasedGroupsWithRoll[largestTotalSizeIndex];

    struct check_t *normalPairsCheck = checkNormalPairs( groupArray );
    struct check_t *unbiasedPairsCheck = checkUnbiasedIntArray( groupArray, groupsWithRoll, g_numberOfRolls );
    struct check_t *biasedBySizeCheck  = checkUnbiasedIntArray( groupArray, groupsWithRollBySize, g_numberOfRolls );
//    struct check_t *biasedCheck        = checkBiasedIntArray( groupArray, biasedGroupsWithRoll, g_numberOfRolls );
//    struct check_t *biasedCheck        = checkUnbiasedIntArray( groupArray, biasedWithMostRolls, g_numberOfRolls );
    struct check_t *doubleBiasedCheck  = checkDoubleBiasedIntArray( groupArray, doubleBiasedGroupsWithRoll, g_numberOfRolls ); 


    printCheck( "Normal Pairs", normalPairsCheck );
    printCheck( "Unbiased Pairs", unbiasedPairsCheck );
    printCheck( "Biased by Size", biasedBySizeCheck );
//    printCheck( "Biased Pairs", biasedCheck );
    printCheck( "Double Biased Pairs", doubleBiasedCheck );
//    for ( int i = 0; i < g_numberOfRolls; i++ ) {
//        printf( "Total number of groups with roll %i: %i\n", i, allGroupsWithRoll[i] );
//    }


    diff = clock() - start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    printf( "Took %i seconds, %i millis\n", msec/1000, msec%1000 );
    printf( "Done!\nFound %'d groups\nGenerating potential orders...", groupArray->length );
    fflush( stdout );

    for ( int orderSize = minRollsInOrder; orderSize <= maxRollsInOrder; orderSize++ ) {
        int order         = ( 1 << orderSize ) - 1;
        int largestNumber = order << ( g_numberOfRolls - orderSize );

        do {
            float orderLength = rollsLength( order );

            if ( orderLength < minOrderLength || orderLength > maxOrderLength ) {
                order = nextSetOfNBits( order );
                continue;
            }
            numPotentialOrders++; 
            order = nextSetOfNBits( order );
        } while ( order <= largestNumber );
    }


//    diff = clock() - start;
//    int msec = diff * 1000 / CLOCKS_PER_SEC;
//
//    printf( "%d second %d milliseconds\n", msec/1000, msec%1000 );
    printf( "Done!\nFound %'d potential orders\n", numPotentialOrders );

    int smallArraySize            = ( 1 << g_numberOfRolls ) - 1;
    int numFound                  = 0;
    struct smallarray *smallArray = createSmallArray( smallArraySize );
    unsigned int *groups          = malloc( sizeof( unsigned int ) * g_numberOfRolls );


    recursiveSolve( 0, groups, 0, groupsWithRoll, minGroupsInOrder, minOrderLength, maxOrderLength, smallArray, &numFound, numPotentialOrders );

    fclose( g_outputFile );
    freeIntArray( groupArray );
    for ( int i = 0; i < g_numberOfRolls; i++ ) {
        freeIntArray( groupsWithRoll[i] );
        for ( int j = 0; j < g_numberOfRolls; j++ ) {
            freeIntArray( biasedGroupsWithRoll[i][j] );
            for ( int k = 0; k < g_numberOfRolls; k++ ) {
                if ( j == k ) {
                    continue;
                }
                freeIntArray( doubleBiasedGroupsWithRoll[j * g_numberOfRolls + k][i] );
            }
        }
        free( biasedGroupsWithRoll[i] );
    }
    free( groupsWithRoll );

    return 0;
}
