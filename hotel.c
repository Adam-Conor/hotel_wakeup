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

#define SLEEP 5
#define MAXROOM 8000

typedef struct { //stuct for wake up time
	int roomNumber;
	time_t callTime;
} wakeupCall_t;

typedef struct { //stuct for nodes in heap
	wakeupCall_t time;
	struct Node *next;
} Node;

typedef struct { //stuct for holding logs
	int generated;
	int pending;
	int called;
	int expired;
} logs_t;

void initLog(logs_t *log) {
	log->generated = 0;
	log->pending = 0;
	log->called = 0;
	log->expired = 0;
}//initialise logs

/*Node *add(Node *root, wakeupCall_t c) {
	Node *newCall = (Node *) malloc(sizeof(Node));
	newCall = root;
	newCall->time = c;

	if(root == NULL) {
		newCall->next = NULL;
		return newCall;
	}//if the heap empty

	if(c.callTime < root->time.callTime) {
		Node *diff  = (Node *) malloc(sizeof(Node));
		diff = root;
		newCall->next = diff;
		return newCall;
	}//if 

	root = add(root->next, c);
}//add a node to heap
*/

static wakeupCall_t newCall() {
	wakeupCall_t c;
	c.roomNumber = randomRoom();
	c.callTime = randomCall();
	return c;
}//generate a new wake up call

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

static void showCall(wakeupCall_t c) {
	//needs to add to data structure
	printf("Registering:\t%04d %s\n", c.roomNumber, ctime(&c.callTime));
}

static int getRandomSleep() {
	return getRandomNumber(SLEEP);
}//returns random sleep time

static void randomSleep() {
	sleep(getRandomSleep());
}//sleep for a random time

/*
 * at the moment just generates random wake up calls at random times
 */

static void * generateCall(void *log_in) {
	logs_t *log = log_in;

	while(1) {
		randomSleep();

		wakeupCall_t call = newCall();

		showCall(call);
		log->generated++;
		printf("%d\n", log->generated);
	}
}

int main() {
	logs_t log;

	initLog(&log);

	pthread_t genCall_t;

	pthread_create(&genCall_t, NULL, &generateCall, (void *)&log);

	pthread_join(genCall_t, NULL);
}
