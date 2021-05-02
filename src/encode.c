#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "encode.h"

// huffman kodlama düğümü için bellek ayrıldı
HEncodeNode* create_huff_encode_node(char symbol, double freq, int is_leaf) {
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

// öncelik kuyruğuna bir düğüm eklemek için kullanılan işlev
void insert_huff_pqueue(HEncodeNode* node, HEncodeNode** q_head) {
	printf("inserting node: (%c, %f)\n", node->symbol, node->freq);
	if (*q_head == NULL) { // pqueue boş olduğunda
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
// pqueue başlangıcında düğümü ekle
	if (curr_node == *q_head) {
		node->next = *q_head;
		*q_head = node;
	}
	else { // prev_node ve curr_node arasına düğüm ekle
		prev_node->next = node;
		node->next = curr_node;
	}
}

// öncelik sırasını görüntülemek için kullanılan işlev
void disp_huff_pqueue(HEncodeNode* q_head) {
	printf("priority queue: ");
	while (q_head) {
		printf("(%c, %f),", q_head->symbol, q_head->freq);
		q_head = q_head->next;
	}
	printf("\n");
}

// öğeyi pqueue'dan pop yapmak için kullanılan işlevler
HEncodeNode* pop_huff_pqueue(HEncodeNode** q_head) {
	if (*q_head == NULL) return NULL;
	HEncodeNode* p_node = *q_head;
	*q_head = (*q_head)->next;
	printf("popped: (%c, %f)\n", p_node->symbol, p_node->freq);
	return p_node;
}


// kod kitabını yinelemeli olarak oluşturmak için kullanılan işlevler
void generate_huff_codebook(HEncodeNode* root, int depth, char* codebook) {
	if (root->is_leaf) { // kodlama ağacının altına ulaşıyoruz
		int len = depth;
		char symbol = root->symbol;
		// dizenin sonuna 0 ekleyin
		*(codebook + ((size_t)symbol) * HUFF_MAX_LEN + len) = 0;
		// alttan (yaprak) üste (kök) başlayın
		HEncodeNode* parent = root->parent;
		while ((parent != NULL) && (len > 0)) {
			// kök ebeveynin solundadır
			if (root == parent->left) {
				*(codebook + ((size_t)symbol) *  HUFF_MAX_LEN + (--len)) = '0';
			}
			else { // kök ebeveynin hakkıdır
				*(codebook + ((size_t)symbol) *  HUFF_MAX_LEN + (--len)) = '1';
			}
			root = parent;
			parent = root->parent;
		}
		// output
		printf("built code: (%c, %s)\n", symbol, codebook + ((size_t)symbol) *  HUFF_MAX_LEN);
	}
	else {
		generate_huff_codebook(root->left, depth + 1, codebook);
		generate_huff_codebook(root->right, depth + 1, codebook);
	}
}
// kod kitabını dosyaya yazmak için kullanılan işlevler
void write_huff_codebook(FILE* f_out, char* codebook) {
	int i = 0;
	for (i = 0; i < HUFF_MAX_SYMBOLS; i++) {
		if (*(codebook + i * HUFF_MAX_LEN)) { // char i sembolünün strcode'u boş olmadığında
			fprintf(f_out, "#%c %s\n", i, codebook + i * HUFF_MAX_LEN);
		}
	}
}

// kodlanmış bit akışını dosyaya yazmak için kullanılan işlevler
void write_huff_encode_stream(FILE* f_out, char* str, char* codebook) {
	while (*str) {
		fprintf(f_out, "%s", codebook + (size_t)(*str) * HUFF_MAX_LEN);
		str++;
	}
}

// huffman kodlama ağacının belleğini boşaltmak için kullanılan işlevler.
void free_huff_encode_tree(HEncodeNode* root) {
	if (root == NULL) return;
	free_huff_encode_tree(root->left);
	free_huff_encode_tree(root->right);
	free(root);
	root = NULL;
}

// huffman kodlama ağacı oluşturmak için kullanılan işlevler
void build_huff_encode_tree(const char* str_arr, const double* freq_arr, size_t len, HEncodeNode** q_head) {
	size_t i = 0;
	HEncodeNode* left = NULL;
	HEncodeNode* right = NULL;
	HEncodeNode* parent = NULL;
// tüm öğeleri bu pqueue'ya ekle
// burada bu adımda yaprak düğümler oluşturuyoruz
	for (i = 0; i < len; i++) {
		insert_huff_pqueue(
			create_huff_encode_node(str_arr[i], freq_arr[i], 1),
			q_head);
	}
// len-1 öğelerini açın ve aşağıdan yukarıya düğümler oluşturun
	for (i = 0; i < len - 1; i++) {
		left = pop_huff_pqueue(q_head);
		right = pop_huff_pqueue(q_head);
		parent = create_huff_encode_node(0, left->freq + right->freq, 0);
		parent->left = left;
		parent->right = right;
		left->parent = parent;
		right->parent = parent;
		insert_huff_pqueue(parent, q_head);
	}
}

// huffman kodlama ağacını yalnızca freq dizisiyle oluşturmak için kullanılan işlevler
void build_huff_encode_tree256(double* freq_arr, size_t len, HEncodeNode** q_head) {
	assert(len == 256 && 256 <= HUFF_MAX_SYMBOLS);
	size_t i = 0;
	HEncodeNode* left = NULL;
	HEncodeNode* right = NULL;
	HEncodeNode* parent = NULL;
// tüm öğeleri bu pqueue'ya ekle
// burada bu adımda yaprak düğümler oluşturuyoruz
	size_t valid_char_num = 0;
	for (i = 0; i < len; i++) {
		if (freq_arr[i] > 0.0f) { // olmayan düğümleri dikkate almayın
			insert_huff_pqueue(
				create_huff_encode_node((char)i, freq_arr[i], 1),
				q_head);
			valid_char_num++;
		}
	}
	// valid_char_num-1 öğelerini açın ve aşağıdan yukarıya düğümler oluşturun
	for (i = 0; i < valid_char_num - 1; i++) {
		left = pop_huff_pqueue(q_head);
		right = pop_huff_pqueue(q_head);
		parent = create_huff_encode_node(0, left->freq + right->freq, 0);
		parent->left = left;
		parent->right = right;
		left->parent = parent;
		right->parent = parent;
		insert_huff_pqueue(parent, q_head);
	}
}

// orijinal dosyadan okurken kodlanmış dosyayı yazmak için kullanılan işlevler
void write_huff_encode_stream_from_file(FILE* f_in, FILE* f_out, char* codebook) {
	char c;
	while (!feof(f_in)) {
		c = getc(f_in);
		if(c == EOF) break;
		fprintf(f_out, "%s", codebook + (size_t)c * HUFF_MAX_LEN);
	}
}

// ASCII karakterlerini saymak için kullanılan işlev
void huff_count_char(double* freq_arr, FILE* f_in, size_t len) {
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
// bir dosyayı kodlamak için interface
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
	huff_count_char(freq_arr, f_in, HUFF_MAX_SYMBOLS);
	fclose(f_in);
	f_in = NULL;
    
	HEncodeNode* q_head = NULL;
	HEncodeNode* root_encode = NULL;
	
	char codebook[HUFF_MAX_SYMBOLS][HUFF_MAX_LEN];
	memset(codebook, 0, sizeof(codebook));
	build_huff_encode_tree256(freq_arr, HUFF_MAX_SYMBOLS, &q_head);
	root_encode = pop_huff_pqueue(&q_head);
	generate_huff_codebook(root_encode, 0, &codebook[0][0]);
	
	FILE* f_out = NULL;
	// fopen_s(&f_out, "codebook.txt", "w");
	f_out = fopen(codebook_filename, "w");
	if(f_out == NULL){
		printf("cannot open %s\n.exit.\n", codebook_filename);
		return -1;
	}
	write_huff_codebook(f_out, &codebook[0][0]);
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
	write_huff_encode_stream_from_file(f_in, f_out, &codebook[0][0]);
	fclose(f_in);
	fclose(f_out);
	free_huff_encode_tree(root_encode);
	return 0;
}



