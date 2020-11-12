#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ptrmap.h"

static size_t hash (void* key)
{
	const char* str = key;
	//djb2 hash algorithm
	size_t hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* same as hash = hash * 33 + c */
	return hash;
}

static int cmp(void* a, void* b)
{
	return strcmp(a, b);
}

size_t ptrMap[20][2] =
{
	{1, 100},
	{2, 200},
	{3, 300},
	{4, 400},
	{5, 500},
	{6, 600},
	{7, 700},
	{8, 800},
	{9, 900},
	{10, 1000},
	{11, 1100},
	{12, 1200},
	{13, 1300},
	{14, 1400},
	{15, 1500},
	{16, 1600},
	{17, 1700},
	{18, 1800},
	{19, 1900},
	{20, 1200},
};

int main () {
	int PID = 0;
	PID = fork();
	if(PID) {
		int status;
		wait(&status);
		if(WIFEXITED(status)) {
			printf("Test passed without crashing\n");
		} else {
			printf("Test failed with a status of %i\n", status);
		}
		exit(0);
	} 
	
	PtrMap map = ptrmap_create(&hash, &cmp);
	
	printf("Testing insertion\n");
	for (size_t i = 0; i < 10; i++) {
		ptrmap_insert(&map, (void*)ptrMap[i][0], (void*)ptrMap[i][1]);
	}
	
	printf("Testing lookup\n");
	for (size_t i = 0; i < 10; i++) {
		void* str = ptrmap_get(&map, (void*)ptrMap[i][0]);
		if (!str) {
			printf("Map failed, returned NULL\n");
		}
		else if (!(str == (void*)ptrMap[i][1])) {
			printf("Ptr returned returned did not match\n");
		}
	}
	
	printf("Testing insertion\n");
	for (size_t i = 10; i < 20; i++) {
		ptrmap_insert(&map, (void*)ptrMap[i][0], (void*)ptrMap[i][1]);
	}
	
	printf("Testing lookup\n");
	for (size_t i = 0; i < 20; i++) {
		const char* str = ptrmap_get(&map, (void*)ptrMap[i][0]);
		if (!str) {
			printf("Map failed, returned NULL\n");
		}
		else if (!(str == (void*)ptrMap[i][1])) {
			printf("Ptr returned did not match\n");
		}
	}
	
	printf("Testing removal\n");
	for (size_t i = 0; i < 10; i++) {
		void* str = ptrmap_remove(&map, (void*)ptrMap[i][0]);
		if (!str) {
			printf("Map failed, returned NULL\n");
		}
		else if (!(str == (void*)ptrMap[i][1])) {
			printf("Ptr returned did not match\n");
		}
	}
	
	for (size_t i = 0; i < 10; i++) {
		int result = ptrmap_has(&map, (void*)ptrMap[i][0]);
		if (result) {
			printf("Delete failed, mapping still exists\n");
		}
	}
	
	printf("Testing persistance\n");
	for (size_t i = 10; i < 20; i++) {
		int result = ptrmap_has(&map, (void*)ptrMap[i][0]);
		if (!result) {
			printf("Persistance failed, mapping no longer exists after delete\n");
		}
	}
	
	if (ptrmap_has(&map, (void*)1000000)) {
		printf("Hashmap returned true for mapping that was never entered\n");
	}
	return 0;
}
