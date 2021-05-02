#ifndef DECODE_H
#define DECODE_H

#include "huffman_const.h"

// huffman kod çözme düğümünün yapısını tanımla

// define the structure of huffman decode node
struct huff_decode_node {
	struct huff_decode_node* left;  //  0 temsil eder
	struct huff_decode_node* right; // 1 temsil eder
	int is_leaf;
	char symbol;
};

// yukarıdaki yapının kısaltılmış adı
typedef struct huff_decode_node HDecodeNode;

// huffman kod çözme düğümü oluşturmak için kullanılan işlev
HuffmanDecodeNode* create_huff_decode_node();

// kod çözme ağacına ayrılan belleği boşaltmak için kullanılan işlev
void huffman_decode_free_tree(HuffmanDecodeNode* node);

//huffman kod çözme ağacı oluşturma işlevi.
void create_huffman_decode_tree(FILE* fp, HuffmanDecodeNode* root_decode);

// kodu çözülecek işlev
// f_in, kodlanmış dosyanın dosya göstericisidir
// f_out, kodu çözülmüş mesajı yazmak için dosya göstericisidir
void huff_decode(FILE* f_in, FILE* f_out, HuffmanDecodeNode* root_decode);

// bir dosyanın kodunu çözmek için arayüz
int huff_decode_ascii_file(const char* filename, const char* codebook_filename, const char* decoded_filename);

#endif