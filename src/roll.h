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

/**
 * Set the minimum/maximum properties of orderStats, in place
 *
 * This assumes that the given structure is already populated with the required
 * properties to calculate these new ones: 
 *  - rollList
 *  - numberOfRolls
 *  - minGroupLength
 *  - minOrderLength
 *  - maxOrderLength
 * None of these properties are checked beforehand. Calling this will also sort
 * rollList in descending order, by length.
 *
 * @param orderStats pointer to the struct that holds info about this order
 */
void setMinMaxRollStats( struct OrderStats *orderStats );

/**
 * Calculates and sets the numGroups property of each roll within
 * orderStats->rollList
 *
 * This assumes that the given structure is already populated with the required
 * properties to calculate this roll property:
 *   - minRollsPerGroup
 *   - maxRollsPerGroup
 *   - numberOfRolls
 *   - rollList
 *   - minGroupLength
 *   - maxGroupLength
 * None of these properties are checked beforehand. The number of groups that a
 * roll belongs to is accomplished by iterating through all combinations of
 * [minRollsPerGroup - maxRollsPerGroup] rolls, finding the ones that fit 
 * between minGroupLength and maxGroupLength, and incrementing each roll within
 * that group's numGroups property.
 *
 * @param orderStats pointer to the struct that holds info about this order
 */
void setNumGroupsPerRoll( struct OrderStats *orderStats );

/**
 * Sort the Rolls with rollList by how many groups they appear in, descending
 *
 * This assumes that setMinMaxRollStats has already been called.
 *
 * @param orderStats pointer to the struct that holds info about this order
 */
void sortRollsByNumGroups( struct OrderStats *orderStats );

/**
 * TODO: roll this into setNumGroupsPerRoll, it is repeating a bunch of code,
 * including the entire calculate-all-groups loop
 */
struct IntArray** getGroupsWithRollBySize( struct OrderStats *orderStats );

/**
 * Calculate and return an array of how many orders each roll can appear in
 *
 * The index of the returned array is the index of the roll in
 * orderStats->rollList. The reason for 'can' in the description is that
 * this is just calculating every possible order based on all of the
 * combinations with [minRollsPerOrder - maxRollsPerOrder] rolls in them, that
 * also fit between minOrderLength and maxOrderLength. It does not checking to
 * see if the order is guaranteed. This array is very useful for early exiting
 * out of the solver loop, where if the current roll already is in all of the
 * possible orders it can be, the loop can continue to the next roll.
 *
 * @param orderStats pointer to the struct that holds info about this order
 *
 * @return array that contains how many orders each roll can appear in
 */
int* getOrdersWithRoll( struct OrderStats *orderStats );

#endif
