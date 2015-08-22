#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "HeapTestEngine.h"
#define DEBUG 1
#define REQUEST_SIZE 1024
#define UNIT_SIZE 16

//MEMORY MUST BE ALLOCATED IN 8 BYTE CHUNKS. unsigned is 4 bytes, pntr is 8 bytes, struct is 16 bytes.

int totalSpace = 0;
int totalUsed = 0;
int totalFree = 0;
struct header{
	struct header *pntr;
	unsigned size;
};
typedef struct header Header;
static Header *freePntr = NULL; // pointer to free list.
static Header start; //first actual header

void printFree(){
	printf("Block at 0x%x, size is %u, next free is at 0x%x, next block at 0x%x\n", freePntr->pntr, freePntr->size, freePntr->pntr, freePntr->pntr + (freePntr->size));

}

void free352(void *ptr){
	printf("got into my free352 \n");
	Header *startF, *cur;
	startF = (Header *)ptr-1; // get the header structure
	printf("startF is 0x%x\n", startF);
	//loop through until we get back to the start.
	for(cur = freePntr; cur != startF; cur = cur->pntr ){ //current location starts at free, crawl through and loop so long as we haven't found a place for start, move to next pntr
		if(cur < startF && cur->pntr > startF){ // we live here. inbetween cur and cur->pntr
			totalFree += startF->size;
			totalUsed -= startF->size;

			if(cur != &start){
				startF->pntr = cur->pntr;
			} else {
				startF ->pntr = startF;
			}
			cur -> pntr = startF;
			printf("inbetween case hit\n");
		} else if(( startF < cur->pntr || startF > cur ) && cur->pntr <= cur ) {// cur points behind itself or to itself && value of start > cur we live after it
			totalFree += startF->size;
			totalUsed -= startF->size;
			if(cur != &start){
				startF ->pntr = cur ->pntr;
			} else {
				startF ->pntr = startF;
			}
			cur -> pntr = startF;
			printf("edge case hit\n");
		}

		if(cur + cur->size == cur->pntr){// we can coalless;
			cur -> size += (cur->pntr)->size;
			cur -> pntr = (cur->pntr)->pntr;
			printf("coallesed\n");
		}
	}
	printf("cur is 0x%x\n", cur);
	freePntr = cur;

}

void *malloc352(int nbytes){
	void *requested;
	Header *cur, *prev;
	Header *fitAddr = NULL;
	unsigned neededUnits = (nbytes/UNIT_SIZE) + ((nbytes%UNIT_SIZE)? 2: 1) ;// calculate how many units of size UNIT_SIZE are needed to fit header+ data
	if( freePntr == NULL){// first call
		//start off a dummy list.
		start.pntr = &start;
		start.size = 0;
		freePntr = &start;
	}
	prev = freePntr; // set prev to start.
	for(cur = prev ->pntr ; ; prev = cur, cur = cur-> pntr){ // set cur = prev's pntr, no logic check infinte loop until we return, crawl down pointer chain.
		



		if (cur == freePntr && fitAddr == NULL){ // need mroe space
			printf("got to my if\n");
			requested = sbrk(REQUEST_SIZE);
			((Header *)requested)->size = REQUEST_SIZE/UNIT_SIZE;
			printf("requested before free is 0x%x\n",requested);

			totalSpace += REQUEST_SIZE/UNIT_SIZE;
			free352(((Header*)requested)+1);
			break;
		}
	}
	/*if( (neededUnits) > totalFree){
		requested = sbrk(REQUEST_SIZE);
		totalSpace += REQUEST_SIZE/UNIT_SIZE;
		totalFree += REQUEST_SIZE/UNIT_SIZE;

		//first call.
		if( freePntr == NULL ){
			start.pntr = (struct header*)requested;
			start.size = REQUEST_SIZE/UNIT_SIZE;
			freePntr = &start;
		}
	} else
	{// else can't fit hard case eg fragments: look for fragmented case

	}*/
	#if DEBUG
		printf("total free is %d\n", totalFree);
		printf("Values at first place, pntr is 0x%x, size is %d\n", ((struct header*)requested)->pntr, ((struct header*)requested)->size);
		printf("Requested is %x\n", requested);
		printf("requested +1 is %x\n", ((Header *)requested)+1);
		printf("start-pntr is 0x%x and size is %u\n", start.pntr, start.size);
		printf("freePntr is 0x%x\n", freePntr);
		printf("Values at freepntr, pntr is 0x%x, size is %u\n", freePntr->pntr, freePntr->size);
		printf("\n");
		printFree();
	#endif
	return NULL;
}

int main(int argc,char *argv[]) {
	#if DEBUG
		 void *temp = malloc352(100);
		 //free352(temp);
		/*char test;
		unsigned test2 = (100/UNIT_SIZE)+((100%UNIT_SIZE)? 2: 1);
		unsigned test3 = (16%UNIT_SIZE)? 1:2;
		printf("needed units is %u \n", test2);
		printf("test 3 is %u\n", test3);
		printf("tested\n");
		printf("sizeof %d bytes\n", sizeof(Header));
		printf("sizeof unsigned %d bytes\n", sizeof( start.size));
		printf("sizeof pntr %d bytes\n", sizeof(start.pntr));
		printf("sizeof a char %d bytes\n", sizeof(test));*/
	#endif
	//heap_test();
	return 0;
}