#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../include/encode.h"
#include "../include/decode.h"


int main(int argn, char* argv[]) {

	const char* usage =	"encode: $chuff -e filename -b codebook_filename -o encoded_filename\n"\
						"decode: $chuff -d encoded_filename -b codebook_filename -o decoded_filename\n";
	if(argn < 2){
		printf("%s", usage);exit(1);
	}
	const int num = 100;
	char inputFileName[num];
	char codebookFileName[num];
	char outputFileName[num];
	memset(inputFileName, 0, sizeof(inputFileName));
	memset(codebookFileName, 0, sizeof(codebookFileName));
	memset(outputFileName, 0, sizeof(outputFileName));
    int encoding = 0;
	int i = 1;
	for(i = 1; i < argn; i++){
		if(argv[i][0] == '-'){
			switch (argv[i][1])
			{
			case 'e':
			        encoding = 1;
			        if(i+1 < argn) {
						strcpy(inputFileName, argv[++i]);
						inputFileName[num - 1] = 0;
					} 
					else{
						printf("%s", usage);exit(1);
					}
					break;
			case 'b':
				    if(i+1 < argn) {
						strcpy(codebookFileName, argv[++i]);
						codebookFileName[num - 1] = 0;
					}
				    break;
			case 'o':
			        if(i+1 < argn) {
						strcpy(outputFileName, argv[++i]);
						outputFileName[num - 1] = 0;
					}
					break;
			case 'd':
			        if(encoding) {
						printf("%s", usage);exit(1);
					}
			        if(i+1 < argn){
						strcpy(inputFileName, argv[++i]);
						inputFileName[num - 1] = 0;
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
    if(strlen(codebookFileName) == 0){
		strcpy(codebookFileName, "codebook.txt");
	}
	if(strlen(outputFileName) == 0){
		strcpy(outputFileName, "output.txt");
	}
	int response = 0;
	if(encoding){
		response = 	huff_encode_ascii_file(inputFileName, codebookFileName, outputFileName);
	}else{
		response = 	huff_decode_ascii_file(inputFileName, codebookFileName, outputFileName);
	}
    if(response < 0){
		printf("operation failed.\n");
		exit(-1);
	}

	//getchar();
	return 0;
}

