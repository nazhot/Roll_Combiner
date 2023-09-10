#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <locale.h>
#include "fileReaders.h"
#include "smallarray.h"
#include "intArray.h"
#include "testMethods.h"
#include "roll.h"
                      //guarded by strncpy
#define MAX_NUM_ROLLS 32;

FILE *g_outputFile;



struct sortedSize_t {
    int size;
    int rollNumber;
};


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

int getSmallestIntArrayIndex( unsigned int currentGroup, struct int_array **groupsWithRoll, int numberOfRolls ) {
    int smallestLength = INT32_MAX;
    int smallestLengthIndex = -1;
    for ( int i = 0; i < numberOfRolls; i++ ) {
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

int getSmallestIntArrayIndexWithBias( unsigned int currentGroup, int rollBias, struct int_array **groupsWithRoll, int numberOfRolls ) {
    if ( currentGroup >> rollBias & 1 ) {
        return rollBias;
    }
    return getSmallestIntArrayIndex( currentGroup, groupsWithRoll, numberOfRolls );
}

//void setGroupsString( unsigned int *groups, int numGroups ) {
//    fputc( '\"', g_outputFile );
//    for ( int i = 0; i < numGroups; i++ ){
//        unsigned int group = groups[i];
//        float groupLength = rollsLength( group );
//        char groupLengthString[256];
//        sprintf( groupLengthString, "%.2f", groupLength );
//
//        fputs( "-\n", g_outputFile );
//
//        for ( int j = 0; j < numberOfRolls; j++ ) {
//            if ( !(  group >> j & 1  ) ) {
//               continue; 
//            }
//            char rollLength[256];
//            sprintf( rollLength, "%.2f", rollList[j].length ); 
//            fputs( rollList[j].id, g_outputFile );
//            fputs( ": ", g_outputFile );
//            fputs( rollLength, g_outputFile );
//            fputc( '\n', g_outputFile );
//        }
//        fputs( "-\n", g_outputFile ); 
//    }
//    fputc( '\"', g_outputFile );
//}

//void setRemainingRollsString( unsigned int order ) {
//    int numRemainingRolls = 0;
//    float remainingRollsLength = 0;
//    float average;
//    fputc( '\"', g_outputFile );
//    for ( int i = 0; i < numberOfRolls; i++ ) {
//        if ( order >> i & 1 ) {
//            continue;
//        }
//        numRemainingRolls++;
//        remainingRollsLength += rollList[i].length;
//        char rollLength[256];
//        sprintf( rollLength, "%.2f", rollList[i].length ); 
//        fputs( rollList[i].id, g_outputFile );
//        fputs( ": ", g_outputFile );
//        fputs( rollLength, g_outputFile );
//        fputc( '\n', g_outputFile );
//
//    }
//    fputs( "\",", g_outputFile );
//
//    if ( numRemainingRolls == 0 ) {
//        average = 0;
//    } else {
//        average = remainingRollsLength / numRemainingRolls;
//    }
//
//    char averageLengthRemaining[256];
//    sprintf( averageLengthRemaining, "%.2f", average);
//    fputs( averageLengthRemaining, g_outputFile );
//    fputc( '\n', g_outputFile );
//}


//void recursiveSolve( unsigned int currentGroup, unsigned int *groups, int numGroupsInOrder, struct int_array **groupsWithRoll, int minGroupsInOrder, int minOrderLength, int maxOrderLength, struct smallarray *alreadyFound, int *numFound, int numPotentialOrders ) {
//    //Id, Length, Number of Groups, Number of Rolls, Order Groups,Remaining Rolls, Average Remaining Roll Length
//    if ( getSmallArrayValue( alreadyFound, currentGroup ) ) {
//        return;
//    }
//
//    setSmallArrayValue( alreadyFound, currentGroup );
//
//    if ( numGroupsInOrder >= minGroupsInOrder ) {
//        float currentLength = rollsLength( currentGroup );
//        if ( currentLength >= minOrderLength && currentLength <= maxOrderLength ) {
//            *numFound += 1;
//            printf( "\rOrders found: %'i/%'i (%.2f%%)", *numFound, numPotentialOrders, *numFound * 1.0 / numPotentialOrders * 100 );
//            fflush( stdout );
//            //printf( "Found %i\n", *numFound );
//
//            char orderNumber[256];
//            sprintf( orderNumber, "%d", currentGroup );
//            fputs( orderNumber, g_outputFile );
//            fputc( ',', g_outputFile );
//
//            char orderLength[256];
//            sprintf( orderLength, "%f", currentLength );
//            fputs( orderLength, g_outputFile );
//            fputc( ',', g_outputFile ); 
//
//            char numGroups[256];
//            sprintf( numGroups, "%d", numGroupsInOrder );
//            fputs( numGroups, g_outputFile );
//            fputc( ',', g_outputFile );
//
//            char numberOfRolls[256];
//            sprintf( numberOfRolls, "%d", __builtin_popcount( currentGroup ) );
//            fputs( numberOfRolls, g_outputFile );
//            fputc( ',', g_outputFile );
//
//            setGroupsString( groups, numGroupsInOrder );
//            //add Order Groups
//            //add Remaining Rolls
//            //add Average Remaining Roll Length
//            fputc( ',', g_outputFile );
//            setRemainingRollsString( currentGroup );
//        }
//        if ( currentLength >= maxOrderLength ) {
//            return;
//        }
//    }
//
//    int numValidGroups = 0; //how many groups don't share any rolls with currentGroup
//    int numValidRolls  = 0; //how many rolls aren't already a part of currentGroup
//                            //
//    for ( int i = 0; i < numberOfRolls; i++ ) {
//        if ( !( currentGroup >> i & 1 ) ) {
//            numValidGroups += groupsWithRoll[i]->length;
//            numValidRolls++;
//        }
//    }
//
//    if ( numValidRolls == 0 || numValidGroups == 0 ) { //prevents divide by 0 error, and is also a valid end point for the function
//        return;
//    }
//
//    int averageGroupsPerRoll             = numValidGroups / numValidRolls + 1;
//    struct int_array **newGroupsWithRoll = malloc( sizeof( struct int_array* ) * numberOfRolls ); //holds the groups that can go with currentGroup
//
//    for ( int i = 0; i < numberOfRolls; i++ ) { //initialize the int_arrays
//        newGroupsWithRoll[i] = createIntArray( averageGroupsPerRoll, 0, 1.1 );
//    }
//
//    for ( int i = 0; i < numberOfRolls; i++ ) {
//        if ( currentGroup >> i & 1 ) {
//            continue; 
//        }
//        for ( int j = 0; j < groupsWithRoll[i]->length; j++ ) {
//            if ( currentGroup & groupsWithRoll[i]->content[j] ) { //make sure group could be added
//                continue;
//            }
//            int index = getSmallestIntArrayIndex( groupsWithRoll[i]->content[j], newGroupsWithRoll );
//            newGroupsWithRoll[index] = addToIntArray( newGroupsWithRoll[index], groupsWithRoll[i]->content[j] );
//        }
//    }
//
//    for ( int i = 0; i < numberOfRolls; i++ ) {
//        if ( currentGroup >> i & 1  ) {
//            continue;
//        }
//        for ( int j = 0; j < newGroupsWithRoll[i]->length; j++ ) {
//            groups[numGroupsInOrder] = newGroupsWithRoll[i]->content[j];
//            recursiveSolve( currentGroup | newGroupsWithRoll[i]->content[j], groups, numGroupsInOrder + 1, newGroupsWithRoll, minGroupsInOrder, minOrderLength, maxOrderLength, alreadyFound, numFound, numPotentialOrders );
//        }
//    }
//
//    for ( int i = 0; i < numberOfRolls; i++ ) {
//        freeIntArray( newGroupsWithRoll[i] );
//    }
//
//    free( newGroupsWithRoll );
//}

void printCheck( char title[], struct check_t *check ) {
   printf( "--------%s-------\n", title ); 
   printf( "Time: %i sec, %i msecs\n", check->msecs / 1000, check->msecs % 1000 );
   printf( "Num checks: %'lu\n", check->numChecks );
   printf( "Num groups: %'lu\n", check->numGroups );
}

int descCompare( const void *a, const void *b ) {
    struct sortedSize_t tempA = *( struct sortedSize_t* ) a;
    struct sortedSize_t tempB = *( struct sortedSize_t* ) b;
    return tempB.size - tempA.size;
}

int ascCompare( const void *a, const void *b ) {
    struct sortedSize_t tempA = *( struct sortedSize_t* ) a;
    struct sortedSize_t tempB = *( struct sortedSize_t* ) b;
    return tempA.size - tempB.size;
}

int main( int argc, char* argv[] ) {

    if ( argc != 2 ) {
        printf( "Not the correct number of arguments (1 expected)\n" );
        return 1;
    }
    setlocale(LC_NUMERIC, "");

    g_outputFile = fopen( "outputs/output.csv", "w" );
    fputs( "Id,Length,Number of Groups,Number of Rolls,Order Groups,Remaining Rolls,Average Remaining Roll Length\n", g_outputFile );
    char *fileName = argv[1];

    struct OrderStats *orderStats = malloc( sizeof( struct OrderStats ) );
    readRollFileIntoOrderStats( fileName, orderStats );

    orderStats->minOrderLength   = 1800;
    orderStats->maxOrderLength   = 2000;
    orderStats->maxRollsPerGroup = 8;
    orderStats->minGroupLength   = 250;
    orderStats->maxGroupLength   = 350;
    orderStats->minGroupsPerOrder = ceil( orderStats->minOrderLength / orderStats->maxGroupLength );
    orderStats->maxGroupsPerOrder = floor( orderStats->maxOrderLength / orderStats->minGroupLength );

    setMinMaxRollStats( orderStats );

    int                minRollsInGroup       = 0;    //minimum number of rolls needed to form a group
    int                minRollsInOrder       = 0;    //minimum number of rolls needed to form an order
    int                maxRollsInOrder       = 0;    //maximum number of rolls needed to form an order
    int                numPotentialOrders    = 0;
    struct int_array  *groupArray            = createIntArray( 1024, 0, 2 ); //malloc( sizeof(unsigned int) * 1024 );
    struct int_array **groupsWithRoll        = malloc( sizeof( struct int_array* ) * orderStats->numberOfRolls );
    struct sortedSize_t allGroupsWithRoll[orderStats->numberOfRolls];
    struct int_array **biasedGroupsWithRoll[orderStats->numberOfRolls];
    struct int_array **doubleBiasedGroupsWithRoll[orderStats->numberOfRolls * orderStats->numberOfRolls];
    struct int_array  *groupsWithRollBySize[orderStats->numberOfRolls];


    //set up maxNumber, and initialize groupContainsRoll array
    for ( int i = 0; i < orderStats->numberOfRolls; i++ ) { 
        allGroupsWithRoll[i] = ( struct sortedSize_t ){ 0, i };
        //if ( tempLengthSum <= maxOrderLength ) {
        //    maxRollsInOrder++;
        //}
        groupsWithRoll[i] = createIntArray( 10000, 0, 1.1 );
        biasedGroupsWithRoll[i] = malloc( sizeof( struct int_array* ) * orderStats->numberOfRolls );
        for ( int j = 0; j < orderStats->numberOfRolls; j++ ) {
            biasedGroupsWithRoll[i][j] = createIntArray( 10000, 0, 1.1 );
         }
    }

    for ( int i = 0; i < orderStats->numberOfRolls; i++ ) {
        for ( int j = 0; j < orderStats->numberOfRolls; j++ ) {
            if ( j == i ) {
                continue;
            }
            int index = i * orderStats->numberOfRolls + j;
            doubleBiasedGroupsWithRoll[index] = malloc( sizeof( struct int_array* ) * orderStats->numberOfRolls );
            for ( int k = 0; k < orderStats->numberOfRolls; k++ ) {
                doubleBiasedGroupsWithRoll[index][k] = createIntArray( 10000, 0, 1.1 );
            }
        }
    }

//    tempLengthSum = 0;
//    //figure out the minimum required rolls for a group/order
//    for ( int i = numberOfRolls - 1; i >= 0; i-- ) {
//        if ( tempLengthSum >= minOrderLength ) {
//            break;
//        }
//        if ( tempLengthSum < minGroupLength ) {
//            minRollsInGroup++;
//        }
//        minRollsInOrder++;
//    }
    
    printf( "Number of rolls inputted: %i\n", orderStats->numberOfRolls );
    printf( "Minimum number of rolls needed to make group: %i\n", orderStats->minRollsPerGroup );
    printf( "Minimum number of rolls needed to make order: %i\n", orderStats->minRollsPerOrder );
    printf( "Maximum number of rolls needed to make order: %i\n", orderStats->maxRollsPerOrder );
    printf( "Minimum number of groups to make an order: %i\n", orderStats->minGroupsPerOrder );
    printf( "Maximum number of groups to make an order: %i\n", orderStats->maxGroupsPerOrder );
    printf( "Generating list of groups..." );
    clock_t start = clock(), diff; 
    //changed so that it is only gathering groups now
    //go through all possible combinations of rolls, from the minimum to maximum to create a group
    //when a valid group is found, it will add it to a general array for all groups,
    //as well as an array of arrays, with the array at index is made up of groups that contain (index) rolls
    for ( int groupSize = minRollsInGroup; groupSize <= orderStats->maxRollsPerGroup; groupSize++ ) {
        int group         = ( 1 << groupSize ) - 1; //starts at the smallest possible number for a group with groupSize bits set
        int largestNumber = group << ( orderStats->numberOfRolls - groupSize ); //largest number that could represent a group with groupSize bits set

        do {
            float groupLength = rollsLength(  group, orderStats->numberOfRolls, orderStats->rollList ); 

            if ( groupLength < orderStats->minGroupLength || groupLength > orderStats->maxGroupLength ) {
                group = nextSetOfNBits( group );
                continue;
            }

            for ( int i = 0; i < orderStats->numberOfRolls; i++ ) {
                for ( int j = 0; j < orderStats->numberOfRolls; j++ ) {
                    if ( j == i ) {
                        continue;
                    }
                    int index = i * orderStats->numberOfRolls + j;
                    int bmsi1 = getSmallestIntArrayIndexWithBias( group, i, doubleBiasedGroupsWithRoll[index], orderStats->numberOfRolls );
                    if ( bmsi1 == i ) {
                        doubleBiasedGroupsWithRoll[index][bmsi1] = addToIntArray( doubleBiasedGroupsWithRoll[index][bmsi1], group );
                        continue;
                    }
                    int bmsi2 = getSmallestIntArrayIndexWithBias( group, j, doubleBiasedGroupsWithRoll[index], orderStats->numberOfRolls );

                    doubleBiasedGroupsWithRoll[index][bmsi2] = addToIntArray( doubleBiasedGroupsWithRoll[index][bmsi2], group );
                }
                int biasedMinSizeIndex = getSmallestIntArrayIndexWithBias( group, i, biasedGroupsWithRoll[i], orderStats->numberOfRolls );
                biasedGroupsWithRoll[i][biasedMinSizeIndex] = addToIntArray( biasedGroupsWithRoll[i][biasedMinSizeIndex], group );
                if ( group >> i & 1 ) {
                    allGroupsWithRoll[i].size++;
                }
            }
            int minSizeIndex = getSmallestIntArrayIndex( group, groupsWithRoll, orderStats->numberOfRolls );

            groupsWithRoll[minSizeIndex] = addToIntArray( groupsWithRoll[minSizeIndex], group );
            groupArray                   = addToIntArray( groupArray, group );
            group                        = nextSetOfNBits( group );
        } while ( group <= largestNumber );
    }

    //qsort( allGroupsWithRoll, g_numberOfRolls, sizeof( struct sortedSize_t ), sortCompare );

    for ( int i = 0; i < orderStats->numberOfRolls; i++ ) {
        groupsWithRollBySize[i] = createIntArray( groupArray->size / orderStats->numberOfRolls, 0, 1.1 );
        printf( "%i: %i\n", allGroupsWithRoll[i].rollNumber, allGroupsWithRoll[i].size );
    }

    for ( int i = 0; i < groupArray->size; i++ ) {
        unsigned int group = groupArray->content[i];
        for ( int j = 0; j < orderStats->numberOfRolls; j++ ) {
            int index = allGroupsWithRoll[j].rollNumber;
            if ( group >> index & 1 ) {
                groupsWithRollBySize[index] = addToIntArray( groupsWithRollBySize[index], group );
                break;
            }   
        }
    }

    for ( int i = 0; i < orderStats->numberOfRolls; i++ ) {
        shrinkIntArray( groupsWithRollBySize[i] );
        printf( "%i: %i\n", i, groupsWithRollBySize[i]->length );
    }

    for ( int i = 0; i < orderStats->numberOfRolls; i++ ) {
        //printf( "Biased towards group %i\n", i );
        groupsWithRoll[i] = shrinkIntArray( groupsWithRoll[i] );
        for ( int j = 0; j < orderStats->numberOfRolls; j++ ) {
        biasedGroupsWithRoll[i][j] = shrinkIntArray( biasedGroupsWithRoll[i][j] );
            if ( i == j ) {
                continue;
            }
            int index = i * orderStats->numberOfRolls + j;
            for ( int k = 0; k < orderStats->numberOfRolls; k++ ) {
                doubleBiasedGroupsWithRoll[index][k] = shrinkIntArray( doubleBiasedGroupsWithRoll[index][k] );
            }
            //printf( "Group %i: length of %i, size of %i\n", j, biasedGroupsWithRoll[i][j]->length, biasedGroupsWithRoll[i][j]->size );
        }
    }

    start = clock();

//    struct int_array **biasedWithMostRolls = biasedGroupsWithRoll[largestTotalSizeIndex];

    struct check_t *normalPairsCheck = checkNormalPairs( groupArray );
    struct check_t *unbiasedPairsCheck = checkUnbiasedIntArray( groupArray, groupsWithRoll, orderStats->numberOfRolls );
    struct check_t *biasedBySizeCheck  = checkUnbiasedIntArray( groupArray, groupsWithRollBySize, orderStats->numberOfRolls );
//    struct check_t *biasedCheck        = checkBiasedIntArray( groupArray, biasedGroupsWithRoll, g_numberOfRolls );
//    struct check_t *biasedCheck        = checkUnbiasedIntArray( groupArray, biasedWithMostRolls, g_numberOfRolls );
    struct check_t *doubleBiasedCheck  = checkDoubleBiasedIntArray( groupArray, doubleBiasedGroupsWithRoll, orderStats->numberOfRolls ); 


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
        int largestNumber = order << ( orderStats->numberOfRolls - orderSize );

        do {
            float orderLength = rollsLength( order, orderStats->numberOfRolls, orderStats->rollList );

            if ( orderLength < orderStats->minOrderLength || orderLength > orderStats->maxOrderLength ) {
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

    //int smallArraySize            = ( 1 << orderStats->numberOfRolls ) - 1;
    //int numFound                  = 0;
    //struct smallarray *smallArray = createSmallArray( smallArraySize );
    //unsigned int *groups          = malloc( sizeof( unsigned int ) * orderStats->numberOfRolls );


    //recursiveSolve( 0, groups, 0, groupsWithRoll, minGroupsInOrder, minOrderLength, maxOrderLength, smallArray, &numFound, numPotentialOrders );

    fclose( g_outputFile );
    freeIntArray( groupArray );
    for ( int i = 0; i < orderStats->numberOfRolls; i++ ) {
        freeIntArray( groupsWithRoll[i] );
        for ( int j = 0; j < orderStats->numberOfRolls; j++ ) {
            freeIntArray( biasedGroupsWithRoll[i][j] );
            for ( int k = 0; k < orderStats->numberOfRolls; k++ ) {
                if ( j == k ) {
                    continue;
                }
                freeIntArray( doubleBiasedGroupsWithRoll[j * orderStats->numberOfRolls + k][i] );
            }
        }
        free( biasedGroupsWithRoll[i] );
    }
    free( groupsWithRoll );

    return 0;
}
