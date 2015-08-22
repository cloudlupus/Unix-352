//David Shagam
//Unix 352 Calculator assignment2

//precomipler
#define DEBUG 0
#define null NULL //because caps...
#define INTERNAL 1
#define EXTERNAL 0

//imports
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/wait.h>

int quit = 0;
int redIn = 0;
char inName[NAME_MAX+1];
int redOut = 0;
char outName[NAME_MAX+1];
char startDir[PATH_MAX];
char **argVec;
int argCNum;

int pwd();
int myexit();
int cd(char *string);
int findCmd();
int doCmd(int isInternal);

//runs until exit or cntrl d is pressed
//takes input from the user and tries to run commands from teh input.

int main(int argc, char *argv[]){
	char inputCh;
	//inits the CWD for use on calc and listf.
	getcwd(startDir, PATH_MAX);
	//while we aren't quitting.
	while(!quit){
		//promtp the user
		printf("$> ");
		//reset things that need to be rest.
		memset(inName, 0, NAME_MAX+1);
		memset(outName, 0, NAME_MAX+1);
		redOut =0;
		redIn =0;
		argCNum = 0;
		//create a string of arbitrary length as it's being typed. Also looking for ctrld
		char *input = calloc(1, sizeof(char));
		int sizeIs = 0;
		while ((inputCh= fgetc(stdin)) != '\n' && inputCh != '\0'){
			if( inputCh == EOF){
				quit = 1;
				break;
			} else {
				sizeIs++;
				input = realloc(input, sizeof(input)*(sizeIs));
				input[sizeIs-1] = inputCh;
			}
		}
		if(quit){
			printf("\n");
			break;
		}
		//Tokenize the user input by whitespace and fill out the argument vector
		//grab redirects
		//and grab command name.
		int firstToken =1;
		char *token = strtok(input, " \t");
		while(token != NULL){
			//cmd name
			if(firstToken && token[0] != '\n'){
				argCNum++;
				argVec = realloc(argVec, sizeof(char*) * argCNum);
				argVec[argCNum-1] = strdup(token);
				firstToken = 0;
			}else{
				//input
				if(token[0] == '<'){
					if(redIn ==0){
						redIn = 1;
						if(strlen(token)>1){
							strncpy(inName, token+1, NAME_MAX);
						} else {
							token = strtok(NULL, " \t");
							if(token != NULL){
								strncpy(inName, token, NAME_MAX);
							} else {
								redIn = 0;
							}
						}
					} else {
						if(strlen(token)>1){

						} else {
							token = strtok(NULL, " \t");
						}
					}
				} else if (token[0] == '>'){ // output
					if(redOut == 0){
						redOut = 1;
						if(strlen(token)>1){
							strncpy(outName, token+1, NAME_MAX);
						} else {
							token = strtok(NULL, " \t");
							if(token != NULL){
								strncpy(outName, token, NAME_MAX);
							} else {
								redOut = 0;
							}
						}
					} else {
						if(strlen(token)>1){

						} else {
							token = strtok(NULL, " \t");
						}
					}
					//it's an argument.
				} else if( token[0] != '>' && token[0] != '<' && token[0] != '\n') {
					argCNum++;
					argVec = realloc(argVec, sizeof(char*) * argCNum);
					argVec[argCNum-1] = strdup(token);
					#if DEBUG
					#endif
				}
			}
			//get the next token assuming the current token isn't already null.
			if(token != NULL){
				token = strtok(NULL," \t");
			}
		}
		//add in a null to the vector list.
		argVec = realloc(argVec, ( sizeof(char*) * argCNum) + 1);
		argVec[argCNum] =  NULL;
	

		#if DEBUG
		if(redOut){
			printf("redOut is %s\n", outName);
		}
		if(redIn){
			printf("redIn is %s\n", inName);
		}


		int iter;
		for (iter =0; iter < argCNum; iter++){
			if(argVec[iter] == NULL){
				printf("the %d th element is NULL\n", iter);

			} else {
				printf(" the element here is %s\n", argVec[iter]);
			}
		}
		#endif
		//if we have a valid vector array find the command
		if(argVec[0] != NULL){
			findCmd();
		}
	}

	return 0;

}

//quits the program
int myexit(){
	quit = 1;
	return 0;
}

//prints the CWD
int pwd(){
	char temp[PATH_MAX];
	getcwd(temp, PATH_MAX);
	printf("%s ",temp);
	return 0;
}

//changes the directory
int cd(char *string){
	if(chdir(string)<0){
		printf("No such directory: %s\n",string);
		return 1;
	} else {
		printf("cwd changed to ");
		pwd();
		printf("\n");
		return 0;
	}
}

//finds commands and if theya re internal or external (interanl means calc or listf)
//external are thigns like ls.
int findCmd(){
	if(strcmp(argVec[0],"exit")==0){
		myexit();
	} else if (strcmp(argVec[0], "cd")==0){
		cd(argVec[1]);
	} else if(strcmp(argVec[0],"pwd")==0){
		pwd();
	} else if( strcmp(argVec[0], "listf")==0){
		doCmd(INTERNAL);
	} else if (strcmp(argVec[0], "calc")==0){
		doCmd(INTERNAL);
	} else {
		doCmd(EXTERNAL);
	}
	return 0;
}


//does the command

int doCmd(int isInternal){
	pid_t id;
	int childInf;
	char path[PATH_MAX+NAME_MAX];
	sprintf(path, "%s/%s",startDir, argVec[0]);

	if((id = fork()) < 0){
		printf("Fork Failed\n");
	} else if ( id == 0 ){
		//child do stuff.
		//do redirect stuff.
		if(redIn){
			freopen(inName, "r", stdin);
		}
		if(redOut){
			freopen(outName, "w",stdout);
		}

		if(isInternal == INTERNAL){
			if(execvp(path, argVec) < 0){
				printf("Exect Failed\n");
			}
		} else {
			execvp(argVec[0], argVec);
		}
	} else {
		//parent wait.
		wait(&childInf);
	}
	return 0;
}