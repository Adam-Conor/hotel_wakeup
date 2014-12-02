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
#define ARRAY_SIZE 1000 //STILL NEED TO DECIDE THIS

/* define all structs */

typedef struct wakeupCall_t { //stuct for wake up time
	int roomNumber;
	time_t callTime;
} wakeupCall_t;

typedef struct Heap {
	struct wakeupCall_t times[ARRAY_SIZE];
	int numElements;
} Heap;

typedef struct { //struct for holding logs
	int generated;
	int pending;
	int called;
	int expired;
} logs_t;

typedef struct { //struct for holding shared data
	logs_t log;
	Heap *heap;
} sharedData_t;

/* Heap methods */

void addTime(Heap *heap, wakeupCall_t c) {
	heap->numElements++;
	heap->times[heap->numElements] = c;

	int current = heap->numElements;

	while(heap->times[current / 2].callTime > c.callTime) {
		heap->times[current] = heap->times[current / 2];
		current /= 2;
	}

	heap->times[current] = c;
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
	initNode(data->heap);
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

static void showCall(wakeupCall_t c) {
	//needs to add to data structure
	printf("Registering:\t%04d %s\n", c.roomNumber, ctime(&c.callTime));
}

/* log methods */

void logNew(logs_t *log) {
	log->generated++;
}

/* at the moment just generates random wake up calls at random times */

static void * generateCall(void *data_in) {
	sharedData_t *data = data_in;

	while(1) {
		//sleep for random seconds
		randomSleep();

		//generate a wake up call
		wakeupCall_t call = newCall();
		showCall(call);

		//add the call to the heap
		data->heap = addNode(data->heap, call);
		showHeap(data->heap);

		//log the new call
		logNew(&data->log);
		printf("%d\n", data->log.generated);
	}
}

static void * makeCall(void *data_in) {
	sharedData_t *data = data_in;
	sigset_t set;
	int sig;

	while(sig != SIGINT) {
		//if no data wait
		//get top of heap
		//remove top of heap
		//if data available sleep until time
		//make call(message)
		//log call made
	}
}

int main() {
	//delcare shared data
	sharedData_t data;

	//initialise shared data
	initData(&data);

	//initialise threads
	pthread_t genCall_t;
	//pthread makeCall_t;

	//create threads
	pthread_create(&genCall_t, NULL, &generateCall, (void *)&data);
	//pthread_create(&makeCall_,t NULL, &makeCall, (void *)&data);

	//join threads
	pthread_join(genCall_t, NULL);
	//pthread_join(makeCall_t, NULL);
}//main
