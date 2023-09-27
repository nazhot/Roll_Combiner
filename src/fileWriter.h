#ifndef FILE_WRITER_H_
#define FILE_WRITER_H_
#include "roll.h"
#include <stdio.h>

FILE* createOutputFile( char *fileName );
void writeOrderToFile( FILE outputFile, unsigned int order, unsigned int *groups, int numGroups, struct OrderStats *orderStats ); 

#endif
