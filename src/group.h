#ifndef  GROUP_H_
#define GROUP_H_

struct Group {
    unsigned int rolls;
    float length;
};

struct GroupArray {
    struct Group *content;
    int size;
    int length;
    int sizeAddConstant;
    float sizeAddMultiplier;
    int lastNonRollIndex: 4;
    int firstNonRollIndex: 4;
};

struct GroupArray* createGroupArray( int arraySize, int sizeAddConstant, float sizeAddMultiplier );
struct GroupArray* addToGroupArray( struct GroupArray* groupArray, struct Group toAdd ); 
struct GroupArray* shrinkGroupArray( struct GroupArray* groupArray );

#endif
