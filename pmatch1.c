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



void pmatch1( char inputWord[64], char  inputFileName[64], FILE* intermediateFile) {

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
					if(line[strlen(line) - 1] == '\n')
						fprintf(intermediateFile, "%s,%d: %s", inputFileName, lineNo, line);
					else
						fprintf(intermediateFile, "%s,%d: %s\n", inputFileName, lineNo, line);

					break;  // break for loop
				}
			}
			p += 1; // next char
		}
	}
	fclose(inputFile);
 }


int main(int argc, char* argv[]) {

	if (argc < 2) exit(EXIT_FAILURE);
	char* inputWord = argv[1];
	int inputCount = strtol(argv[2],NULL, 10);
	FILE* intermediateFile  =  fopen("interm.txt", "wb");

	for (int i = 3; i < 3 + inputCount; i++) {
		char* filename = argv[i];
		pid_t n = fork();
		if (n == 0) { 
			pmatch1(inputWord, filename, intermediateFile);
			return(0);
		}
	}
	//Wait for Child processes to return
	for (int i = 3; i < 3 + inputCount; i++)
		wait(NULL);
	fclose(intermediateFile);

	char* outputFileName = argv[3 + inputCount];
	FILE* outputFile = fopen(outputFileName, "wb");

	//Sort intermediate file into output file
	for (int i = 3; i < 3 + inputCount; i++) {
		intermediateFile = fopen("interm.txt", "r");
		fseek(intermediateFile, 0, SEEK_SET );
		char* filename = argv[i];
		char line[256];
		while (fgets(line, sizeof(line), intermediateFile)) {
			char p[256];
			strcpy(p, line);
			if (strcmp(strtok(p, ","), filename) == 0) {
				fprintf(outputFile, "%s", line);
			}
		}
		fclose(intermediateFile);
	}
	fclose(outputFile);
	remove("interm.txt");
	
    return 0;

}



