#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "intArray.h"
#include "smallArray.h"
#include "roll.h"
#include "solver.h"
#include <pthread.h>
#include <time.h>

struct StackParameters{
    unsigned int currentGroup;
    int currentArrayIndex;
    int numGroupsInOrder;
};

struct SolveStack {
    int capacity;
    int top;
    struct StackParameters *content;
};

static struct SolveStack* createStack( int capacity ) {
    struct SolveStack *solveStack = malloc( sizeof( struct SolveStack ) );
    solveStack->capacity = capacity;
    solveStack->top = -1;
    solveStack->content = malloc( sizeof( struct StackParameters ) * capacity );
    return solveStack;
}


static struct StackParameters popStack( struct SolveStack *solveStack ) {
    if ( solveStack->top == -1 ) {
        struct StackParameters temp = ( struct StackParameters ) {-1, -1 -1};
        return temp;
    }
    return solveStack->content[solveStack->top--];
}

static void pushStack( struct SolveStack *solveStack, struct StackParameters stackParameter ) {
    if ( solveStack->top == solveStack->capacity - 1 ) {
        puts( "All full" );
        return;
    }
    solveStack->content[++solveStack->top] = stackParameter;
}

static int stackIsEmpty( struct SolveStack *solveStack ) {
    return solveStack->top == -1;
}

struct ThreadArgs{
    int8_t startingIndex;
    int *numFound;
    int *ordersWithRollBitMask;
    struct SmallArray *alreadyFound;
    struct IntArray **groupsWithRoll;
    struct OrderStats *orderStats;
    int *ordersWithRoll;
    int *threadsCompleted;
};

pthread_mutex_t setSAMutex;

static void* threadSolve( void *args )  { 
    struct ThreadArgs *threadArgs = ( struct ThreadArgs * ) args;
    int8_t startingIndex = threadArgs->startingIndex;
    int *numFound = threadArgs->numFound;
    int *ordersWithRollBitMask = threadArgs->ordersWithRollBitMask;
    struct SmallArray *alreadyFound = threadArgs->alreadyFound;
    struct IntArray **groupsWithRoll = threadArgs->groupsWithRoll;
    struct OrderStats *orderStats = threadArgs->orderStats;
    int *ordersWithRoll = threadArgs->ordersWithRoll;

    printf( "Starting thread with index %i\n", startingIndex );

    const int8_t numberOfRolls = orderStats->numberOfRolls;
    struct SolveStack *solveStack = createStack( 50000 );

    for ( int groupNumber = 0; groupNumber < groupsWithRoll[startingIndex]->length; ++groupNumber ) {

        unsigned int startingGroup = groupsWithRoll[startingIndex]->content[groupNumber];

        pushStack( solveStack, ( struct StackParameters ) { startingGroup, startingIndex, 1 } );

        while( !stackIsEmpty( solveStack ) ) {
            struct StackParameters parameters = popStack( solveStack );
            const unsigned int currentGroup = parameters.currentGroup;
            const int currentArrayIndex = parameters.currentArrayIndex;
            const int numGroupsInOrder = parameters.numGroupsInOrder;

            if ( currentGroup & *ordersWithRollBitMask ) {
                continue;
            }
            pthread_mutex_lock( &setSAMutex );
            setSmallArrayValue( alreadyFound, currentGroup );
            pthread_mutex_unlock( &setSAMutex );

            if ( numGroupsInOrder >= orderStats->minGroupsPerOrder && __builtin_popcount( currentGroup ) >= orderStats->minRollsPerOrder ) {
                const float currentLength = rollsLength( currentGroup, orderStats->numberOfRolls, orderStats->rollList );
                if ( currentLength >= orderStats->minOrderLength && currentLength <= orderStats->maxOrderLength ) {
                    pthread_mutex_lock( &setSAMutex );
                    *numFound += 1;
                    for ( int i = 0; i < numberOfRolls; ++i ) {
                        ordersWithRoll[i] -= ( currentGroup >> i & 1 );
                        if ( ordersWithRoll[i] == 0 ) {
                        *ordersWithRollBitMask |= 1 << i; //|= to guard against this happening multiple times, which it will everytime a new order is found when ordersWithRoll[i] is already 0
                        }
                    }
                    pthread_mutex_unlock( &setSAMutex );
                    printf( "\rOrders found: %'i/%'i (%.2f%%)", *numFound, orderStats->numberOfPotentialOrders, *numFound * 1.0 / orderStats->numberOfPotentialOrders * 100 );
                    fflush( stdout );
                }
                if ( currentLength + orderStats->minGroupLength > orderStats->maxOrderLength ) {
                    continue;
                }
            }
            for ( int i = currentArrayIndex + 1; i < numberOfRolls; ++i ) {
                if ( currentGroup >> i & 1 || ordersWithRoll[i] == 0 ) {
                    continue;
                }
                for ( int j = 0; j < groupsWithRoll[i]->length; ++j ) {
                    if ( currentGroup & groupsWithRoll[i]->content[j] || groupsWithRoll[i]->content[j] & *ordersWithRollBitMask || getSmallArrayValue( alreadyFound, currentGroup | groupsWithRoll[i]->content[j] ) ) { //could include || groupsWithRoll[i]->content[j] & *ordersWithRollBitMask, but this would only happen if that bitmask
                        continue;                                            //changes during the run, since it is already checked before
                    }
                    pushStack( solveStack, ( struct StackParameters ) { currentGroup | groupsWithRoll[i]->content[j], i, numGroupsInOrder + 1} );
                }
            }
        }
    }
    pthread_exit( NULL );
}

void nonRecursiveSolve( struct IntArray **groupsWithRoll, struct OrderStats *orderStats, int *ordersWithRoll ) {

    int start = clock();

    int numFound = 0;
    int threadsCompleted = 0;
    int ordersWithRollBitMask = 0;
    int alreadyFoundSize = 1 << orderStats->numberOfRolls;
    const int8_t numberOfRolls = orderStats->numberOfRolls;
    struct SmallArray *alreadyFound = createSmallArray( alreadyFoundSize );
    struct SolveStack *solveStack = createStack( 5000000 );
    pushStack( solveStack, ( struct StackParameters ) { 0, -1, 0 } );

    pthread_t threadIds[numberOfRolls];

    if ( pthread_mutex_init( &setSAMutex, NULL ) != 0 ) { 
        puts( "Failed to initialize mutex" );
        exit(1);
    }

    int threadErrorCode;

    for ( int8_t i = 0; i < numberOfRolls; i++ ) {
        //rc = pthread_create( &threadIds[i], 
        struct ThreadArgs *threadArgs = malloc( sizeof( struct ThreadArgs ) );
        threadArgs->startingIndex = i;
        threadArgs->numFound = &numFound;
        threadArgs->ordersWithRollBitMask = &ordersWithRollBitMask;
        threadArgs->alreadyFound = alreadyFound;
        threadArgs->groupsWithRoll = groupsWithRoll;
        threadArgs->orderStats = orderStats;
        threadArgs->ordersWithRoll = ordersWithRoll;
        threadArgs->threadsCompleted = &threadsCompleted;

        threadErrorCode = pthread_create( &threadIds[i], NULL, threadSolve, ( void * ) threadArgs );
        if ( threadErrorCode ) {
            printf("\n ERROR: return code from pthread_create is %d \n", threadErrorCode );
            exit(1);
        }
        pthread_join( threadIds[i], NULL );
    }

//    while( !stackIsEmpty( solveStack ) ) {
//        struct StackParameters parameters = popStack( solveStack );
//        const unsigned int currentGroup = parameters.currentGroup;
//        const int currentArrayIndex = parameters.currentArrayIndex;
//        const int numGroupsInOrder = parameters.numGroupsInOrder;
//
//        if ( currentGroup & ordersWithRollBitMask  ) {
//            continue;
//        }
//
//        setSmallArrayValue( alreadyFound, currentGroup );
//
//        if ( numGroupsInOrder >= orderStats->minGroupsPerOrder && __builtin_popcount( currentGroup ) >= orderStats->minRollsPerOrder ) {
//            const float currentLength = rollsLength( currentGroup, orderStats->numberOfRolls, orderStats->rollList );
//            if ( currentLength >= orderStats->minOrderLength && currentLength <= orderStats->maxOrderLength ) {
//                numFound += 1;
//                for ( int i = 0; i < numberOfRolls; ++i ) {
//                    ordersWithRoll[i] -= ( currentGroup >> i & 1 );
//                    if ( ordersWithRoll[i] == 0 ) {
//                        ordersWithRollBitMask |= 1 << i; //|= to guard against this happening multiple times, which it will everytime a new order is found when ordersWithRoll[i] is already 0
//                    }
//                }
//                printf( "\rOrders found: %'i/%'i (%.2f%%)", numFound, orderStats->numberOfPotentialOrders, numFound * 1.0 / orderStats->numberOfPotentialOrders * 100 );
//                fflush( stdout );
//            }
//            if ( currentLength + orderStats->minGroupLength > orderStats->maxOrderLength ) {
//                continue;
//            }
//        }
//        for ( int i = currentArrayIndex + 1; i < numberOfRolls; ++i ) {
//            if ( currentGroup >> i & 1 || ordersWithRoll[i] == 0 ) {
//                continue;
//            }
//            for ( int j = 0; j < groupsWithRoll[i]->length; ++j ) {
//                if ( currentGroup & groupsWithRoll[i]->content[j] || groupsWithRoll[i]->content[j] & ordersWithRollBitMask || getSmallArrayValue( alreadyFound, currentGroup | groupsWithRoll[i]->content[j] ) ) { //could include || groupsWithRoll[i]->content[j] & *ordersWithRollBitMask, but this would only happen if that bitmask
//                    continue;                                            //changes during the run, since it is already checked before
//                }
//                pushStack( solveStack, ( struct StackParameters ) { currentGroup | groupsWithRoll[i]->content[j], i, numGroupsInOrder + 1} );
//            }
//        }
//    }
}
