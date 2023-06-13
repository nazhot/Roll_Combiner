#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

int searchTree( struct node *root, unsigned int value ) { 
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

struct node* newNode( unsigned int value ) {
    struct node* temp = ( struct node* ) malloc( sizeof( struct node ) );
    temp->value = value;
    temp->left = temp->right = NULL;
    return temp;
}


struct node* addNode( struct node *node, unsigned int value ) {
    // If the tree is empty, return a new node
    if (node == NULL)
        return newNode(value);
 
    // Otherwise, recur down the tree
    if ( value < node->value ) {
        node->left = addNode( node->left, value );
    }
    else if ( value > node->value ) {
        node->right = addNode( node->right, value );
    }
 
    // Return the (unchanged) node pointer
    return node;
}


long getTreeSize( struct node *node, long num ) {
  
    if ( node == NULL ) {
        return num;
    }

    num++;

    num = getTreeSize( node->left, num );
    num = getTreeSize( node->right, num );

    return num;
}
