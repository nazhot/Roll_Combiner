#ifndef TRIE_H_
#define TRIE_H_

struct trieNode {
    int isEndpoint;
    struct trieNode *zero;
    struct trieNode *one;
};

int lastOnePosition( unsigned int );
int searchTrie( struct trieNode*, unsigned int );
int addTrieNode( struct trieNode*, unsigned int );
struct trieNode* getTrieNode();

#endif
