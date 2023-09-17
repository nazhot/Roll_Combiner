#ifndef INT_ARRAY_H_
#define INT_ARRAY_H_


struct IntArray {
    unsigned int *content;
    int size;
    int length;
    int sizeAddConstant;
    float sizeAddMultiplier;
};


struct IntArray* createIntArray( int, int, float );
struct IntArray* addToIntArray( struct IntArray*, unsigned int );
void addToIntArrayNoResize( struct IntArray*, unsigned int );
void freeIntArray( struct IntArray* );
struct IntArray* shrinkIntArray( struct IntArray* );


#endif
