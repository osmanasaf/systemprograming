/*
codes for huffman encode tree
www.github.com/ludlows
2019-04
*/
#ifndef HUFF_ENCODE_H
#define HUFF_ENCODE_H

#include "huff_const.h"


struct huffEncodeNode {
	struct huffEncodeNode* left;   
	struct huffEncodeNode* right;  
	struct huffEncodeNode* parent; 
	struct huffEncodeNode* next;  
	double freq;                     
	int    is_leaf;                  
	char   symbol;                   
};

typedef struct huffEncodeNode HEncodeNode;


HEncodeNode* createHuffEncodeNode(char symbol, double freq, int is_leaf);

void insertHuffQueue(HEncodeNode* node, HEncodeNode** q_head);

void dispHuffQueue(HEncodeNode* q_head);

HEncodeNode* popHuffQueue(HEncodeNode** q_head);

void generateHuffCodebook(HEncodeNode* root, int depth, char* codebook);

void writeHuffCodebook(FILE* f_out, char* codebook);

void writeHuffEncodeStream(FILE* f_out, char* str, char* codebook);




#endif 

