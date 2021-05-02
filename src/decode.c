#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "decode.h"

HuffmanDecodeNode* create_huff_decode_node() {
	HuffmanDecodeNode* node = (HuffmanDecodeNode*)malloc(sizeof(HuffmanDecodeNode));
	if (node != NULL) {
		node->left = NULL;
		node->right = NULL;
		node->symbol = 0;
		node->is_leaf = 0;
	}
	return node;
}

#memory free
void huffman_decode_free_tree(HuffmanDecodeNode* node) {
	if (node == NULL) return;
	huffman_decode_free_tree(node->left);
	huffman_decode_free_tree(node->right);
	free(node);
	node = NULL;
}

void create_huffman_decode_tree(FILE* fp, HuffmanDecodeNode* root_decode) {
	char symbol=0;
	char strcode[HUFF_MAX_LEN];
	int idx=0, length=0;
	int num_input=0;
	HuffmanDecodeNode* curr_node = NULL;
	while (!feof(fp)) {
		memset(strcode, 0, sizeof(strcode));
		symbol = 0;
		// num_input = fscanf_s(fp, "#%c %s\n", &symbol, 1, strcode, sizeof(strcode));
		num_input = fscanf(fp, "#%c %s\n", &symbol, strcode);
		if (num_input != 2) {
			break;
		}
		printf("reading:(%c,%s)successfully\n", symbol, strcode);
		// point curr_node to the root
		curr_node = root_decode;
		length = strlen(strcode);
		printf("length=%d\n", length);
		// create nodes one by one
		for (idx = 0; idx < length; idx++) {
			if (strcode[idx] == '0') {
				if (curr_node->left == NULL) {
					curr_node->left = create_huff_decode_node();
				}
				curr_node = curr_node->left;
			}
			else if(strcode[idx] == '1') { // strcode[idx] == '1'
				if (curr_node->right == NULL) {
					curr_node->right = create_huff_decode_node();
				}
				curr_node = curr_node->right;
			}
			else {
				printf("unexpected char %c\n", strcode[idx]);
				assert(0);
			}
		}

		assert(curr_node->is_leaf == 0);
		// at last assign the symbol to the leaf node
		curr_node->is_leaf = 1;
		curr_node->symbol = symbol;
		printf("successfully inserted symbol:(%c,%s)\n", symbol, strcode);
	}
}

// kodu çözülecek işlev
// f_in, kodlanmış dosyanın dosya göstericisidir
// f_out, kodu çözülmüş mesajı yazmak için dosya göstericisidir


void huff_decode(FILE* f_in, FILE* f_out, HuffmanDecodeNode* root_decode) {
	char c = 0;
	// int num_read = 0;
	// assign the current node to root of decode tree
	HuffmanDecodeNode* curr_node = root_decode;
	while (!feof(f_in)) { // updating c bit by bit (mimic)
		// num_read = fscanf_s(f_in, "%c", &c, 1);
		c = getc(f_in);
		if(c == EOF) break;
		if (c == '0') {
			curr_node = curr_node->left;
		}
		else if (c == '1') {  // c == '1'
			curr_node = curr_node->right;
		}
		else {
			printf("\nchar(%c)rather than 1 or 0 appears\n", c);
			assert(0); // raise error;
		}
		if (curr_node->is_leaf) {
			//printf("%c", curr_node->symbol);
			fprintf(f_out, "%c", curr_node->symbol);
			// need to assign the curr_node to root_decode to start over
			curr_node = root_decode;
		}
	}
}

// bir dosyanın kodunu çözmek için arayüz
int huff_decode_ascii_file(const char* filename, const char* codebook_filename, const char* decoded_filename){
	// decode
	// get the root to decode
	FILE* f_in = NULL;
	FILE* f_out = NULL;
	
	// fopen_s(&f_in, "codebook.txt", "r");
	f_in = fopen(codebook_filename, "r");
	if(f_in == NULL){
		printf("cannot open %s\n.exit.\n", codebook_filename);
		return -1;
	}

	HDecodeNode* root_decode = create_huff_decode_node();
	build_huff_decode_tree(f_in, root_decode);

	fclose(f_in);
	f_in = NULL;
	

	// fopen_s(&f_in, "encoded.txt", "r");
	f_in = fopen(filename, "r");
	if(f_in == NULL){
		printf("cannot open %s\n.exit.\n", filename);
		return -1;
	}
	
	// fopen_s(&f_out, "decoded.txt", "w");
	f_out = fopen(decoded_filename, "w");
	if(f_out == NULL){
		printf("cannot open %s\n.exit.\n", decoded_filename);
		return -1;
	}
	huff_decode(f_in, f_out, root_decode);
	fclose(f_in);
	f_in = NULL;
	fclose(f_out);
	f_out = NULL;

// yığın üzerindeki belleği temizle
	huffman_decode_free_tree(root_decode);
	return 0;
}





