#ifndef ROLL_H_
#define ROLL_H_
#define MAX_ID_LEN 15
#include "intArray.h"
#include "smallArray.h"
#include "group.h"

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
    int numberOfPotentialOrders;
};

void sortRollsAscending( struct Roll* rollList, int numberOfRolls );
void sortRollsDescending( struct Roll* rollList, int numberOfRolls ); 
void printRollsFromInt(  unsigned int, int, struct Roll* );
float rollsLength(  unsigned int, int, struct Roll* );
void setMinMaxRollStats( struct OrderStats *orderStats );
struct GroupArray* setGroupArray( struct OrderStats *orderStats );
void setNumGroupsPerRoll( struct OrderStats *orderStats );
void sortRollsByNumGroups( struct OrderStats *orderStats );
struct GroupArray** setGroupsWithRollBySize( struct GroupArray *groupArray, int numberOfRolls );
struct GroupArray** setGroupsWithoutRollBySize( struct GroupArray *groupArray, int numberOfRolls );
int getPotentialOrders( struct OrderStats *orderStats, int *ordersWithRoll );

#endif
