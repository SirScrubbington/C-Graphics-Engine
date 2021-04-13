/*
* Name: cs.c
* Author: Damon Murdoch (s2970548)
* Description: Queue Implementation
* Creation Date: 12/09/2017
*/

/*
* To use this file, include the following libraries in your program.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
*/

#include "queue.h"

// initialise_queue(s,q): void
// Initialises a queue structure at the given queue pointer q of size s.
void initialise_queue(int s,struct Queue * q){
	q->size=s;
	q->itemc=0;
	int i;
	for(i=0;i<q->size;i++){
		q->items[i]=0;
	}
}

// push(n,q): int
// Pushes an item to the back of the queue if space is available. Returns 1 if successful, 0 on failure.
int push(int n,struct Queue * q){
	if(q->itemc < q->size){
		q->items[q->itemc++]=n;
		return 0;
	}
	return 1;
}

// pop(q): int
// Pops the top item off the queue if there are any items available, returns the variable if successful, 0 if failure.
int pop(struct Queue * q){
	if(q->itemc > 0){
		int ret = q->items[0];
		int i;
		for(i=1;i<q->itemc;i++){
			q->items[i-1]=q->items[i];
		}
		q->itemc--;
		return ret;
	}
	return -1;
}