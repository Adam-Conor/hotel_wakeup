#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SLEEP 10
#define MAXROOM 1000

int main() {
	while(1) {
		int roomNumber = randomRoom();
		//Registering: RoomNumber Day Month Date Time Year
		printf("Registering: %d\n", roomNumber);
	}
}
