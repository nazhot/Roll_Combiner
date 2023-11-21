#ifndef SOLVER_H_
#define SOLVER_H_
#include <stdio.h>

/**
 * Gather and output all of the possible orders for the given rolls
 *
 * This is the heart of the program. In the simplest of terms, it is going
 * through all of the combinations of groups [minGroupsPerOrder - 
 * maxGroupsPerOrder], seeing which fall in [minOrderLength - maxOrderLength],
 * and outputting those in the specified file. In reality, it is doing a lot
 * to reduce how many combinations it actually has to check.
 *
 * Reduction methods:
 *   - Groups were put into the groupsWithRoll 2 dimensional array, where the 
 *     array at index N is all groups that contain roll N. This allows the
 *     solver to skip all of the groups at index N when checking a group from
 *     index N.
 *   - Before being put into those array, it was determined how many groups each
 *     roll was a part of, and the rollList sorted in descending order of how
 *     many groups a roll belongs to. This means index 0 of the groupsWithRoll
 *     array will have the largest possible number of groups, index 1 the 
 *     second most, etc. Having each group be the largest possible allows the
 *     most possible groups to be skipped at every index.
 *   - The number of possible orders each roll could be a part of was
 *     calculated, and this value is now used to potentially move past a roll
 *     much earlier. Every time an order is found, every roll within it
 *     decrements the matching index of ordersWithRoll, and when that counter
 *     hits 0, that bit within a bit mask is set to 1. That bitmask is used at
 *     the beginning of checking a new group to see if the rest of the code
 *     needs to be run, which allows for it to exit much earlier if the current
 *     rollNumber has found all of its orders
 *
 * @param groupsWithRoll 2d array, first index is the same index in rollList, 
 *                       each array is groups that contain the roll at index
 * @param orderStats     stats pertaining to the order
 * @param ordersWithRoll first index is the same index in rollList, value is how
 *                       many potential orders contain that roll, decremented
 *                       in place by the function
 * @param outputFile     file to output results to
 */
void nonRecursiveSolve( struct IntArray **groupsWithRoll, struct OrderStats *orderStats, int *ordersWithRoll, FILE *outputFile );

#endif
