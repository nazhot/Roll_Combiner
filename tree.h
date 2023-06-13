#ifndef TREE_H_
#define TREE_H_

struct node {
    unsigned int value;
    struct node *left;
    struct node *right;
};


int searchTree( struct node*, unsigned int );

struct node* addNode( struct node*, unsigned int );

long getTreeSize( struct node* , long );

#endif
