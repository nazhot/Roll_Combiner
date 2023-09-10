#ifndef ROLL_H_
#define ROLL_H_
#define MAX_ID_LEN 15

struct Roll {
    char id[MAX_ID_LEN];
    float length;
    int numGroups;
};

struct OrderStats {
    int minRollsPerGroup: 5;
    int maxRollsPerGroup: 5;

    int minRollsPerOrder: 5;
    int maxRollsPerOrder: 5;
    int minGroupsPerOrder: 5;
    int maxGroupsPerOrder: 5;

    int minGroupLength;
    int maxGroupLength;

    int minOrderLength;
    int maxOrderLength;
};

void printRollsFromInt(  unsigned int, int, struct Roll* );
float rollsLength(  unsigned int, int, struct Roll* );

#endif
