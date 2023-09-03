#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <locale.h>
#include "trie.h"
#include "smallarray.h"
#include "intArray.h"

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

//standard way to create an array, keeping track of its size/length
//the first element of the array is 1 + index of the final element
//the second element of the array is how large the array is
unsigned int* createArray( int initialSize ) {
    unsigned int *array    = malloc( sizeof( unsigned int ) * initialSize );
                  array[0] = 2;
                  array[1] = initialSize;

    return array;
}


//standard way to add to an array, keeping track of its size/length
//the first element of the array is 1 + index of the final element
//the second element of the array is how large the array is
unsigned int* addToArray( unsigned int array[], unsigned int intToAdd ) {
    int arrayLength = array[0];
    int arraySize   = array[1];
    
    if ( arrayLength == arraySize ) {
        arraySize *= 2;
        unsigned int *temp;
        temp      = realloc( array, sizeof( unsigned int ) * arraySize );
        if ( temp == NULL ) {
            free( array );
            printf( "Not able to realloc memory, exiting\n" );
            exit(0);
        }
        array = temp;
        array[1]  *= 2;
    }

    array[arrayLength] = intToAdd;
    array[0]++;

    return array;
}


unsigned int* shrinkArray( unsigned int array[] ) {
    int arrayLength = array[0];
    int arraySize   = array[1];

    unsigned int *temp;
    temp = realloc( array, sizeof( unsigned int ) * arrayLength );
    if ( temp == NULL ) {
        free( array );
        printf( "Not able to shrink array, exiting\n" );
        exit(0);
    }
    array = temp;
    array[1] = arrayLength;

    return array;
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


//the indexes are the different roll numbers
//the second arrays are all of the groups that contain the roll of index
//the arrays are in ascending order of their size
//
void solve( struct Roll rolls[], float minOrderLength, float maxOrderLength, unsigned int **groups, int rollNumbers[], unsigned int cur[], unsigned int **solutions, int position, int count, unsigned int seen, int skipsLeft, int numberOfRolls, int isFirst ){
    //groups: sorted array of array of groups
    //cur: partial solution, array of group numbers
    //solutions: array of array of solutions (groups)
    //position: current index of groups first array
    //count: how many groups are in partial solution
    //seen: unsigned int representation of current 
    //skipped: how many more rolls the program can skip
    

    float orderLength = rollsLength( rolls, seen, numberOfRolls );
    if ( orderLength > maxOrderLength ) {
        return;
    }
    if ( orderLength >= minOrderLength && orderLength <= maxOrderLength ) {
        //return;
        //printf( "-----------------------------------\n" );
        //printf( "[" );
        //for ( int i = 0; i < count; i++ ) {
           // printRollsFromInt( rolls, cur[i], numberOfRolls );
        //   printf( "%i", cur[i] );
        //   if ( i < count - 1 ) {
        //    printf( ", " );
        //   }
        //}
        //printf( "]\n" );
        //printf( "-----------------------------------\n" );
        //printRollsFromInt( rolls, seen, numberOfRolls );
        return;
    }

    while ( 1 ) {
        if ( position >= numberOfRolls ) {
            return;
        }

        if ( isFirst ) {
            printf( "Position: %i\n", position );
        }
        unsigned int rollNumber = 1 << rollNumbers[position];
        if ( ! ( seen & rollNumber ) ) {
            break;
        }
        position++;

    }


    for ( int i = 2; i < groups[position][0]; i++ ) {
        unsigned int group = groups[position][i];

        if ( seen & group ) {
            continue;
        }

        cur[count] = i;
        solve( rolls, minOrderLength, maxOrderLength, groups, rollNumbers, cur, solutions, position + 1, count + 1, ( seen | group ) , skipsLeft, numberOfRolls, 0 );
    }


    if ( skipsLeft ) {
        skipsLeft--;
        solve( rolls, minOrderLength, maxOrderLength, groups, rollNumbers, cur, solutions, position + 1, count, seen, skipsLeft, numberOfRolls, 0 );
    }
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

void recursiveSolve( unsigned long currentGroup, int numRolls, struct int_array **groupsWithRoll ) {
    int numValidGroups = 0;
    int numValidRolls = 0;
    for ( int i = 0; i < numRolls; i++ ) {
        if ( !( currentGroup >> i & 1 ) ) {
            numValidGroups += groupsWithRoll[i]->length;
            numValidRolls++;
        }
    }
    int averageGroupsPerRoll = numValidGroups / numValidRolls;
    printf( "_rs_ Stats for group " );
    printNumberBits( currentGroup );
    printf( "\n Number of valid groups: %i\n", numValidGroups );
    printf( "Number of valid rolls: %i\n", numValidRolls );
    printf( "Average groups per roll: %i\n", averageGroupsPerRoll );
    struct int_array **newGroupsWithRoll = malloc( sizeof( struct int_array* ) * numRolls ); 
    for ( int i = 0; i < numRolls; i++ ) {
        newGroupsWithRoll[i] = createIntArray( averageGroupsPerRoll, 0, 1.1 );
    }

    numValidGroups = 0;

    for ( int i = 0; i < numRolls; i++ ) {
        if ( currentGroup >> i & 1 ) {
            continue; 
        }
        for ( int j = 0; j < groupsWithRoll[i]->length; j++ ) {
            if ( currentGroup & groupsWithRoll[i]->content[j] ) {
                continue;
            }
            int index = getSmallestIntArrayIndex( groupsWithRoll[i]->content[j], numRolls, newGroupsWithRoll );
            if ( currentGroup >> index & 1 ) {
                printf( "Going to index %i for some reason??\n", index );
                printNumberBits( groupsWithRoll[i]->content[j] );
            }
            numValidGroups++;
            newGroupsWithRoll[index] = addToIntArray( newGroupsWithRoll[index], groupsWithRoll[i]->content[j] );
        }
    }

    for ( int i = 0; i < numRolls; i++ ) {
        printf( "Size of balanced array for roll %i: %i\n", i, newGroupsWithRoll[i]->length );
    }
    
    printf( "_rs_ Total number of valid groups for given group: %i\n", numValidGroups );

    for ( int i = 0; i < numRolls; i++ ) {
        
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
    unsigned int *groupArray           = createArray( 1024 ); //malloc( sizeof(unsigned int) * 1024 );
    struct trieNode *trieRoot          = getTrieNode();
    int numAdded                       = 0;
    printf( "%f\n", pow( 2, numberOfRolls - 1 ) + 1 );
    struct smallarray *smallarray      = createSmallArray( pow( 2, numberOfRolls - 1) + 1 );

    unsigned int groupsContainRoll[numberOfRolls]; 
    unsigned int **groupsWithXRolls = malloc( (maxSplices + 2 ) * sizeof( int* ) );
    unsigned int **groupsThatStartWithRoll = malloc( sizeof( unsigned int ) * numberOfRolls );

    for ( int i = 0; i <= maxSplices + 1; i++ ) {
        groupsWithXRolls[i] = createArray( 1024 );//malloc( sizeof( unsigned int ) * 1024 );
    }

    
    struct int_array **balancedGroupsContainRoll = malloc( sizeof( struct int_array ) * numberOfRolls );


    //set up maxNumber, and initialize groupContainsRoll array
    for ( int i = 0; i < numberOfRolls; i++ ) { 
        groupsThatStartWithRoll[i] = createArray( 1024 );
        tempLengthSum += ascendingLengthsArray[i];
        if ( tempLengthSum <= maxOrderLength ) {
            maxRollsInOrder++;
        }
        balancedGroupsContainRoll[i] = createIntArray( 10000, 0, 1.1 );
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

            if ( groupLength >= minGroupLength && groupLength <= maxGroupLength ) {
                int alreadyAdded = 0;
                int minBalancedLength = INT32_MAX;
                int minBalancedIndex = -1;
                for ( int j = 0; j <= currentMaxRoll; j++ ) { 
                    if ( groupRolls >> j & 1 ) {
                        if ( balancedGroupsContainRoll[j]->length < minBalancedLength ) {
                            minBalancedLength = balancedGroupsContainRoll[j]->length;
                            minBalancedIndex = j;
                        }
                        groupsContainRoll[j]++; 
                        if ( !alreadyAdded ) {
                            groupsThatStartWithRoll[j] = addToArray( groupsThatStartWithRoll[j], groupRolls );
                            alreadyAdded = 1;
                        }
                    }
                }
                balancedGroupsContainRoll[minBalancedIndex] = addToIntArray( balancedGroupsContainRoll[minBalancedIndex], groupRolls );
                groupsWithXRolls[groupSize] = addToArray( groupsWithXRolls[groupSize], groupRolls ); 
                groupArray = addToArray( groupArray, groupRolls ); 
                numAdded += addTrieNode( trieRoot, groupRolls );
                numberOfGroups++;
            }
        } while ( incrementArray( rollsInGroupArray, groupSize, numberOfRolls - 1 ) );
    }

    int temp_total = 0;
    for ( int i = 0; i < numberOfRolls; i++ ) {
        printf( "Size of array for roll %i: %i\n", i, balancedGroupsContainRoll[i]->length );
        temp_total += balancedGroupsContainRoll[i]->length;
    }

    recursiveSolve( groupArray[2], numberOfRolls, balancedGroupsContainRoll );
    printf( "And the total is: %i\n", temp_total );
    
    


    int total = 0;
    
    for ( int i = 0; i < numberOfRolls; i++ ) { 
        printf( "Roll %i Count: %i\n", i, groupsThatStartWithRoll[i][0] - 2 );
        total += groupsThatStartWithRoll[i][0] - 2;
    }
    printf( "Total: %'i\n", total );
    printf( "Trie Node total: %'i\n", numAdded );

    unsigned long temp_totalSize = 0;
    for ( int i = 2; i < groupArray[0]; i++ ) {
        unsigned long group1 = groupArray[i];
        unsigned long temp_smallSize = 0;
        for ( int j = 2; j < groupArray[0]; j++ ) {
            unsigned long group2 = groupArray[j];
            if ( group1 & group2 ) {
                continue;
            }
            temp_smallSize++;
            temp_totalSize++;
        }
        if ( groupArray[0] - ( i - 2 ) < 1000 ) {
            printf( "Group %i has %lu other groups that it could be added to\n", i, temp_smallSize );
        }
    }
    printf( "In total, there are %'lu groups\n", temp_totalSize );

    int sectionsNeeded = ceil( 1.0 * numberOfRolls / 8 );
    printf( "Number of sections needed: %i\n", sectionsNeeded );
    int sections[sectionsNeeded][256];
    int sectionIndexes[sectionsNeeded];
    
    for ( int i = 0; i < sectionsNeeded; i++ ) {
        sectionIndexes[i] = 1;
        for ( int j = 0; j < 256; j++ ) {
            sections[i][j] = 0;
        }
    }

    for ( int bits = 1; bits <= maxSplices + 1; bits++ ) {
        unsigned number = pow( 2, bits ) - 1;

        while ( number < 256 ) {
            int sectionAdded[sectionsNeeded];
            for ( int i = 0; i < sectionsNeeded; i++ ) {
                sectionAdded[i] = 0;
            }

            for ( int i = 2; i < groupArray[0]; i++ ) {
                int allAdded = 1;
                for ( int section = 0; section < sectionsNeeded; section++ ) {
                    if ( sectionAdded[section] ) {
                        continue;
                    }
                    if ( ( ( groupArray[i] >> ( 8 * section ) ) & number ) == number ) {
                        sectionAdded[section] = 1;
                        sections[section][sectionIndexes[section]++] = number;
                        continue;
                    }
                    allAdded = 0;
                }
                if ( allAdded ) {
                    break;
                }
            }

            number = nextSetOfNBits( number );
        }
    }

    int multiplied = 1;
    for ( int i = 0; i < sectionsNeeded; i++ ) {
        printf( "Number of %ith sections: %i\n", i, sectionIndexes[i] );
        multiplied *= sectionIndexes[i];
    }
    printf( "Total multiplied: %'i\n", multiplied );

    struct trieNode *secondTrieRoot = getTrieNode();
    int numPairs = 0;

    for ( int i = 2; i < groupArray[0]; i++ ) {
        if ( i % 10000 == 0 ) {
            printf( "%i\n", i );
        }
        findCompatibleGroups( trieRoot, groupArray[i], 0, 0, &numPairs, secondTrieRoot, smallarray );
    }

    printf( "Number of unique pairs: %'i\n", numPairs );
/*
    long numPairs = 0;
    long numAdded = 0;
    struct trieNode *root = getTrieNode();

    for ( int i = 0; i < numberOfRolls; i++ ) {
        printf(" count: %i\n", i );
        for ( int j = 2; j < groupsThatStartWithRoll[i][0]; j++ ) {
            unsigned int group1 = groupsThatStartWithRoll[i][j];
            for ( int k = i + 1; k < numberOfRolls; k++ ) {
                if ( group1 >> k & 1 ) {
                    continue;
                }
                for ( int l = 2; l < groupsThatStartWithRoll[k][0]; l++ ) {
                    unsigned int group2 = groupsThatStartWithRoll[k][l];
                    if ( group2 & group1 ) {
                        continue;
                    }
                    numAdded += addTrieNode( root, group1 ^ group2 );
                    //addNode( &binarySearchTree, group2 ^ group1 );
                    numPairs++;
                }
            }
        }

    }
    printf( "Number of pairs: %'ld\n", numPairs );
    printf( "Number of pairs added: %ld\n", numAdded );
*/
    unsigned int cur[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int **solns;
    //solve( rollList, minOrderLength,  maxOrderLength, sortedGroupsArray, sortedRollNumbersByGroupCount, cur, solns, 0, 0, 0, numberOfRolls - minRollsInOrder, numberOfRolls, 1);

    diff = clock() - start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;

    printf( "%d second %d milliseconds\n", msec/1000, msec%1000 );

    free(groupArray);

    return 0;
}
