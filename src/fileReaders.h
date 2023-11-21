#ifndef FILE_READERS_H_
#define FILE_READERS_H_
#include "roll.h"

/**
 * Parse the given roll file, and initialize/set OrderStats for the program
 *
 * Roll file lines are expected to be in the format rollId:rollLength. Line
 * lengths are capped at 100 characters, and rollId's are capped at
 * MAX_ID_LENGTH (defined within roll.h).
 *
 * @param rollFilePath path to the file with the rolls
 *
 * @return pointer to OrderStats with rollList set based on the input file
 */
struct OrderStats* readRollFile( char *rollFilePath );

#endif
