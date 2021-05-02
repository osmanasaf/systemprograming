#ifndef HUFF_ENCODE_H
#define HUFF_ENCODE_H

#include "huff_const.h"


struct huff_encode_node {
	struct huff_encode_node* left;   //  kodlama ağacında kullanılır; '0'
	struct huff_encode_node* right;  //  kodlama ağacında kullanılır; '1'
	struct huff_encode_node* parent; //  kodlama ağacında kullanılır
	struct huff_encode_node* next;   //  öncelik kuyruğunda kullanılır
	double freq;                     //  dosyadaki sembolün sıklığı
	int    is_leaf;                  //  düğüm, kodlama ağacında bir yaprak düğüm ise
	char   symbol;                   //  bu düğüme atanan sembol (char)
};


typedef struct huff_encode_node HEncodeNode;

HEncodeNode* create_huff_encode_node(char symbol, double freq, int is_leaf);

void insert_huff_pqueue(HEncodeNode* node, HEncodeNode** q_head);

void disp_huff_pqueue(HEncodeNode* q_head);

HEncodeNode* pop_huff_pqueue(HEncodeNode** q_head);

void generate_huff_codebook(HEncodeNode* root, int depth, char* codebook);

void write_huff_codebook(FILE* f_out, char* codebook);

void write_huff_encode_stream(FILE* f_out, char* str, char* codebook);

void free_huff_encode_tree(HEncodeNode* root);

void build_huff_encode_tree(const char* str_arr, const double* freq_arr, size_t len, HEncodeNode** q_head);

void build_huff_encode_tree256(double* freq_arr, size_t len, HEncodeNode** q_head);

void write_huff_encode_stream_from_file(FILE* f_in, FILE* f_out, char* codebook);

int huff_encode_ascii_file(const char* filename, const char* codebook_filename, const char* encoded_filename);

void huff_count_char(double* freq_arr, FILE* f_in, size_t len);

#endif