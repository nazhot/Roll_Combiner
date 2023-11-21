#ifndef FILE_WRITER_H_
#define FILE_WRITER_H_
#include "roll.h"
#include <stdio.h>

/**
 * Create and return the standard output file for orders
 * 
 * This will also set the column names within the created file, and these are
 * not modifiable. If for whatever reason the file cannot be created, the
 * program will exit.
 * 
 * @param fileName path for the output file
 *
 * @return pointer to the created file
 */
FILE* createOutputFile( char *fileName );

/**
 * Write a specific order as a line in the output file
 *
 * No checking of the parameters is done, and the properties/their orders
 * are written in a predefined way, no way to modify.
 *
 * @param outputFile pointer to the output file
 * @param order      unsigned int representation of a valid order
 * @param groups     array of groups that make up the order
 * @param numGroups  how many groups make up the order
 * @param orderStats contains the numberOfRolls and rollList
 */
void writeOrderToFile( FILE *outputFile, unsigned int order, unsigned int *groups, int numGroups, struct OrderStats *orderStats ); 

#endif
