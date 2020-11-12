#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stringIntern.h"

char* stringMap[20] = {
	"Test 0",
	"Test 1",
	"Test 2",
	"Test 3",
	"Test 4",
	"Test 5",
	"Test 6",
	"Test 7",
	"Test 8",
	"Test 9",
	"Test 0",
	"Test 1",
	"Test 2",
	"Test 3",
	"Test 4",
	"Test 5",
	"Test 6",
	"Test 7",
	"Test 8",
	"Test 9",
};

int main() {
	int PID = 0;
	PID = fork();
	if(PID) {
		int status;
		wait(&status);
		if(WIFEXITED(status)) {
			printf("Test completed\n");
		} else {
			printf("Test failed with a status of %i\n", status);
		}
		exit(0);
	}
	
	StringStore pool = intern_create();
	const char* mappings[10];
	
	printf("Testing Internment\n");
	for (size_t i = 0; i < 10; i++) {
		mappings[i] = intern_intern(&pool, stringMap[i], MAX_INTERN_LENGTH);
	}
	
	printf("Testing if the pool successfully removes duplicates\n");
	for (size_t i = 10; i < 20; i++) {
		if (mappings[i - 10] != intern_intern(&pool, stringMap[i], MAX_INTERN_LENGTH)) {
			printf("Pool failed to remove duplicate string\n");
		} 
	}
	
	printf("Testing validity of mappings\n");
	for (size_t i = 0; i < 10; i++) {
		printf("%s\n", mappings[i]);
	}
}
