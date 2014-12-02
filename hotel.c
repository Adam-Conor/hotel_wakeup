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
#define LATEST_TIME 100
#define MAXROOM 8000
#define ARRAY_SIZE 1000

/* Define structs */

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
	pthread_mutex_t m;
	pthread_cond_t cv;
} sharedData_t;

/* Heap methods */

/*struct wakeupCall_t resizeArray(Heap *heap) {
	struct wakeupCall_t temp[ARRAY_SIZE * 2];
	int i;
	for(i = 0; i < ARRAY_SIZE; i++) {
		temp[i] = heap->times[i];
	}

	//heap->times = ptr;
	return temp;
}
*/

void addTime(Heap *heap, wakeupCall_t c) { //add element to heap
	heap->numElements++;
	
	/* resize array if too big */
	/*if(heap->numElements > ARRAY_SIZE) {
		resizeArray(heap);
	}
	*/

	heap->times[heap->numElements] = c;

	int current = heap->numElements;

	while(heap->times[current / 2].callTime > c.callTime) {
		heap->times[current] = heap->times[current / 2];
		current /= 2;
	}

	heap->times[current] = c;
}

wakeupCall_t removeFirst(Heap *heap) {
	wakeupCall_t first;
	wakeupCall_t last;
	int child;
	int current;

	first = heap->times[1];
	last = heap->times[heap->numElements--];

	for(current = 1; (current * 2) <= heap->numElements; current = child) {
		child = current * 2;

		if(child != heap->numElements && (heap->times[child + 1].callTime < heap->times[child].callTime)) {
			child++;
		}

		if(last.callTime > heap->times[child].callTime) {
			heap->times[current].callTime = heap->times[child].callTime;
		} else {
			break;
		}
	}

	heap->times[current].callTime = last.callTime;
	return first;
}

void showHeap(Heap james) {
	int i;
	for(i = 1; i < james.numElements; i++) {
		printf("%s\n", ctime(&james.times[i].callTime));
	}
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

void initMutex(pthread_mutex_t *tom) {
	pthread_mutex_init(tom, NULL);
}

void initCV(pthread_cond_t *cv) {
	cv = PTHREAD_COND_INITIALIZER;
}

void initData(sharedData_t *data) {
	initLog(&data->log);
	initMutex(&data->m);
	initCV(&data->cv);
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
	return time(NULL) + getRandomNumber(LATEST_TIME);
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
}//generate a random call

static void showCall(wakeupCall_t c) {
	printf("Registering:\t%4d %s\n", c.roomNumber, ctime(&c.callTime));
}//show a call

static void showWakeUp(wakeupCall_t c) {
	printf("Wake Up:\t%4d %s\n", c.roomNumber, ctime(&c.callTime));
}//show a wake up

/* log methods */

void logNew(logs_t *log) {
	log->generated++;
	log->pending++;
}//Log new call

/* at the moment just generates random wake up calls at random times */
static void * guest(void *data_in) {
	sharedData_t *data = data_in;
	int sig;

	while(sig != SIGINT) {
		/* Sleep for random time */
		randomSleep();

		/* Generate and show wake up call */
		wakeupCall_t call = newCall();
		showCall(call);

		/* add the call to the heap */
		addTime(&data->heap, call);

		/* Log new call */
		logNew(&data->log);
	}
}

static void * waiter(void *data_in) {
	sharedData_t *data = data_in;
	sigset_t set;
	int sig;

	while(sig != SIGINT) {
		while(1) {
			pthread_cond_wait
		}
		//if no data wait
		//get top of heap
		//remove top of heap
		//if data available sleep until time
		//make call(message)
		//log call made
	}
}

static void handler(int signo) {
	printf("SIGINT recieved. Cancelling threads");
}

int main() {
	/* Declare shared data */
	sharedData_t data;

	/* Initialise shared data */
	initData(&data);

	/* Initialise threads */
	pthread_t guest_t;
	//pthread waiter_t;

	/* Create threads */
	pthread_create(&guest_t, NULL, &guest, (void *)&data);
	//pthread_create(&waiter_t, NULL, &waiter, (void *)&data);
	
	/* Set up set information */
	sigset_t set;
	sigemptyset(&set);
	//sigset(SIGINT, handler);
	int sig;

	//sigwait(&set, &sig);
	
	//pthread_kill(guest_t, SIGINT);

	/* Join Threads */
	pthread_join(guest_t, NULL);
	//pthread_join(waiter_t, NULL);
}//main
