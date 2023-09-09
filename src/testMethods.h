#ifndef TEST_METHODS_H_
#define TEST_METHODS_H_
#include "intArray.h"

struct check_t {
    int msecs;
    long numChecks;
    long numGroups;
};


struct check_t* checkNormalPairs( struct int_array* );
struct check_t* checkUnbiasedIntArray( struct int_array** );
struct check_t* checkBiasedIntArray( struct int_array** );

#endif
