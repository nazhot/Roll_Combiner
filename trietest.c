#include "trie.h"
#include <stdlib.h>
#include <stdio.h>


int main( int argc, char **argv ) {
    
    struct trieNode *root = getTrieNode();

    for ( unsigned int i = 1; i < 50; i += 3 ) {
        printf( "Number %u: %s\n", i, addTrieNode( root, i) ? "Added" : "Not Added" );
    }

    for ( unsigned int i = 0; i < 50; i += 2 ) {
        printf( "Number %u: %s\n", i, searchTrie( root, i ) ? "Found" : "Not Found" );
    }

    return 0;
}
