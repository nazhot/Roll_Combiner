#ifndef INT_ARRAY_H_
#define INT_ARRAY_H_


struct IntArray {
    unsigned int *content;
    int size;
    int length;
    int sizeAddConstant;
    float sizeAddMultiplier;
};

/**
 * Create and return a default IntArray, length set to 0, content untouched 
 *
 * Simple attempt at making a dynamic array for just unsigned ints,
 * specifically for this use case. It can be added to using 
 * {@link addToIntArray}.
 *
 * @param size              how many elements large the array should be
 * @param sizeAddConstant   size increase when trying to add to a full array
 * @param sizeAddMultiplier newize = size * sizeAddMultiplier when trying to
 *                          add to a full array
 *
 * @return newly initialized IntArray
 */
struct IntArray* createIntArray( const int size, const int sizeAddConstant, const float sizeAddMultipler );

/**
 * Append an unsigned int to the array
 *
 * When you try to insert another element when
 * length == size, it will realloc content to be size * sizeAddMultiplier +
 * sizeAddConstant.
 *
 * @param intArray array to be added to
 * @param toAdd    element to add
 * 
 * @return pointer back to the IntArray in case of realloc
 */
struct IntArray* addToIntArray( struct IntArray *intArray, unsigned int toAdd );

/**
 * Append an unsigned int to the array, without the ability to resize
 *
 * This is some incredible unsafe code. It doesn't check anything at all, it is
 * purely aimed at being blazingly fast. All it does is insert the element at
 * intArray->length, and increment the length.
 *
 * This was created for the situation where I knew I was going to be going
 * through another array, and only keeping certain elements, and needing a new
 * array rather than modifying it in place. Under those exact conditions, the
 * new array could take the size of old array, and know it wouldn't ever have
 * to resize.
 *
 * @param intArray array to be added to
 * @param toAdd    element to add
 */
void addToIntArrayNoResize( struct IntArray *intArray, unsigned int toAdd );

/**
 * Free the elements of an IntArray, and then free it
 *
 * @param intArray IntArray to free 
 */
void freeIntArray( struct IntArray *intArray );

/**
 * Realloc content size to be length
 *
 * This was aimed at cleaning up some of the arrays that I had to either wildly
 * guess an initial size for, or that were getting updated with a large
 * sizeAddConstant/sizeAddMultiplier. Running this for every IntArray created
 * after it was filled caused a big bottleneck all to shrink arrays that had an
 * incredibly short lifetime, so now it is mainly used for arrays made early on
 * whose lifetimes are the whole program's runtime.
 *
 * @param intArray array to shrink
 *
 * @return pointer to the IntArray, in case of realloc update
 */
struct IntArray* shrinkIntArray( struct IntArray *intArray );

#endif
