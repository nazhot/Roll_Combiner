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
    for ( int i = 0; i < groupArray->length; i++ ) {
        unsigned int group = groupArray->content[i];
        for ( int j = 0; j < numberOfRolls; j++ ) {
            if ( group >> j & 1 ){
                continue;
            }
            for ( int k = 0; k < groupsWithRoll[j]->length; k++ ) {
                numChecks++;
                if ( group & groupsWithRoll[j]->content[k] ) {
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
