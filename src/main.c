#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "encode.h"
#include "decode.h"

// kodlama hakkında örnek
void encode_example() {
	puts("\nexamples about huffman encode.");
	// kuyruğu test edin
	double freq_arr[] = { 0.01,0.04,0.05,0.11,0.19,0.20,0.4 };
	char   char_arr[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g' };
	const size_t len = 7;
	size_t i = 0;
	HEncodeNode* q_head = NULL;
	HEncodeNode* root = NULL;
	HEncodeNode* encode_node = NULL;

	for (i = 0; i < len; i++) {
		insert_huff_pqueue(
			create_huff_encode_node(char_arr[i], freq_arr[i], 1),
			&q_head
		);
	}

	disp_huff_pqueue(q_head);
	puts("the popping order should be from smallest to largest.");
	while ((encode_node = pop_huff_pqueue(&q_head))) {
		free(encode_node);
	}

	// kodlamayı test et
	q_head = NULL;
	char codebook[HUFF_MAX_SYMBOLS][HUFF_MAX_LEN];
	// belleği ayarla
	memset(codebook, 0, sizeof(codebook));

	build_huff_encode_tree(char_arr, freq_arr, len, &q_head);

	// kodlama ağacının kökünü al
	root = pop_huff_pqueue(&q_head);
	//codebook oluştur
	generate_huff_codebook(root, 0, &codebook[0][0]);

	// codebook yazdır
	FILE* f_out = NULL;
	f_out = fopen("codebook.txt", "w");
	// print codebook
	write_huff_codebook(stdout, &codebook[0][0]);
	// write codebook
	write_huff_codebook(f_out, &codebook[0][0]);
	fclose(f_out);
	f_out = NULL;

	// write bit stream;
	
	f_out = fopen("encoded.txt", "w");

	write_huff_encode_stream(f_out, "abba cafe bad", &codebook[0][0]);
	write_huff_encode_stream(stdout, "abba cafe bad", &codebook[0][0]);
	fclose(f_out);
	f_out = NULL;
	free_huff_encode_tree(root);
}

// huffman kod çözme hakkında örnek
void decode_example() {
	puts("\nexamples about huffman decode");
	
	FILE* f_in = NULL;
	FILE* f_out = NULL;;

	// kodu çözülecek kökü al
	HDecodeNode* root_decode = create_huff_decode_node();


	f_in = fopen("codebook.txt", "r");
	build_huff_decode_tree(f_in, root_decode);
	fclose(f_in);
	f_in = NULL;

	
	f_in = fopen("encoded.txt", "r");
	f_out = fopen("decoded.txt", "w");
	huff_decode(f_in, f_out, root_decode);
	fclose(f_in);
	f_in = NULL;
	fclose(f_out);
	f_out = NULL;

	// yığın üzerindeki belleği temizle
	free_huff_decode_tree(root_decode);
}

void example_encode_decode_ascii_file(){
	int response = 0;
	
	response = huff_encode_ascii_file("book.txt", "codebook.txt", "encoded.txt");
	if(response < 0) exit(-1);
    
	response = huff_decode_ascii_file("encoded.txt", "codebook.txt", "decoded.txt");
	if(response < 0) exit(-1);

}


int main(int argn, char* argv[]) {

	const char* usage = "github.com/ludlows\n"\
	                    "huffman coding for educational purpose.\n"\
						"encode: $chuff -e filename -b codebook_filename -o encoded_filename\n"\
						"decode: $chuff -d encoded_filename -b codebook_filename -o decoded_filename\n";
	if(argn < 2){
		printf("%s", usage);exit(1);
	}
	const int num = 100;
	char input_filename_buff[num];
	char codebook_filename_buff[num];
	char output_filename_buff[num];
	memset(input_filename_buff, 0, sizeof(input_filename_buff));
	memset(codebook_filename_buff, 0, sizeof(codebook_filename_buff));
	memset(output_filename_buff, 0, sizeof(output_filename_buff));
    int encoding = 0;
	int i = 1;
	for(i = 1; i < argn; i++){
		if(argv[i][0] == '-'){
			switch (argv[i][1])
			{
			case 'e':
			        encoding = 1;
			        if(i+1 < argn) {
						strcpy(input_filename_buff, argv[++i]);
						input_filename_buff[num - 1] = 0;
					} 
					else{
						printf("%s", usage);exit(1);
					}
					break;
			case 'b':
				    if(i+1 < argn) {
						strcpy(codebook_filename_buff, argv[++i]);
						codebook_filename_buff[num - 1] = 0;
					}
				    break;
			case 'o':
			        if(i+1 < argn) {
						strcpy(output_filename_buff, argv[++i]);
						output_filename_buff[num - 1] = 0;
					}
					break;
			case 'd':
			        if(encoding) {
						printf("%s", usage);exit(1);
					}
			        if(i+1 < argn){
						strcpy(input_filename_buff, argv[++i]);
						input_filename_buff[num - 1] = 0;
					}
					else{
						printf("%s", usage);exit(1);
					}
			        break;
			default:
			        printf("%s", usage);exit(1);
				    break;
			}

		}
		else{
			printf("%s", usage);exit(1);
		}

	}
	// set defualt value
    if(strlen(codebook_filename_buff) == 0){
		strcpy(codebook_filename_buff, "codebook.txt");
	}
	if(strlen(output_filename_buff) == 0){
		strcpy(output_filename_buff, "output.txt");
	}
	int response = 0;
	if(encoding){
		response = 	huff_encode_ascii_file(input_filename_buff, codebook_filename_buff, output_filename_buff);
	}else{
		response = 	huff_decode_ascii_file(input_filename_buff, codebook_filename_buff, output_filename_buff);
	}
    if(response < 0){
		printf("operation failed.\n");
		exit(-1);
	}

	//getchar();
	return 0;
}

