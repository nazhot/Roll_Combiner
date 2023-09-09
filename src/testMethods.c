#include "testMethods.h"
#include <stdio.h>
#include <stdlib.h>
#include "intArray.h"
#include <time.h>

struct check_t* checkNormalPairs(struct int_array *groupArray ) {
    struct check_t *checks = malloc( sizeof( struct check_t ) );
    clock_t start = clock(), diff;
    long numChecks = 0;
    long numGroups = 0;
    for ( int i = 0; i < groupArray->length; i++ ) {
        unsigned int group1 = groupArray->content[i];
        for ( int j = i + 1; j < groupArray->length; j++ ) {
            numChecks++;
            if ( group1 & groupArray->content[j] ) {
                continue;
            }
            numGroups++;
        }
    }
    diff = clock() - start;
    checks->msecs = diff * 1000 / CLOCKS_PER_SEC;
    checks->numChecks = numChecks;
    checks->numGroups = numGroups;

    return checks;
}


struct check_t* checkUnbiasedIntArray( struct int_array *groupArray, struct int_array **groupsWithRoll, int numberOfRolls ) {
    struct check_t *checks = malloc( sizeof( struct check_t ) );
    clock_t start = clock(), diff;
    long numChecks = 0;
    long numGroups = 0;

    for ( int i = 0; i < numberOfRolls; i++ ) {
        for ( int j = 0; j < groupsWithRoll[i]->length; j++ ) {
            unsigned int group = groupsWithRoll[i]->content[j];
            for ( int k = i + 1; k < numberOfRolls; k++ ) {
                if ( group >> k & 1 ) {
                    continue;
                }
                for ( int l = 0; l < groupsWithRoll[k]->length; l++ ) {
                    numChecks++;
                    if ( group & groupsWithRoll[k]->content[l] ) {
                        continue;
                    }
                    numGroups++;
                }
            }
        }
    }



//    for ( int i = 0; i < groupArray->length; i++ ) {
//        unsigned int group = groupArray->content[i];
//        for ( int j = 0; j < numberOfRolls; j++ ) {
//            if ( group >> j & 1 ){
//                continue;
//            }
//            for ( int k = 0; k < groupsWithRoll[j]->length; k++ ) {
//                numChecks++;
//                if ( group & groupsWithRoll[j]->content[k] ) {
//                    continue;
//                }
//                numGroups++;
//            }
//        }
//    }

    diff = clock() - start;
    checks->msecs = diff * 1000 / CLOCKS_PER_SEC;
    checks->numChecks = numChecks;
    checks->numGroups = numGroups;

    return checks;
}

struct check_t* checkBiasedIntArray( struct int_array *groupArray, struct int_array ***biasedGroupsWithRoll, int numberOfRolls ) {
    struct check_t *checks = malloc( sizeof( struct check_t ) );
    clock_t start = clock(), diff;
    long numChecks = 0;
    long numGroups = 0;
    for ( int i = 0; i < groupArray->length; i++ ) {
        unsigned int group = groupArray->content[i];
        int largestSize = 0;
        int largestSizeIndex = -1;
        for ( int j = 0; j < numberOfRolls; j++ ) {
            if ( !( group >> j & 1 ) ) {
                continue;
            }
            if ( biasedGroupsWithRoll[j][j]->length > largestSize ) {
                largestSize = biasedGroupsWithRoll[j][j]->length;
                largestSizeIndex = j;
            }
        }
        struct int_array **temp_groupsWithRoll = biasedGroupsWithRoll[largestSizeIndex];
        for ( int j = i + 1; j < numberOfRolls; j++ ) {
            if ( group >> j & 1 ) {
                continue;
            }
            for ( int k = 0; k < temp_groupsWithRoll[j]->length; k++ ) {
                numChecks++;
                if ( group & temp_groupsWithRoll[j]->content[k] ) {
                    continue;
                }
                numGroups++;
            }
        }
    }
//    for ( int i = 0; i < groupArray->length; i++ ) {
//        unsigned int group = groupArray->content[i];
//        int largestSize = 0;
//        int largestSizeIndex = -1;
//        for ( int j = 0; j < numberOfRolls; j++ ) {
//            if ( !( group >> j & 1 ) ) {
//                continue;
//            }
//            if ( biasedGroupsWithRoll[j][j]->length > largestSize ) {
//                largestSize = biasedGroupsWithRoll[j][j]->length;
//                largestSizeIndex = j;
//            }
//        }
//        struct int_array **temp_groupsWithRoll = biasedGroupsWithRoll[largestSizeIndex];
//        for ( int j = 0; j < numberOfRolls; j++ ) {
//            if ( group >> j & 1 ) {
//                continue;
//            }
//            for ( int k = 0; k < temp_groupsWithRoll[j]->length; k++ ) {
//                numChecks++;
//                if ( group & temp_groupsWithRoll[j]->content[k] ) {
//                    continue;
//                }
//                numGroups++;
//            }
//        }
//    }
    diff = clock() - start;
    checks->msecs = diff * 1000 / CLOCKS_PER_SEC;
    checks->numChecks = numChecks;
    checks->numGroups = numGroups;

    return checks;
//
}

struct check_t* checkDoubleBiasedIntArray( struct int_array *groupArray, struct int_array ***biasedGroupsWithRoll, int numberOfRolls ) {
    struct check_t *checks = malloc( sizeof( struct check_t ) );
    clock_t start = clock(), diff;
    long numChecks = 0;
    long numGroups = 0;

    for ( int i = 0; i < groupArray->length; i++ ) {
        unsigned int group = groupArray->content[i];
        int largestSize = -1;
        int largestSizeIndex = -1;
        for ( int j = 0; j < numberOfRolls; j++ ) {
            for ( int k = 0; k < numberOfRolls; k++ ) {
                if ( j == k ) {
                    continue;
                }
                int index = j * numberOfRolls + k;
                if ( !( group >> j & 1 ) | !( group >> k & 1 ) ) {
                    continue;
                }
                if ( ( biasedGroupsWithRoll[index][j]->length + biasedGroupsWithRoll[index][k]->length ) > largestSize ) {
                    largestSize = biasedGroupsWithRoll[index][j]->length + biasedGroupsWithRoll[index][k]->length;
                    largestSizeIndex = index;
                }
            }
        }
        struct int_array **temp_groupsWithRoll = biasedGroupsWithRoll[largestSizeIndex];
        for ( int j = 0; j < numberOfRolls; j++ ) {
            if ( group >> j & 1 ) {
                continue;
            }
            for ( int k = 0; k < temp_groupsWithRoll[j]->length; k++ ) {
                numChecks++;
                if ( group & temp_groupsWithRoll[j]->content[k] ) {
                    continue;
                }
                numGroups++;
            }
        }

    }
    diff = clock() - start;
    checks->msecs = diff * 1000 / CLOCKS_PER_SEC;
    checks->numChecks = numChecks;
    checks->numGroups = numGroups;

    return checks;
}
