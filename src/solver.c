#include <stdio.h>
#include <stdlib.h>
#include "intArray.h"
#include "smallArray.h"
#include "roll.h"
#include "solver.h"


static void recursiveSolve( const unsigned int currentGroup, const int currentArrayIndex, const int numGroupsInOrder, struct IntArray **groupsWithRoll, struct OrderStats *orderStats, struct SmallArray *alreadyFound, int *numFound, int *ordersWithRoll, int *ordersWithRollBitMask ) {
    //Id, Length, Number of Groups, Number of Rolls, Order Groups,Remaining Rolls, Average Remaining Roll Length
    if ( currentGroup & *ordersWithRollBitMask || getSmallArrayValue( alreadyFound, currentGroup ) ) {
        return;
    }

    setSmallArrayValue( alreadyFound, currentGroup );

    if ( numGroupsInOrder >= orderStats->minGroupsPerOrder && __builtin_popcount( currentGroup ) >= orderStats->minRollsPerOrder ) {
        const float currentLength = rollsLength( currentGroup, orderStats->numberOfRolls, orderStats->rollList );
        if ( currentLength >= orderStats->minOrderLength && currentLength <= orderStats->maxOrderLength ) {
            *numFound += 1;
            for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
                ordersWithRoll[i] -= ( currentGroup >> i & 1 );
                if ( ordersWithRoll[i] == 0 ) {
                    *ordersWithRollBitMask |= 1 << i; //|= to guard against this happening multiple times, which it will everytime a new order is found when ordersWithRoll[i] is already 0
                }
            }
            printf( "\rOrders found: %'i/%'i (%.2f%%)", *numFound, orderStats->numberOfPotentialOrders, *numFound * 1.0 / orderStats->numberOfPotentialOrders * 100 );
            fflush( stdout );
        }
        if ( currentLength + orderStats->minGroupLength > orderStats->maxOrderLength ) {
            return;
        }
    }
    for ( int i = currentArrayIndex + 1; i < orderStats->numberOfRolls; ++i ) {
        if ( currentGroup >> i & 1 || ordersWithRoll[i] == 0 ) {
            continue;
        }
        for ( int j = 0; j < groupsWithRoll[i]->length; ++j ) {
            if ( currentGroup & groupsWithRoll[i]->content[j] ) { //could include || groupsWithRoll[i]->content[j] & *ordersWithRollBitMask, but this would only happen if that bitmask
                continue;                                            //changes during the run, since it is already checked before
            }
            recursiveSolve( currentGroup | groupsWithRoll[i]->content[j], i, numGroupsInOrder + 1, groupsWithRoll, orderStats, alreadyFound, numFound, ordersWithRoll, ordersWithRollBitMask );
        }
    }
}

void orderSolve( struct IntArray **groupsWithRoll, struct OrderStats *orderStats, int *ordersWithRoll ) {
    int numFound = 0;
    int ordersWithRollBitMask = 0;
    int alreadyFoundSize = 1 << orderStats->numberOfRolls;
    struct SmallArray *alreadyFound = createSmallArray( alreadyFoundSize );

    for ( int i = 0; i < orderStats->numberOfRolls; ++i ) {
        printf(" Starting solving for index %i\n", i );
        for ( int j = 0; j < groupsWithRoll[i]->length; ++j ) {
            unsigned int group = groupsWithRoll[i]->content[j];
            recursiveSolve( group, i, 1, groupsWithRoll, orderStats, alreadyFound, &numFound, ordersWithRoll, &ordersWithRollBitMask );
        }
    }
}

//currentGroup, currentArrayIndex, numGroupsInOrder

struct StackParameter{
    unsigned int currentGroup;
    int currentArrayIndex;
    int numGroupsInOrder;
};

struct SolveStack {
    int capacity;
    int top;
    struct StackParameter *content;
};

static struct SolveStack* createStack( int capacity ) {
    struct SolveStack *solveStack = malloc( sizeof( struct SolveStack ) );
    solveStack->capacity = capacity;
    solveStack->top = -1;
    solveStack->content = malloc( sizeof( struct StackParameter ) * capacity );
    return solveStack;
}


static struct StackParameter popStack( struct SolveStack *solveStack ) {
    if ( solveStack->top == -1 ) {
        struct StackParameter temp = ( struct StackParameter ) {-1, -1 -1};
        return temp;
    }
    return solveStack->content[solveStack->top--];
}

static void pushStack( struct SolveStack *solveStack, struct StackParameter stackParameter ) {
    if ( solveStack->top == solveStack->capacity - 1 ) {
        return;
    }
    solveStack->content[++solveStack->top] = stackParameter;
}

static int stackIsEmpty( struct SolveStack *solveStack ) {
    return solveStack->top == -1;
}

void nonRecursiveSolve( struct IntArray **groupsWithRoll, struct OrderStats *orderStats, int *ordersWithRoll ) {

}
