#ifndef SOLVER_H_
#define SOLVER_H_

#include "group.h"
void recursiveSolve( unsigned int currentGroup, int currentArrayIndex, int numGroupsInOrder, struct GroupArray **groupsWithRoll, struct OrderStats *orderStats, struct SmallArray *alreadyFound, int *numFound, int numPotentialOrders, int *ordersWithRoll, int *ordersWithRollBitMask );
//void rollSolve( struct OrderStats *orderStats, struct IntArray **groupsWithRoll, int *ordersWithRoll );

#endif
