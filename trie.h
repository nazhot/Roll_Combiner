#ifndef TRIE_H_
#define TRIE_H_
#include <stdint.h>

struct trieNode {
    int8_t isEndpoint;
    struct trieNode *zero;
    struct trieNode *one;
};

int lastOnePosition( unsigned int );
int searchTrie( struct trieNode*, unsigned int );
int addTrieNode( struct trieNode*, unsigned int );
int deleteTrie( struct trieNode* );
int findCompatibleGroups( struct trieNode*, unsigned int, unsigned int, int, int*, struct trieNode* );
struct trieNode* getTrieNode();

#endif
