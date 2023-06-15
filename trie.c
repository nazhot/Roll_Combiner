#include "trie.h"
#include <stdio.h>
#include <stdlib.h>

int lastOnePosition( unsigned int value ) {
    if ( value == 0 ) {
        return 0;
    }
    int lastOnePos = -1;
    int intSize = sizeof( unsigned int ) * 8 - 1;
    for ( int i = intSize ; i >= 0; i-- ) {
        if ( value >> i & 1 ) {
            lastOnePos = i;
            break;
        }
    }

    return lastOnePos; 
}

int searchTrie( struct trieNode *root, unsigned int value ) {
    struct trieNode *child = root;
    int lastOnePos = lastOnePosition( value );
    for ( int i = 0; i <= lastOnePos; i++ ) {
        int isOne = value >> i & 1;
        child = isOne ? child->one : child->zero;
        if ( child == NULL ) {
            return 0;
        }
    }
    return child->isEndpoint;
}

struct trieNode* getTrieNode(){
    struct trieNode *newNode = ( struct trieNode* ) malloc( sizeof( struct trieNode ) );
    if ( newNode ) {
        newNode->one = NULL;
        newNode->zero = NULL;
        newNode->isEndpoint = 0;
    }
    return newNode;
}

//give the root node 
int addTrieNode( struct trieNode *root, unsigned int value ) {
    struct trieNode *child = root;
    int lastOnePos = lastOnePosition( value );

    for ( int i = 0; i <= lastOnePos; i++ ) {
        int isOne = value >> i & 1;
        if ( isOne ) {
            if ( !child->one ) {
                child->one = getTrieNode();
            }
            child = child->one;
        } else {
            if ( !child->zero ) {
                child->zero = getTrieNode();
            }
            child = child->zero;
        }
    }
    int isAdded = !child->isEndpoint;
    child->isEndpoint = 1;
    return isAdded;
    
}

int deleteTrie( struct trieNode *trieNode ) {
    
    if ( trieNode->one ) {
        deleteTrie( trieNode->one );
    }

    if ( trieNode->zero ) {
        deleteTrie( trieNode->zero );
    }

    free( trieNode );


    return 1;
}

int findCompatibleGroups( struct trieNode *root, unsigned int value, unsigned int nodeValue, int nodeLevel, int *numAdded, struct trieNode *secondRoot ) {

    if ( root == NULL ) {
        return 0;
    }

    int searchBoth = ! (value & 1 );

    if ( root->isEndpoint ) {
        *numAdded = *numAdded + addTrieNode( secondRoot, value ^ nodeValue );
        //printf( "Found a match! %u\n", nodeValue );
    }

    if ( searchBoth ) {
        findCompatibleGroups( root->one, value >> 1, nodeValue ^ ( 1 << nodeLevel ), nodeLevel + 1, numAdded, secondRoot );
    }
    
    findCompatibleGroups( root->zero, value >> 1, nodeValue, nodeLevel + 1, numAdded, secondRoot );
    return 1;
}

