#ifndef INT_ARRAY_H_
#define INT_ARRAY_H_


struct int_array {
    unsigned int *content;
    int size;
    int length;
    int sizeAddConstant;
    float sizeAddMultiplier;
};


struct int_array* createIntArray( int, int, float );
struct int_array* addToIntArray( struct int_array*, unsigned int );
void freeIntArray( struct int_array* );
struct int_array* shrinkIntArray( struct int_array* );


#endif
