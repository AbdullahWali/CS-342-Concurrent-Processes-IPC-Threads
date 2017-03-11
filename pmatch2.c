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
#include <assert.h>

#define READ_END 0
#define WRITE_END 1



void pmatch2( char inputWord[64], char  inputFileName[64],int* pd) {

	dup2(pd[WRITE_END], STDOUT_FILENO);
	close(pd[READ_END]);
    close(pd[WRITE_END]);
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
					printf( "%s,%d: %s", inputFileName, lineNo, line);
					break;  // break for loop
				}
			}
			p += 1; // next char
		}
	}
	printf( "\n");	
	fclose(inputFile);
 }


int main(int argc, char* argv[]) {

	if (argc < 2) exit(EXIT_FAILURE);
	char* inputWord = argv[1];
	int inputCount = strtol(argv[2],NULL, 10);
	int pd[2]; //Pipe Descriptors
	if (pipe(pd) == -1) {
		printf("Pipe Creation Failed\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 3; i < 3 + inputCount; i++) {
		char* filename = argv[i];
		pid_t n = fork();
		if (n == 0) { 
			pmatch2(inputWord, filename, pd);
			return(0);
		}
	}
	//Wait for Child processes to return
	for (int i = 3; i < 3 + inputCount; i++)
		wait(NULL);
	
	//move the descriptor to STDIN and close open descriptors
	dup2(pd[READ_END], STDIN_FILENO);
    close(pd[WRITE_END]);
    close(pd[READ_END]);

	char* outputFileName = argv[3 + inputCount];
	FILE* outputFile = fopen(outputFileName, "wb");

	char lineBuffer[256];
	size_t strContentSize = 1;
	char* str = malloc (sizeof(char)*256);
	str[0] = '\0'; // make null-terminated so that strcat can be used


	while (fgets(lineBuffer, sizeof(lineBuffer), stdin)){
		strContentSize += sizeof(lineBuffer);
		str = realloc(str, strContentSize);
		strcat(str,lineBuffer);
	}

	for (int i = 3; i < 3 + inputCount; i++) {
		char* filename = argv[i];
		char* curLineRead = str;	//Pointer to string, will iterate over '\n' s 
		while (curLineRead) {
			char* c  = strchr(curLineRead , '\n');
			if (c != NULL) {
				*c = '\0'; //NULL terminate temporarily 

				char p[256]; //need to use a temporary char array since strtok modifies the ptr passed
				strcpy(p, curLineRead);
				char * tokenized = strtok(p, ",");
				if (tokenized!= NULL &&  strcmp(tokenized, filename) == 0) { //if line starts with correct filename, print it to output file
					fprintf(outputFile, "%s", curLineRead);	
				}
				*c = '\n';
			}
			if (c== NULL) curLineRead = NULL;
			else curLineRead = c+1;
		}
	}
	
	fclose(outputFile);
	free(str);
    return 0;
}



