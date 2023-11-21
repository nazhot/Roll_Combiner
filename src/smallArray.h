#ifndef SMALLARRAY_H_
#define SMALLARRAY_H_
#include <stdint.h>

struct SmallArray{
    int size;
    int32_t *contents;
    int numElements;
};


/**
 * Create of SmallArray of given size
 * 
 * The whole point of SmallArray is to store a boolean as an actual bit, rather
 * than taking up a full byte, with the purpose of having a huge number of
 * elements while being smaller and faster than trying to shove it all into
 * a normal boolean array.
 *
 * TODO: Continue to work towards a hash function for numbers with a certain
 * number of set bits to replace this.
 *
 * @param numberOfElements how many elements should be stored in the array 
 *
 * @return pointer to the newly created SmallArray
 */
struct SmallArray* createSmallArray( const int numberOfElements );

/**
 * Get the bit at element
 *
 * This handles all of the conversion from element to the equivalent index in
 * the SmallArray.
 *
 * @param smallArray SmallArray to get from
 * @param element    index to get, [0 - numberOfElements]
 *
 * @return 0 or 1
 */
int8_t getSmallArrayValue( struct SmallArray* smallArray, const int element );

/**
 * Set the bit at element
 *
 * This handles all of the conversion from element to the equivalent index in
 * the SmallArray.
 *
 * @param smallArray SmallArray to set in
 * @param element    index to set, [0 - numberOfElements]
 */
void   setSmallArrayValue( struct SmallArray* smallArray, const int element );

#endif
