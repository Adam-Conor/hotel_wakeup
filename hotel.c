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

/* define all structs */

typedef struct { //stuct for wake up time
	int roomNumber;
	time_t callTime;
} wakeupCall_t;

typedef struct Node { //stuct for nodes in heap
	wakeupCall_t time;
	struct Node *next;
} Node;

typedef struct { //stuct for holding logs
	int generated;
	int pending;
	int called;
	int expired;
} logs_t;

typedef struct { //struct for holding shared data
	logs_t log;
	Node *heap;
} sharedData_t;

/* Node methods */

Node *addNode(Node *root, wakeupCall_t c) {
	Node *newCall = (Node *) malloc(sizeof(Node));
	newCall->time = c;
	//newCall->next = NULL;

	if(root == NULL) {
		newCall->next = NULL;
		return newCall;
	}//if the heap empty

	if(newCall->time.callTime < root->time.callTime) {
		newCall->next = root;
		return newCall;
	}

	Node *prev = root;
	Node *current = root->next;

	while(current->next != NULL && newCall->time.callTime > current->time.callTime) {
		prev = current;
		current = current->next;
	}
	
	newCall->next = current;
	prev->next = newCall;

	return root;
}//add a node to heap

Node *removeNode(Node *root) {
	Node *next = root->next;

	free(root);

	return next;
}//remove first node from heap

void showHeap(Node *root) {
	Node *temp = root;

	while(temp != NULL) {
		printf("%04d %s\n", temp->time.roomNumber, ctime(&temp->time.callTime));
		temp = temp->next;
	}
}

/* Initialisation methods */

void initLog(logs_t *log) {
	log->generated = 0;
	log->pending = 0;
	log->called = 0;
	log->expired = 0;
}

void initNode(Node *ned) {
	ned = (Node *) malloc(sizeof(Node));
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

int main() {
	//delcare shared data
	sharedData_t data;

	//initialise shared data
	initData(&data);

	//initialise threads
	pthread_t genCall_t;

	//create threads
	pthread_create(&genCall_t, NULL, &generateCall, (void *)&data);

	//join threads
	pthread_join(genCall_t, NULL);
}//main
