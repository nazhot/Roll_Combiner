#ifndef TEST_METHODS_H_
#define TEST_METHODS_H_
#include "intArray.h"

struct check_t {
    int msecs;
    long numChecks;
    long numGroups;
};


struct check_t* checkNormalPairs( struct IntArray* );
struct check_t* checkUnbiasedIntArray( struct IntArray*, struct IntArray**, int );
struct check_t* checkBiasedIntArray( struct IntArray*, struct IntArray***, int );
struct check_t* checkDoubleBiasedIntArray( struct IntArray*, struct IntArray***, int );

#endif
