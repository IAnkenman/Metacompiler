#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stringmap.h"

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

const char* stringMap[20][2] =
{
	{"Mapping0", "Mapped0"},
	{"Mapping1", "Mapped1"},
	{"Mapping2", "Mapped2"},
	{"Mapping3", "Mapped3"},
	{"Mapping4", "Mapped4"},
	{"Mapping5", "Mapped5"},
	{"Mapping6", "Mapped6"},
	{"Mapping7", "Mapped7"},
	{"Mapping8", "Mapped8"},
	{"Mapping9", "Mapped9"},
	{"Mapping10", "Mapped10"},
	{"Mapping11", "Mapped11"},
	{"Mapping12", "Mapped12"},
	{"Mapping13", "Mapped13"},
	{"Mapping14", "Mapped14"},
	{"Mapping15", "Mapped15"},
	{"Mapping16", "Mapped16"},
	{"Mapping17", "Mapped17"},
	{"Mapping18", "Mapped18"},
	{"Mapping19", "Mapped19"},
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
	
	StringMap map = stringmap_create(&hash, &cmp);
	
	printf("Testing insertion\n");
	for (size_t i = 0; i < 10; i++) {
		stringmap_insert(&map, (void*)stringMap[i][0], (void*)stringMap[i][1]);
	}
	
	printf("Testing lookup\n");
	for (size_t i = 0; i < 10; i++) {
		const char* str = stringmap_get(&map, (void*)stringMap[i][0]);
		if (!str) {
			printf("Map failed, returned NULL\n");
		}
		else if (!strcmp(str, stringMap[i][1])) {
			printf("String returned did not match\n");
		}
	}
	
	printf("Testing rehashing\n");
	for (size_t i = 10; i < 20; i++) {
		stringmap_insert(&map, (void*)stringMap[i][0], (void*)stringMap[i][1]);
	}
	
	printf("Testing lookup\n");
	for (size_t i = 0; i < 20; i++) {
		const char* str = stringmap_get(&map, (void*)stringMap[i][0]);
		if (!str) {
			printf("Map failed, returned NULL\n");
		}
		else if (!strcmp(str, stringMap[i][1])) {
			printf("String returned did not match\n");
		}
	}
	
	printf("Testing removal\n");
	for (size_t i = 0; i < 10; i++) {
		const char* str = stringmap_remove(&map, (void*)stringMap[i][0]);
		if (!str) {
			printf("Map failed, returned NULL\n");
		}
		else if (!strcmp(str, stringMap[i][1])) {
			printf("String returned did not match\n");
		}
	}
	
	for (size_t i = 0; i < 10; i++) {
		int result = stringmap_has(&map, (void*)stringMap[i][0]);
		if (result) {
			printf("Delete failed, mapping still exists\n");
		}
	}
	
	printf("Testing persistance\n");
	for (size_t i = 0; i < 10; i++) {
		int result = stringmap_has(&map, (void*)stringMap[i][0]);
		if (!result) {
			printf("Persistance failed, mapping no longer exists after delete\n");
		}
	}
	
	if (stringmap_has(&map, "Random Querry")) {
		printf("Hashmap returned true for mapping that was never entered\n");
	}
	return 0;
}
