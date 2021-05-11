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




void huffEncodeTree(HEncodeNode* root) {
	if (root == NULL) return;
	huffEncodeTree(root->left);
	huffEncodeTree(root->right);
	free(root);
	root = NULL;
}


void buildHuffEncodeTree(const char* str_arr, const double* freq_arr, size_t len, HEncodeNode** q_head) {
	size_t i = 0;
	HEncodeNode* left = NULL;
	HEncodeNode* right = NULL;
	HEncodeNode* parent = NULL;

	for (i = 0; i < len; i++) {
		insertHuffQueue(
			createHuffEncodeNode(str_arr[i], freq_arr[i], 1),
			q_head);
	}

	for (i = 0; i < len - 1; i++) {
		left = popHuffQueue(q_head);
		right = popHuffQueue(q_head);
		parent = createHuffEncodeNode(0, left->freq + right->freq, 0);
		parent->left = left;
		parent->right = right;
		left->parent = parent;
		right->parent = parent;
		insertHuffQueue(parent, q_head);
	}
}


void buildHuffEncodeTree256(double* freq_arr, size_t len, HEncodeNode** q_head) {
	assert(len == 256 && 256 <= HUFF_MAX_SYMBOLS);
	size_t i = 0;
	HEncodeNode* left = NULL;
	HEncodeNode* right = NULL;
	HEncodeNode* parent = NULL;
	
	size_t valid_char_num = 0;
	for (i = 0; i < len; i++) {
		if (freq_arr[i] > 0.0f) { 
			insertHuffQueue(
				createHuffEncodeNode((char)i, freq_arr[i], 1),
				q_head);
			valid_char_num++;
		}
	}
	for (i = 0; i < valid_char_num - 1; i++) {
		left = popHuffQueue(q_head);
		right = popHuffQueue(q_head);
		parent = createHuffEncodeNode(0, left->freq + right->freq, 0);
		parent->left = left;
		parent->right = right;
		left->parent = parent;
		right->parent = parent;
		insertHuffQueue(parent, q_head);
	}
}


void writeHuffEncodeFromFile(FILE* f_in, FILE* f_out, char* codebook) {
	char c;
	while (!feof(f_in)) {
		c = getc(f_in);
		if(c == EOF) break;
		fprintf(f_out, "%s", codebook + (size_t)c * HUFF_MAX_LEN);
	}
}

void huffCount(double* freq_arr, FILE* f_in, size_t len) {
	assert(len == 256 && len <= HUFF_MAX_SYMBOLS);
	char c;
	double s = 0.0f;
	while ((c=getc(f_in)) != EOF) {
		freq_arr[(size_t)c] += 1.0f;
		s += 1.0f;
	}
	size_t i = 0;
	for (i = 0; i < len; i++) {
		if (freq_arr[i] > 0.0f) {
			freq_arr[i] = freq_arr[i] / s;
		}
	}
}


int huff_encode_ascii_file(const char* filename, const char* codebook_filename, const char* encoded_filename){
	FILE* f_in = fopen(filename, "r");
	if(f_in == NULL){
		printf("cannot open %s\n.exit.\n", filename);
		return -1;
	}
	int i = 0;
	double freq_arr[HUFF_MAX_SYMBOLS];
	for (i = 0; i < HUFF_MAX_SYMBOLS; i++) {
		freq_arr[i] = 0.0f;
	}
	huffCount(freq_arr, f_in, HUFF_MAX_SYMBOLS);
	fclose(f_in);
	f_in = NULL;
    
	HEncodeNode* q_head = NULL;
	HEncodeNode* root_encode = NULL;
	
	char codebook[HUFF_MAX_SYMBOLS][HUFF_MAX_LEN];
	memset(codebook, 0, sizeof(codebook));
	buildHuffEncodeTree256(freq_arr, HUFF_MAX_SYMBOLS, &q_head);
	root_encode = popHuffQueue(&q_head);
	generateHuffCodebook(root_encode, 0, &codebook[0][0]);
	
	FILE* f_out = NULL;
	f_out = fopen(codebook_filename, "w");
	if(f_out == NULL){
		printf("cannot open %s\n.exit.\n", codebook_filename);
		return -1;
	}
	writeHuffCodebook(f_out, &codebook[0][0]);
	fclose(f_out);

	f_in = fopen(filename, "r");
	if(f_in == NULL){
		printf("cannot open %s\n.exit.\n", filename);
		return -1;
	}

	f_out = fopen(encoded_filename, "w");
	if(f_out == NULL){
		printf("cannot open %s\n.exit.\n", encoded_filename);
		return -1;
	}
	writeHuffEncodeFromFile(f_in, f_out, &codebook[0][0]);
	fclose(f_in);
	fclose(f_out);
	huffEncodeTree(root_encode);
	return 0;
}


