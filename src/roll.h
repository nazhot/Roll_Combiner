#ifndef ROLL_H_
#define ROLL_H_
#define MAX_ID_LEN 15
#include "intArray.h"
#include "smallArray.h"

struct Roll {
    char id[MAX_ID_LEN];
    float length;
    int numGroups;
};


struct OrderStats {
    int minRollsPerGroup;
    int maxRollsPerGroup;

    int minRollsPerOrder;
    int maxRollsPerOrder;
    int minGroupsPerOrder;
    int maxGroupsPerOrder;

    int minGroupLength;
    int maxGroupLength;

    float minOrderLength;
    float maxOrderLength;

    struct Roll *rollList;
    float rollLengths[4][256];
    int numberOfBytesForRolls;
    int numberOfRolls;
    int numberOfGroups;
    int numberOfPotentialOrders;
};

void sortRollsAscending( struct Roll* rollList, int numberOfRolls );
void sortRollsDescending( struct Roll* rollList, int numberOfRolls ); 
void printRollsFromInt(  unsigned int, int, struct Roll* );
float rollsLength(  unsigned int, int, struct Roll* );
float test_rollsLength( const unsigned int num, float rollLengths[4][256]) ;
void setMinMaxRollStats( struct OrderStats *orderStats );
void setNumGroupsPerRoll( struct OrderStats *orderStats );
void sortRollsByNumGroups( struct OrderStats *orderStats );
void setRollLengthsArray( struct OrderStats *orderStats, struct IntArray **groupsWithRoll );
struct IntArray** getGroupsWithRollBySize( struct OrderStats *orderStats );
int* getOrdersWithRoll( struct OrderStats *orderStats );

#endif
