/*
 * Conor Smyth 12452382
 * Adam O'Flynn 12378651
 * All work is our own
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

/* Declare constants */

#define SLEEP 5
#define MAXROOM 8000 
#define ARRAY_SIZE 5

/* define all structs */

typedef struct wakeupCall_t { //stuct for wake up time
	int roomNumber;
	time_t callTime;
} wakeupCall_t;

typedef struct Heap {
	struct wakeupCall_t times[ARRAY_SIZE];
	int numElements;
} Heap;

typedef struct logs_t { //struct for holding logs
	int generated;
	int pending;
	int called;
	int expired;
} logs_t;

typedef struct { //struct for holding shared data
	logs_t log;
	Heap heap;
} sharedData_t;

/* Heap methods */

void addTime(Heap *heap, wakeupCall_t c) {
	heap->numElements++;
	heap->times[heap->numElements] = c;
	int current = heap->numElements;
	if(current == 1){
		resizeHeap(heap->times);
		printf("Resizing array");
	}
			
	while(heap->times[current / 2].callTime > c.callTime) {
		heap->times[current] = heap->times[current / 2];
		current /= 2;
	}

	heap->times[current] = c;
}
	
void showHeap(Heap james){
	int i;
	for(i = 1; i < james.numElements; i++){
		printf("%s\n", ctime(&james.times[i].callTime));
	}
}

void removePriorty(Heap *heap){
	wakeupCall_t min = heap->times[1];
	printf("Wakeup ya cunt:\t%04d %s\n\n\n", heap->times[1].roomNumber, ctime(&heap->times[1].callTime));
	heap->times[1] = heap->times[heap->numElements--];
	fixHeap(heap, 1);
}

void fixHeap(Heap *heap, int root){
	int childNode;
	wakeupCall_t tmp = heap->times[root];
	for(; root * 2 <= heap->numElements ; root = childNode){
		childNode = root * 2;
		if(childNode != heap->numElements && heap->times[childNode + 1].callTime < heap->times[childNode].callTime)
			childNode++;
		if(heap->times[childNode].callTime < tmp.callTime)
			heap->times[root] = heap->times[childNode];
		else break;	
	}
	heap->times[root] = tmp;
	//printf("Wakeup ya cunt:\t%04d %s\n", heap->times[root].roomNumber, ctime(&heap->times[root].callTime));
}	
void resizeHeap(wakeupCall_t c[]){
	wakeupCall_t tmp[] = (wakeupCall_t)realloc(c, ARRAY_SIZE * sizeof(c) );
	printf("IN MEtHOD :)");
	//void *_tmp = realloc(c, ( * sizeof(DATA)));
	c = tmp;	
}



/* Initialisation methods */

void initLog(logs_t *log) {
	log->generated = 0;
	log->pending = 0;
	log->called = 0;
	log->expired = 0;
}

void initHeap(Heap *harry) {
	harry->numElements = 0;
}


void initData(sharedData_t *data) {
	initLog(&data->log);
	initHeap(&data->heap);
}

/* Random number generators */

static int getRandomNumber(int x) {
	unsigned int seedp = time(NULL);
	return (rand_r(&seedp) % x) + 1;
}//generate a random number

static int getRandomRoom() {
	return getRandomNumber(MAXROOM);
}//return a random room number

static int getRandomCall() {
	return time(NULL) + getRandomNumber(100);
}//return random wake up time

int randomRoom() {
	return getRandomRoom();
}//get a random room

int randomCall() {
	return getRandomCall();
}//get a random call time

static int getRandomSleep() {
	return getRandomNumber(SLEEP);
}//returns random sleep time

static void randomSleep() {
	sleep(getRandomSleep());
}//sleep for a random time

/* wakeupCall methods */

static wakeupCall_t newCall() {
	wakeupCall_t c;
	c.roomNumber = randomRoom();
	c.callTime = randomCall();
	return c;
}//generate a new wake up call

static void showCall(wakeupCall_t c) 
{
	//needs to add to data structure
	printf("Registering:\t%04d %s\n", c.roomNumber, ctime(&c.callTime));
}

static void showWakeup(wakeupCall_t c){
	printf("Wakeup ya cunt:\t%04d %s\n", c.roomNumber, ctime(&c.callTime));
}
	
/* log methods */

void logNew(logs_t *log) {
	log->generated++;
}

/* at the moment just generates random wake up calls at random times */

static void * guest(void *data_in) {
	sharedData_t *data = data_in;
	int count = 0;
	while(1) {
		//sleep for random second
		randomSleep();
		//generate a wake up call
		wakeupCall_t call = newCall();
		showCall(call);

		/* add the call to the heap */
		addTime(&data->heap, call);
		showHeap(data->heap);
		/*if(count == 3){
			printf("Removing a node");
			removePriorty(&data->heap);
			count = 0;
		}*/
		//log the new call
		logNew(&data->log);
		printf("%d\n", data->log.generated);
		count++;
	}
}

/*static void * waiter(void *data_in) {
	sharedData_t *data = data_in;
	sigset_t set;
	int sig;

	while(pthread_cond_timedwait() = ) {
		//if no data wait
		//get top of heap
		//remove top of heap
		//if data available sleep until time
		//make call(message)
		//log call made
	}
}*/


int main() {
	//delcare shared data
	sharedData_t data;

	//initialise shared data
	initData(&data);

	//initialise threads
	pthread_t guest_t;
	//pthread waiter_t;

	//create threads
	pthread_create(&guest_t, NULL, &guest, (void *)&data);
	//pthread_create(&waiter_t, NULL, &waiter, (void *)&data);

	//join threads
	pthread_join(guest_t, NULL);
	//pthread_join(waiter_t, NULL);
}//main
