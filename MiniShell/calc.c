//David Shagam
//Unix 352 Calculator assignment2

//precomipler
#define DEBUG 0

//imports
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


//globals
//takes in no arguments.
int main(int argc, char *argv[]){
	char inputCh;
	//exits on EOF
	while((inputCh = fgetc(stdin)) != EOF){
		char *input = calloc(1, sizeof(char));
		char operator[100];
		int seenChar = 1;
		int val1 = 0;
		int val2= 0;
		int sizeIs = 1;
		if(!isspace(inputCh)){
			input[sizeIs -1] = inputCh;
			sizeIs++;
			input = realloc(input, sizeof(char)*sizeIs);
		}
		while((inputCh = fgetc(stdin))!= '\n'){
			if (ispunct(inputCh) && seenChar == 1){
				input[sizeIs-1] = ' ';
				sizeIs++;
				input = realloc(input, sizeof(char)*(sizeIs+1));
				input[sizeIs-1] = inputCh;
				sizeIs++;
				input[sizeIs-1] = ' ';
				sizeIs++;
				input = realloc(input, sizeof(char)*sizeIs);
				seenChar = 0;
			} else if(!isspace(inputCh)){
				input[sizeIs-1] = inputCh;
				sizeIs++;
				input = realloc(input, sizeof(char)*sizeIs);
			}
		}
		//make sure its valid input
		int numMatch = sscanf(input, "%d %s %d", &val1, operator, &val2);
		if(numMatch != 3){
			printf("Invalid input\n");
		} else {
			//do math.
			if(strcmp(operator,"*")== 0){
				printf("%d %c %d = %d\n",val1, '*', val2, val1*val2);
			} else if (strcmp(operator,"/")== 0){
				printf("%d %c %d = %d\n",val1, '/', val2, val1/val2);
			} else if (strcmp(operator,"+")== 0){
				printf("%d %c %d = %d\n",val1, '+', val2, val1+val2);
			} else if (strcmp(operator,"-")== 0){
				printf("%d %c %d = %d\n",val1, '-', val2, val1-val2);
			} else {
				printf("Invalid Operator\n");
			}
		}
		memset(input, 0, sizeof(char)*sizeIs);
	}
	return 0;

}