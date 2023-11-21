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
    int numberOfGroups;
    int numberOfPotentialOrders;
};

/**
 * Sort the given rollList by each roll's length, ascending
 *
 * This has no parameter checking of any kind, it strictly calls qsort
 * with the given parameters. 
 *
 * @param rollList      array of Roll structs to sort
 * @param numberOfRolls how many rolls are in the array
 */
void sortRollsByLengthAscending( struct Roll *rollList, int numberOfRolls );

/**
 * Sort the given rollList by each roll's length, descending
 *
 * This has no parameter checking of any kind, it strictly calls qsort
 * with the given parameters. 
 *
 * @param rollList      array of Roll structs to sort
 * @param numberOfRolls how many rolls are in the array
 */
void sortRollsByLengthDescending( struct Roll *rollList, int numberOfRolls ); 

/**
 * Print a representation of the given int using the input rolls
 * 
 * For space and speed, combinations of rolls are stored as a single unsigned
 * int. This is great for both, horrible for debugging/readability. This
 * function turns that standard storage to something human readable.
 *
 * @param rolls         unsigned int representation of a combination of rolls
 * @param numberOfRolls how many bits of 'rolls' to check
 * @param rollList      user provided Rolls
 */
void printRollsFromInt( unsigned int rolls, int numberOfRolls, struct Roll *rollList );

/**
 * Get the length of all of the rolls in a group
 *
 * @param rolls         unsigned int representation of a combination of rolls
 * @param numberOfRolls how many bits of 'rolls' to check
 * @param rollList      user provided Rolls
 * 
 * @return the length of the given rolls
 */
float rollsLength(  unsigned int rolls, int numberOfRolls, struct Roll *rollList );
void setMinMaxRollStats( struct OrderStats *orderStats );
void setNumGroupsPerRoll( struct OrderStats *orderStats );
void sortRollsByNumGroups( struct OrderStats *orderStats );
void setRollLengthsArray( struct OrderStats *orderStats, struct IntArray **groupsWithRoll );
struct IntArray** getGroupsWithRollBySize( struct OrderStats *orderStats );
int* getOrdersWithRoll( struct OrderStats *orderStats );

#endif
