#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "huff_encode.h"




HEncodeNode* createHuffEncodeNode(char symbol, double freq, int is_leaf) {
	HEncodeNode* p_node = (HEncodeNode*)malloc(sizeof(HEncodeNode));
	if (p_node != NULL) {
		p_node->left = NULL;
		p_node->right = NULL;
		p_node->parent = NULL;
		p_node->next = NULL;
		p_node->freq = freq;
		p_node->is_leaf = is_leaf;
		p_node->symbol = symbol;
	}
	return p_node;
}



void insertHuffQueue(HEncodeNode* node, HEncodeNode** q_head) {
	printf("inserting node: (%c, %f)\n", node->symbol, node->freq);
	if (*q_head == NULL) { // when the pqueue is empty
		*q_head = node;
		return;
	}
	HEncodeNode* curr_node = NULL;
	HEncodeNode* prev_node = NULL;
	curr_node = *q_head;
	while ((curr_node != NULL) && (curr_node->freq < node->freq)) {
		prev_node = curr_node;
		curr_node = curr_node->next;
	}

	if (curr_node == *q_head) {
		node->next = *q_head;
		*q_head = node;
	}
	else { 
		prev_node->next = node;
		node->next = curr_node;
	}
}


void dispHuffQueue(HEncodeNode* q_head) {
	printf("priority queue: ");
	while (q_head) {
		printf("(%c, %f),", q_head->symbol, q_head->freq);
		q_head = q_head->next;
	}
	printf("\n");
}



HEncodeNode* popHuffQueue(HEncodeNode** q_head) {
	if (*q_head == NULL) return NULL;
	HEncodeNode* p_node = *q_head;
	*q_head = (*q_head)->next;
	printf("popped: (%c, %f)\n", p_node->symbol, p_node->freq);
	return p_node;
}



void generateHuffCodebook(HEncodeNode* root, int depth, char* codebook) {
	if (root->is_leaf) { 
		int len = depth;
		char symbol = root->symbol;	
		*(codebook + ((size_t)symbol) * HUFF_MAX_LEN + len) = 0;
		HEncodeNode* parent = root->parent;
		while ((parent != NULL) && (len > 0)) {
			if (root == parent->left) {
				*(codebook + ((size_t)symbol) *  HUFF_MAX_LEN + (--len)) = '0';
			}
			else { 
				*(codebook + ((size_t)symbol) *  HUFF_MAX_LEN + (--len)) = '1';
			}
			root = parent;
			parent = root->parent;
		}
		printf("built code: (%c, %s)\n", symbol, codebook + ((size_t)symbol) *  HUFF_MAX_LEN);
	}
	else {
		generateHuffCodebook(root->left, depth + 1, codebook);
		generateHuffCodebook(root->right, depth + 1, codebook);
	}
}


void writeHuffCodebook(FILE* f_out, char* codebook) {
	int i = 0;
	for (i = 0; i < HUFF_MAX_SYMBOLS; i++) {
		if (*(codebook + i * HUFF_MAX_LEN)) { 
			fprintf(f_out, "#%c %s\n", i, codebook + i * HUFF_MAX_LEN);
		}
	}
}


void writeHuffEncode(FILE* f_out, char* str, char* codebook) {
	while (*str) {
		fprintf(f_out, "%s", codebook + (size_t)(*str) * HUFF_MAX_LEN);
		str++;
	}
}











