/*
 * Conor Smyth 12452382
 * Adam O'Flynn 12378651
 * All work is our own
 */

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#define SLEEP 5
#define MAXROOM 8000

int seed = 1;

typedef struct {
	int generated;
	int pending;
	int called;
	int expired;
} logs_t;

typedef struct {
	int roomNumber;
	int callTime;
} wakeupCall_t;

static wakeupCall_t newCall() {
	wakeupCall_t c;
	c.roomNumber = randomRoom();
	c.callTime = 10;
	return c;
}

static int getRandomNumber(int x) {
	unsigned int *seedp = &seed;
	seed++;
	return rand_r(seedp) % x;
}

static int getRandomRoom() {
	return getRandomNumber(MAXROOM);
}

int randomRoom() {
	return getRandomRoom();
}

static void showCall(wakeupCall_t c) {
	printf("Room Number: %d, Call Time: %d\n", c.roomNumber, c.callTime);
}

static int getRandomSleep() {
	return getRandomNumber(SLEEP);
}

static void randomSleep() {
	sleep(getRandomSleep());
}

static void * generateCall(void *log_in) {
	logs_t *log = log_in;

	while(1) {
		randomSleep();

		wakeupCall_t call = newCall();

		showCall(call);
	}
}

int main() {
	logs_t log;

	pthread_t genCall_t;

	pthread_create(&genCall_t, NULL, &generateCall, (void *)&log);

	pthread_join(genCall_t, NULL);
}
