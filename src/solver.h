#ifndef SOLVER_H_
#define SOLVER_H_

void recursiveSolve( unsigned int currentGroup, int currentArrayIndex, int numGroupsInOrder, struct IntArray **groupsWithRoll, struct OrderStats *orderStats, struct SmallArray *alreadyFound, int *numFound, int numPotentialOrders, unsigned long *numChecked );

#endif
