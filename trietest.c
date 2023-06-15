#include "trie.h"
#include <stdlib.h>
#include <stdio.h>

int main( int argc, char **argv ) {
    
    struct trieNode *root = getTrieNode();

    unsigned int topNumber = 50000000;

    for ( unsigned int i = 1; i < topNumber; i += 3 ) {
        addTrieNode( root, i );
        //printf( "Number %u: %s\n", i, addTrieNode( root, i) ? "Added" : "Not Added" );
    }

    printf( "Added First Nodes\n" );

    for ( unsigned int i = 0; i < topNumber; i += 4 ) {
        addTrieNode( root, i );
    }
    
    printf( "Added Second Nodes\n" );

    for ( unsigned int i = 0; i < topNumber; i += 2 ) {
        searchTrie( root, i );
        //printf( "Number %u: %s\n", i, searchTrie( root, i ) ? "Found" : "Not Found" );
    }
/*
    addTrieNode( root, 1 );
    addTrieNode( root, 5 );
    addTrieNode( root, 12 );
    addTrieNode( root, 13 );
*/

    printf( "Finding compatible groups\n" );
    findCompatibleGroups( root, 2, 0, 0 );
    printf( "Deleting Trie\n" );
    deleteTrie( root );
    printf( "Deleted\n" );
    printf( "%p\n", root );

    return 0;
}
