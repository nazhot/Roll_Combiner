#ifndef ROLL_H_
#define ROLL_H_
#define MAX_ID_LEN 15
#include "intArray.h"

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
};

void sortRollsAscending( struct Roll* rollList, int numberOfRolls );
void sortRollsDescending( struct Roll* rollList, int numberOfRolls ); 
void printRollsFromInt(  unsigned int, int, struct Roll* );
float rollsLength(  unsigned int, int, struct Roll* );
void setMinMaxRollStats( struct OrderStats *orderStats );
void setGroupArray( struct OrderStats *orderStats, struct int_array *groupArray );
void setNumGroupsPerRoll( struct OrderStats *orderStats );
void sortRollsByNumGroups( struct OrderStats *orderStats );

#endif
