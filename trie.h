#ifndef TRIE_H_
#define TRIE_H_

struct trieNode {
    int isEndpoint;
    struct trieNode *zero;
    struct trieNode *one;
};


int searchTrie( struct trieNode*, unsigned int, int );

struct trieNode* addTrieNode( struct trieNode*, unsigned int, int );

long getTrieSize( struct node* , long );

#endif
