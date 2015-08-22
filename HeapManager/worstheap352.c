/*
David Shagam
Heap Manager Worst Fit Version Unix 352
4/26/15
*/
//General imports
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/times.h>
#include "HeapTestEngine.h"

//Definitions
#define DEBUG 1
#define REQUEST_SIZE 1024
#define UNIT_SIZE 16

//MEMORY MUST BE ALLOCATED IN 8 BYTE CHUNKS. unsigned is 4 bytes, pntr is 8 bytes, struct is 16 bytes.
//Globals
unsigned totalSpace = 0;
unsigned totalUsed = 0;
unsigned totalFree = 0;
clock_t startT, endT;
struct tms buff1, buff2;

struct header{
	struct header *pntr;
	unsigned size;
};

typedef struct header Header;
static Header *freePntr = NULL; // pointer to free list.
static Header start; //first actual header

/*
	Takes nothing, returns nothing
	Print Free prints the contents of the free list(s)
*/
void printFree(){
	//Prints out the freepntr
	if(freePntr != NULL){
		if (freePntr + (freePntr->size) == freePntr->pntr){
			printf("*****Block at 0x%p, size is %u, next free is at 0x%p, next block at 0x%p*****\n", freePntr, freePntr->size, freePntr->pntr, freePntr + (freePntr->size));
		}else {
			printf("Block at 0x%p, size is %u, next free is at 0x%p, next block at 0x%p\n", freePntr, freePntr->size, freePntr->pntr, freePntr + (freePntr->size));
		}
		Header *cur, *prev;
		prev = freePntr;
		//Prints out the remainder of the things in the free list.
		for (cur= freePntr->pntr; cur != prev; cur= cur->pntr){
			if(cur ->pntr == cur+ cur->size){
				printf("*****Block at 0x%p, size is %u, next free is at 0x%p, next block at 0x%p*****\n", cur, cur->size, cur->pntr, cur + (cur->size));
			} else {
				printf("Block at 0x%p, size is %u, next free is at 0x%p, next block at 0x%p\n", cur, cur->size, cur->pntr, cur->pntr + (cur->size));
			}
		}
	}else {
		printf("Free list is empty\n");	
	}
	//prints the block information
	printf("Number of free blocks: %u, Number of used blocks: %u, total number of blocks requested: %u\n", totalFree, totalUsed, totalSpace);
	printf("\n");
}

/*
	Takes a void *pntr returns nothing
	This returns space to the heap for use later.
*/
void free352(void *ptr){
	//local vars
	Header *startF, *cur;
	int added =0;
	int coallsed =0;
	startF = ((Header *)ptr)-1; // get the header structure

	//loop through until we get back to the start.
	//current location starts at free, crawl through and loop so long as we haven't found a place for start, move to next pntr
	for(cur = freePntr; cur != startF; cur = cur->pntr ){ 
		if(added == 0){ // used for inserting once and starting coallessing
			if(cur < startF && cur->pntr > startF){ // we live here. inbetween cur and cur->pntr
				totalFree += startF->size;
				totalUsed -= startF->size;

				if(cur != &start){ // special case of a null list
					startF->pntr = cur->pntr;
				} else {
					startF ->pntr = startF;
				}
				cur -> pntr = startF;
				added = 1;

			// cur points behind itself or to itself && value of start > cur we live after it
			} else if(( startF < cur->pntr || startF > cur ) && cur->pntr <= cur ) {
				totalFree += startF->size;
				totalUsed -= startF->size;
				added = 1;
				if(cur != &start){ //Special case of a null list
					startF ->pntr = cur ->pntr;
				} else {
					startF ->pntr = startF;
				}
				cur -> pntr = startF;
			}
		}
		//Count loops through list.
		if(cur == freePntr && added == 1){
			coallsed+=1;
		}
		//NECESARY DO NOT REMOVE
		if(coallsed >=3){
			break;
		}

		if(cur + cur->size == cur->pntr){// we can coalless;
			cur -> size += (cur->pntr)->size;
			cur -> pntr = (cur->pntr)->pntr;
			freePntr = cur;
		}
	}

	freePntr = cur; // maintains integrity of freelist

	#if DEBUG
		printFree();
	#endif

}

/*
	Takes integer representing a number of bytes returns a void* pointer to a location on the heap
	This function is used for providing heap space for data allocation to another program.
*/
void *malloc352(int nbytes){
	void *requested;
	Header *cur, *prev;
	Header *fitAddr = NULL;
	int fitSize = -1;
	// calculate how many units of size UNIT_SIZE are needed to fit header+ data
	unsigned neededUnits = (nbytes/UNIT_SIZE) + ((nbytes%UNIT_SIZE)? 2: 1);

	if( freePntr == NULL){// first call
		//start off a dummy list.
		start.pntr = &start;
		start.size = 0;
		freePntr = &start;
	}
	prev = freePntr; // set prev to start.

	// set cur = prev's pntr, no logic check infinte loop until we return, crawl down pointer chain.
	for(cur = prev ->pntr ; ; prev = cur, cur = cur-> pntr){ 
		
		if (cur -> size == neededUnits){// exact fit
			if(prev ->pntr == cur->pntr){//Only one thing in empty list
				freePntr = NULL;
			} else{// else backtrack for consistency
				freePntr = prev;
			}

			prev ->pntr = cur->pntr;

			totalFree -=neededUnits;
			totalUsed += neededUnits;
			
			#if DEBUG
				printFree();
			#endif

			return (void *)(cur+1);

		// if the current size is greater than fitSize or fitSize isn't initalized AND we aren't the special pointer case.
			//We have a smaller thing that fits what we want.
		}else if((cur->size > fitSize || fitSize == -1) && cur->size >= neededUnits ){ 
			fitSize = cur->size;
			fitAddr = cur;
		}


		if (cur == freePntr && fitAddr == NULL){ // need mroe space due to looping and not finding space
			requested = sbrk(REQUEST_SIZE);
			((Header *)requested)->size = REQUEST_SIZE/UNIT_SIZE;

			totalSpace += REQUEST_SIZE/UNIT_SIZE;
			totalUsed += REQUEST_SIZE/UNIT_SIZE;
			free352(((Header*)requested)+1);
		} else if(fitAddr != NULL) {// else we are too large and have looped

			fitAddr -> size -= neededUnits;
			fitAddr += fitAddr->size;

			totalFree -=neededUnits;
			totalUsed +=neededUnits;
			fitAddr -> size = neededUnits;

			#if DEBUG
				printFree();
			#endif

			return (void *)(fitAddr+1);
		}
	}
	return NULL;
}

/*
	Main takes nothing, returns nothing
	prints a time output.
	If debug = 1 in the defines we print out free list a lot (prepare for a big file)
*/
int main(int argc,char *argv[]) {
	//stat time
	startT = times(&buff1);

	heap_test();
	//end time
	endT = times(&buff2);
	//calculate time
	long clockTicks= sysconf(_SC_CLK_TCK);
	double userTime =(buff2.tms_utime - buff1.tms_utime)/((double) clockTicks);
	double sysTime = (buff2.tms_stime - buff1.tms_stime)/((double)clockTicks);
	double totalTime = sysTime+userTime;
	double wallTime = (endT - startT)/ ((double)clockTicks);
	printf("Total time taken was %f, user time was %f, and system time was %f, total wall clock time taken: %f\n", totalTime, userTime, sysTime, wallTime);

	return 0;
}