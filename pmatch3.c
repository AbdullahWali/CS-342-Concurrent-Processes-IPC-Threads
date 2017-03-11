// Abdullah Wali


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#define READ_END 0
#define WRITE_END 1


struct thread_args {
	char inputWord[64];
	char  inputFileName[64];
	struct Node** linkedHeads;
	int index;
};


struct Node
{
	char data[256];
	struct Node* next;

};

struct Node* createNode(char* data) {
	struct Node* node;
	node = (struct Node*) malloc(sizeof(struct Node));
	strcpy(node->data, data);
	node->next = NULL;
	return node;
}


void insert(struct Node* root, char* data) {
	if (root == NULL) {
		printf("Linked List not created");
		return;
	}
	while ( root->next != NULL) { 
		root = root->next;
	}
	root->next = createNode(data);
}

void delete(struct Node* root) {
	if (root == NULL) return;
	struct Node* next = root->next;
	free(root);
	delete(next);
}


void* pmatch3(void *ptr) {

	struct thread_args * args = (struct thread_args* )ptr;
	char *inputWord = args->inputWord;
	char  *inputFileName = args->inputFileName;
	struct Node** linkedHeads = args->linkedHeads;
	int index = args->index;

	FILE* inputFile = fopen(inputFileName, "r");
	if (!inputFile) exit(EXIT_FAILURE);
	int lineNo = 0;
	char line[256];
	while (fgets(line, sizeof(line), inputFile)) {
		lineNo++;

		const char* p = line;
		for (;;)
		{
			p = strstr(p, inputWord);
			if (p == NULL) break; //If no match found

			if ((p == line) || !isalnum(p[-1])) //if match found check that the word is seperate 
			{
				p += strlen(inputWord);
				if (!isalnum(*p))
				{
					char str[256];
					sprintf(str, "%s,%d: %s", inputFileName, lineNo, line);
					if (linkedHeads[index] == NULL ) { 
						linkedHeads[index] = createNode(str);
					}
					else { 
						insert(linkedHeads[index], str);
					}
					break;  // break for loop
				}
			}
			p += 1; // next char
		}
	}
	printf( "\n");	
	fclose(inputFile);
	return 0;
 }


int main(int argc, char* argv[]) {

	if (argc < 2) exit(EXIT_FAILURE);
	char* inputWord = argv[1];
	int inputCount = strtol(argv[2],NULL, 10);

	struct Node** linkedHeads = malloc(sizeof(struct Node) * inputCount);
	for (int i = 0; i < inputCount; i++) linkedHeads[i] = NULL;

	pthread_t* threads = malloc (sizeof(pthread_t)* inputCount);

	struct thread_args *allArgs = malloc (sizeof(struct thread_args)* inputCount);

	for (int i = 3; i < 3 + inputCount; i++) {
		char* filename = argv[i];
		struct thread_args *args = &allArgs[i-3];
		strcpy(args->inputWord ,inputWord);
		strcpy(args->inputFileName ,filename);
		args->linkedHeads = linkedHeads;
		args->index = i-3;
		pthread_create(&threads[i-3], NULL,  pmatch3, args);
	}

	for (int i = 0; i < inputCount; i++ ) pthread_join(threads[i], NULL); 

	free(allArgs);


	char* outputFileName = argv[3 + inputCount];
	FILE* outputFile = fopen(outputFileName, "wb");



	for (int i = 0; i < inputCount; i++) {
		struct Node* cur = linkedHeads[i];
		while ( cur != NULL) {
			fprintf(outputFile, "%s", cur->data);
			cur = cur->next;
		}
	}

	free(linkedHeads);
	
	fclose(outputFile);
    return 0;
}



