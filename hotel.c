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
#define MAXROOM 5000

typedef struct {
	int generated;
	int pending;
	int called;
	int expired;
} logs_t;

typedef struct {
	int roomNumber;
	int callTime;
} room_t;

static room_t newRoom() {
	room_t r;
	r.roomNumber = randomRoom();
	r.callTime = 10;
	return r;
}

int randomRoom() {
	//srand(time(NULL));

	return rand_r() % MAXROOM;
}

static void showRoom(room_t r) {
	printf("Room Number: %d, Call Time: %d\n", r.roomNumber, r.callTime);
}

static int getRandomSleep() {
	//srand(time(NULL));

	return rand_r() % SLEEP;
}

static void randomSleep() {
	int randomNumber = getRandomSleep();

	printf("%d\n", randomNumber);

	sleep(randomNumber);
}

static void * generateCall(void *log_in) {
	//srand(time(NULL));
	logs_t *log = log_in;
	while(1) {
		randomSleep();

		room_t room = newRoom();

		showRoom(room);
	}
}

int main() {
	logs_t log;
	srand(time(NULL));

	pthread_t genCall_t;

	pthread_create(&genCall_t, NULL, &generateCall, (void *)&log);

	pthread_join(genCall_t, NULL);

	//int roomNumber = randomRoom();
	//Registering: RoomNumber Day Month Date Time Year
	//printf("Registering: %d\n", roomNumber);
}
