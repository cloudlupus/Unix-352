//David Shagam
//Unix 352 Calculator assignment2
//listf, acts like ls.

//precomipler
#define DEBUG 0
#define null NULL //because caps...

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




//globals
char flags[100];
char (*dirs)[PATH_MAX];
int numDirs = 0;
int l = 0;
int a= 0;
int m = 0;
int c = 0;
char gcwd[PATH_MAX];

int printInf(char *string);
int printMod(struct stat *inf);
int printAccess(struct stat *inf);
int printCreate(struct stat *inf);

int printCommon(char *string, struct stat *pntr, int numPad);

//static int cmpFunc(const void *a, const void *b);

int printFile(char *string, struct stat *pntr, int numPad);


//takes in arguments and information to use.
int main(int argc, char *argv[]){
	//get args
	memset(flags,0, sizeof(flags));
	int it;
	//store out cwd for something.
	getcwd(gcwd, PATH_MAX);
	//check the arguments if it's a flag toggle the flag state. flags start with -.
	for(it = 1; it < argc; it++){
		if(argv[it][0]=='-'){
			//this is a flag
			strcat(flags, argv[it]+1);
			if(strchr(argv[it], 'l')!= NULL){
				l = 1;
			}
			if (strchr(argv[it], 'a')!= NULL){
				a = 1;
			}
			if (strchr(argv[it], 'c')!= NULL){
				c = 1;
			}
			if (strchr(argv[it], 'm')!= NULL){
				m = 1;
			}



		} else {
			//this is a dir/file
			//store all dirs/file in a giant list.
			numDirs++;
			dirs = realloc(dirs, sizeof(*dirs) * numDirs);
			strcpy(dirs[numDirs-1], argv[it]);
			#if DEBUG
				printf("%s\n",dirs[numDirs-1]);
			#endif
		}
	}
	#if DEBUG
		printf("flags is: %s and l is %d a is %d m is %d and c is %d \n", flags, l, a, m ,c);
	#endif
		//if we were provided no dirs/files use the default cwd.
	if( numDirs == 0){
		//CWD!
		char cwd[PATH_MAX];
		if( getcwd(cwd, PATH_MAX) < 0){
			//error no cwd...
		} else {
			#if DEBUG
				printf("%s\n", cwd);
			#endif
			printInf(cwd);
		}

	} else {
		//Print all the files/dirs
		int i;
		for(i = 0; i< numDirs; i++){
			printInf(dirs[i]);
		}
	}
	return 0;
}

//takes a file or dir abs/relative path/name and gets info abotu it and pritns it.
//if it's a directory it grabs everything in the directoy and prints that
//if it's a file prints just the file.
int printInf(char *string){
	struct stat buf;
	if(stat(string, &buf) == -1){
		fprintf(stderr,"Error can't find stat for %s\n",string);
		return 1;
		//
	}

	if (S_ISDIR(buf.st_mode)){
		#if DEBUG
			printf("IT's A DIR!\n");
		#endif
			//This all garbs the files int he direcotry and maintans it.
			//This was to be used for alphabetizing however when I attmpted to figure out
			//qsort i only ever got seg faults.
		int largest = 0;
		char (*getFiles)[PATH_MAX+2]= malloc( sizeof(*getFiles));
		int numFiles = 0;
		DIR *myDir = opendir(string);
		struct dirent *myEnt;
		while((myEnt = readdir(myDir)) != NULL){
			numFiles++;
			getFiles = realloc(getFiles, sizeof(*getFiles) * (numFiles));
			#if DEBUG
				printf("%s  =  ", myEnt ->d_name);
			#endif
			strncpy(getFiles[numFiles-1], myEnt->d_name, strlen(myEnt->d_name)+1);
			#if DEBUG
				printf("%s\n",getFiles[numFiles-1]);
			#endif
			struct stat tempStat;
			chdir(string);
			stat(myEnt -> d_name, &tempStat);
			if( tempStat.st_size > largest){
				largest = tempStat. st_size;
			}
		}
		//calcualte padding
		int numPad = 1;
		chdir(gcwd);
		while((largest = largest/10) != 0){
			numPad++;
		}
		#if DEBUG
			printf("numd padding is %d", numPad);
			printf("size of files is %lu\nand num of files is %d\n", sizeof(getFiles), numFiles);
		#endif
		//getFiles = realloc(getFiles, sizeof(getFiles) * (numFiles+3)*2);
		//qsort(getFiles, numFiles, sizeof(getFiles), cmpFunc );
		
		//for every file print out.
		int i;
		printf("\n");
		printf("%d Entries found\n", numFiles);
		for (i = 0; i < numFiles; i++){
			//printf("%s\n",getFiles[i]);
			struct stat buf2;
			stat(getFiles[i], &buf2);
			printFile(getFiles[i], &buf2, numPad);
		}
		if(!l){
			printf("\n");
		}

	} else {
		#if DEBUG
			printf("IT'S A FILE!\n");
		#endif
		printf("1 Entry Found\n");
		printFile(string, &buf, 0);
		printf("\n");
	}

	return 0;
}

//given the string which is the file, a stat struct, and how much padding
//prints out the information based on the flags.
int printFile(char *string, struct stat *pntr, int numPad){
	if(l && !a && !m &&!c){
		printCommon(string, pntr, numPad);
		//insert the info print here.
		printMod(pntr);
		printf(" %s\n", string);
	} else if(l){
		printCommon(string, pntr, numPad);
		if(a){
			printAccess(pntr);
		}
		if(c){
			printCreate(pntr);
		}
		if(m){
			printMod(pntr);
		}
		printf(" %s\n", string);
	} else {
		printf("%s ", string);
	}

	return 0;
}

//Prints out all the common things from all file types.
int printCommon(char *string, struct stat *pntr, int numPad){
	if(S_ISDIR(pntr->st_mode)){
		printf("d");
	} else if (S_ISLNK(pntr->st_mode)){
		printf("l");
	} else if (S_ISREG(pntr->st_mode)){
		printf("_");
	} else {
		printf("o");
	}
	if(S_IRUSR & pntr->st_mode){
		printf("r");
	} else {
		printf("-");
	}
	if(S_IWUSR & pntr->st_mode){
		printf("w");
	} else {
		printf("-");
	}
	if(S_IXUSR & pntr->st_mode){
		printf("x");
	} else {
		printf("-");
	}
	if(S_IRGRP & pntr->st_mode){
		printf("r");
	} else {
		printf("-");
	}
	if(S_IWGRP & pntr->st_mode){
		printf("w");
	} else {
		printf("-");
	}
	if(S_IXGRP & pntr->st_mode){
		printf("x");
	} else {
		printf("-");
	}
		if(S_IROTH & pntr->st_mode){
		printf("r");
	} else {
		printf("-");
	}
	if(S_IWOTH & pntr->st_mode){
		printf("w");
	} else {
		printf("-");
	}
	if(S_IXOTH & pntr->st_mode){
		printf("x");
	} else {
		printf("-");
	}
	printf(" %d", (int) pntr->st_nlink);
	struct passwd *usr;
	struct group *grp;
	usr = getpwuid(pntr->st_uid);
	grp = getgrgid(pntr->st_gid);
	printf(" %s", usr->pw_name);
	printf(" %s", grp->gr_name);
	printf(" %*u",numPad,(unsigned) pntr->st_size);
	return 0;

}

//Couldn't get this to work. commented out to prevent compiler errors.

/*static int cmpFunc(const void *a, const void *b){
	fprintf(stderr, "compared a which is %s and b which is %s\n",(char *)a, (char *) b);
	return strcasecmp( * (char * const *) a, * (char * const *) b);
	//return strcasecmp( *(char **)a, * (char **)b);
}*/

//takes in a stat struct and prints the last Modification date.
int printMod(struct stat *inf){
	printf(" M");
	char timest[30];
	time_t tempT = inf->st_mtim.tv_sec;
	struct tm *getTime = localtime(&tempT);
	strftime(timest, 30, "%D-%R", getTime);
	printf("%s",timest);
	return 0;
}
//takes in a stat struct and prints the last Access date
int printAccess(struct stat *inf){
	printf(" A");
	char timest[30];
	time_t tempT = inf->st_atim.tv_sec;
	struct tm *getTime = localtime(&tempT);
	strftime(timest, 30, "%D-%R", getTime);
	printf("%s",timest);
	return 0;
}
//Takes in a stat struct and prints the creation date
int printCreate(struct stat *inf){
	printf(" C");
	char timest[30];
	time_t tempT = inf->st_ctim.tv_sec;
	struct tm *getTime = localtime(&tempT);
	strftime(timest, 30, "%D-%R", getTime);
	printf("%s",timest);
	return 0;
}
