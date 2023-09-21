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
    int numberOfRolls;
    int numberOfGroups;
    int numberOfPotentialOrders;
};

void sortRollsAscending( struct Roll* rollList, int numberOfRolls );
void sortRollsDescending( struct Roll* rollList, int numberOfRolls ); 
void printRollsFromInt(  unsigned int, int, struct Roll* );
float rollsLength(  unsigned int, int, struct Roll* );
void setMinMaxRollStats( struct OrderStats *orderStats );
struct IntArray* setGroupArray( struct OrderStats *orderStats, struct IntArray *groupArray );
void setNumGroupsPerRoll( struct OrderStats *orderStats );
void sortRollsByNumGroups( struct OrderStats *orderStats );
struct IntArray** getGroupsWithRollBySize( struct OrderStats *orderStats );
struct IntArray** setGroupsWithoutRollBySize( struct IntArray **groupsWithoutRollBySize, struct IntArray *groupArray, int numberOfRolls );
int* getOrdersWithRoll( struct OrderStats *orderStats );

#endif
