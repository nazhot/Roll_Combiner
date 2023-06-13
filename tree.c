#include "tree.h"
#include <stdio.h>

int searchTree(struct node *root, unsigned int value) { 
    if ( root->value == value ) {
        return 1;
    }  

    if ( root->left == NULL  && root->right == NULL ) {
        return 0;
    }

    if ( value > root->value ) {
        return searchTree( root->right, value );
    }

    return searchTree( root->left, value );

}
