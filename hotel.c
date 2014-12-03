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
	int pending;
	int expired;
} logs_t;

typedef struct { //struct for holding shared data
	logs_t log;
	Heap heap;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} sharedData_t;

/* Heap methods */

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
	log->pending = 0;
	log->expired = 0;
}//initialise the logs

void initHeap(Heap *harry) {
	harry->numElements = 0;
}//initialise the heap

void initMutex(pthread_mutex_t *tom) {
	pthread_mutex_init(tom, NULL);
}//initialise the mutex

void initCond(pthread_cond_t *cond) {
	pthread_cond_init(cond, NULL);
}//initialise the cond variable

void initData(sharedData_t *data) {
	initLog(&data->log);
	initMutex(&data->mutex);
	initCond(&data->cond);
	initHeap(&data->heap);
}//initialise the shared data

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

/* Print methods */

static void showCall(wakeupCall_t c) {
	printf("Registering:\t%4d %s\n", c.roomNumber, ctime(&c.callTime));
}//show registered call

static void showWakeUp(wakeupCall_t c) {
	printf("Wake Up:\t%4d %s\n", c.roomNumber, ctime(&c.callTime));
}//show wake up call

static void showLog(logs_t *log) {
	printf("Expired Alarms: %d\nPending Alarms: %d\n", log->expired, log->pending);
}//show the log

/* log methods */

void logNew(logs_t *log) {
	log->pending++;
} //log a pending call

void logExpired(logs_t *log) {
	log->expired++;
	log->pending--;
} //log an expired call

/* Guest generates random wake up times */

static void * guest(void *data_in) {
	/* Take in shared data */
	sharedData_t *data = data_in;

	/* Set up signal information */
	int sig;

	while(sig != SIGINT) {
		/* Sleep for random time */
		randomSleep();

		/* Generate and show wake up call */
		wakeupCall_t call = newCall();
		showCall(call);

		/* */
		pthread_mutex_lock(&data->mutex);

		/* add the call to the heap */
		addTime(&data->heap, call);

		/* Log new call */
		logNew(&data->log);

		pthread_mutex_unlock(&data->mutex);

		while(time(NULL) >= data->heap.times[1].callTime) {
			pthread_cond_signal(&data->cond);
			pthread_cond_wait(&data->cond, &data->mutex);
		}
	}
}

//int pthread_mutex_init(pthread_mutex_t *mutex,const pthread_mutexattr_t *attr);
//int pthread_mutex_lock(pthread_mutex_t *mutex);
//int pthread_mutex_unlock(pthread_mutex_t *mutex);
//int pthread_mutex_destroy(pthread_mutex_t *mutex);

//void *foo(void *p) {
//pthread_mutex_lock(&mutex);
///* Update p */
//pthread_mutex_unlock(&mutex);
//return (NULL);
//}
//
//int pthread_cond_init(pthread_cond_t *cond,
//const pthread_condattr_t *attr);
//int pthread_cond_destroy(pthread_cond_t *cond);
//int pthread_cond_wait(pthread_cond_t *cond,pthread_mutex_t *mutex);
//int pthread_cond_timedwait(pthread_cond_t *cond,pthread_mutex_t *mutex,
//const struct timespec *abstime);
//int pthread_cond_broadcast(pthread_cond_t *cond);
//int pthread_cond_signal(pthread_cond_t *cond);

static void * waiter(void *data_in) {
	/* Take in shared data */
	sharedData_t *data = data_in;

	/* Set up signal information */
	sigset_t set;
	int sig;

	while(sig != SIGINT) {
		/* Wait for time */
		while(time(NULL) < data->heap.times[1].callTime && time(NULL) > 0) {
			pthread_cond_wait(&data->cond, &data->mutex);
		}
		
		/* Protect shared data */
		pthread_mutex_lock(&data->mutex);
		
		/* Show wake up call */
		showWakeUp(data->heap.times[1]);

		/* Remove from heap */
		//removeFirst(&data->heap.times);

		/* Log expired call */
		logExpired(&data->log);
		
		/* Show pending and expired alarms */
		showLog(&data->log);

		/* Signal guest thread */
		pthread_cond_signal(&data->cond);

		/* Release shared data */
		pthread_mutex_unlock(&data->mutex);
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
	pthread_t waiter_t;

	/* Create threads */
	pthread_create(&guest_t, NULL, &guest, (void *)&data);
	pthread_create(&waiter_t, NULL, &waiter, (void *)&data);
	
	/* Set up signal information */
	sigset_t set;
	sigemptyset(&set);
	//sigset(SIGINT, handler);
	int sig;

	//sigwait(&set, &sig);
	
	//pthread_kill(guest_t, SIGINT);

	/* Wait for threads to finish */
	pthread_join(guest_t, NULL);
	//pthread_join(waiter_t, NULL);
}//main
