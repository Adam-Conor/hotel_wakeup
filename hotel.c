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
#include <time.h>

/* Declare constants */

#define SLEEP 5
#define LATEST_TIME 100
#define MAXROOM 8000
#define HEAP_SIZE 5
#define ETIMEDOUT 110

/* Define structs */

typedef struct wakeupCall_t { //stuct for wake up time
	int roomNumber;
	time_t callTime;
} wakeupCall_t;

typedef struct Heap { //struct for the heap
	struct wakeupCall_t *times;
	int size;
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

void resizeHeap(Heap *heap) {
	heap->size = heap->numElements * 2;

	/* Allocate an array double the size */
	heap->times = realloc(heap->times, heap->size * sizeof(wakeupCall_t));
}//double the size of the heap

void addTime(Heap *heap, wakeupCall_t c) {
	/* Increment number of elements */
	heap->numElements++;

	/* resize array if too big */
	if(heap->numElements >= heap->size) {
		resizeHeap(heap);
	}

	/* Add element to end of heap */
	heap->times[heap->numElements] = c;

	/* Fix up heap */
	int current = heap->numElements;
			
	while(heap->times[current / 2].callTime > c.callTime) {
		heap->times[current] = heap->times[current / 2];
		current /= 2;
	}

	heap->times[current] = c;
}//add an element to the heap

void fixHeap(Heap *heap, int root) {
	int childNode;
	wakeupCall_t temp = heap->times[root];

	for(; root * 2 <= heap->numElements ; root = childNode) {
		childNode = root * 2;

		if(childNode != heap->numElements && 
			heap->times[childNode + 1].callTime < heap->times[childNode].callTime) {
			childNode++;
		}

		if(heap->times[childNode].callTime < temp.callTime) {
			heap->times[root] = heap->times[childNode];
		} else {
			break;
		}
	}

	heap->times[root] = temp;
}//fix the heap after removing

void removePriorty(Heap *heap) {
	heap->times[1] = heap->times[heap->numElements--];
	fixHeap(heap, 1);
}//remove the first item

/* Initialisation methods */

void initLog(logs_t *log) {
	log->pending = 0;
	log->expired = 0;
}//initialise the logs

void initHeap(Heap *harry) {
	harry->size = HEAP_SIZE;
	harry->times = malloc(harry->size * sizeof(struct wakeupCall_t));
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
	printf("Expired Alarms: %d\nPending Alarms: %d\n\n", 
			log->expired, log->pending);
}//show the log
	
/* log methods */

void logNew(logs_t *log) {
	log->pending++;
}//log a pending call

void logExpired(logs_t *log) {
	log->expired++;
	log->pending--;
}//log an expired call

static void cleanupLog(logs_t *log) {
	log->expired = 0;
	log->pending = 0;
}//set pending back to 0

/* Thread cleanup methods */

static void * guest_cleanup(void *data_in) {
	sharedData_t *data = data_in;

	/* Protect the data */
	pthread_mutex_lock(&data->mutex);

	/* Wait for waiter thread to cancel */
	while(data->log.pending > 0) {
		pthread_cond_wait(&data->cond, &data->mutex);
	}

	printf("The guest thread is cleaning up...\n");

	/* Unlock the mutex */
	pthread_mutex_unlock(&data->mutex);

	printf("The guest thread says goodbye.\n");
}//cleanup guest thread

static void * waiter_cleanup(void *data_in) {
	printf("\nThe waiter thread is cleaning up...\n");
	sharedData_t *data = data_in;

	/* Clean up log */
	cleanupLog(&data->log);

	/* Free memory */
	free(data->heap.times);

	/* Signal guest to end */
	if(data->log.pending == 0) {
		pthread_cond_signal(&data->cond);
	}

	/* Unlock the mutex */
	pthread_mutex_unlock(&data->mutex);

	printf("The waiter thread says goodbye.\n");
}//cleanup waiter thread

/* Guest generates random wake up times */
static void * guest(void *data_in) {
	/* Take in shared data */
	sharedData_t *data = data_in;

	/* Install a cleanup handler */
	pthread_cleanup_push(guest_cleanup, data);

	while(1) {
		/* Protect data */
		pthread_mutex_lock(&data->mutex);

		/* Generate a call */
		wakeupCall_t call = newCall();

		/* Show call being generated */
		showCall(call);

		/* add the call to the heap */
		addTime(&data->heap, call);

		/* Log new call */
		logNew(&data->log);

		/* Signal waiter thread */
		if(call.callTime == data->heap.times[1].callTime) {
			pthread_cond_signal(&data->cond);
		}

		/* Release shared data */
		pthread_mutex_unlock(&data->mutex);

		/* Sleep for random seconds */
		randomSleep();
	}

	/* Cleanup and exit thread */
	pthread_cleanup_pop(1);
	pthread_exit(NULL);
}

/* Makes wakeup call at time */
static void * waiter(void *data_in) {
	/* Take in shared data */
	sharedData_t *data = data_in;

	/* Install cleanup handler */
	pthread_cleanup_push(waiter_cleanup, data);

	/* Wait for time */
	while(1) {
		/* Protect shared data */
		pthread_mutex_lock(&data->mutex);

		while(&data->heap.times[1] == NULL) {
			pthread_cond_wait(&data->cond, &data->mutex);
		}

		/* Set up wait */
		struct timespec ts; 

		ts.tv_sec = data->heap.times[1].callTime;
		ts.tv_nsec = 0;

		int timeOut = pthread_cond_timedwait(&data->cond, &data->mutex, &ts);

		if(timeOut == ETIMEDOUT) {
			/* Show wake up call */
			showWakeUp(data->heap.times[1]);

			/* Remove from heap */
			removePriorty(&data->heap);

			/* Log expired call */
			logExpired(&data->log);

			/* Show pending and expired alarms */
			showLog(&data->log);
		}

		/* Release shared data */
		pthread_mutex_unlock(&data->mutex);

	}

	/* Cleanup and exit thread */
	pthread_cleanup_pop(1);
	pthread_exit(NULL);
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
	sigaddset(&set, SIGINT);
	sigprocmask(SIG_BLOCK, &set, NULL);
	int sig;

	/* Wait for SIGINT */
	sigwait(&set, &sig);

	/* Cancel the threads */
	pthread_cancel(guest_t);
	pthread_cancel(waiter_t);

	/* Wait for threads to finish */
	pthread_join(guest_t, NULL);
	pthread_join(waiter_t, NULL);

	/* Cleanup mutex and condition varibale */
	pthread_mutex_destroy(&data.mutex);
	pthread_cond_destroy(&data.cond);

	/* Show pending alarms */
	printf("Pending: %d\n", data.log.pending);

	return 0;
}//main
