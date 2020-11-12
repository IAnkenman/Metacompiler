#include <stdalign.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include "pool.h"

struct obj {
	size_t a;
	size_t b;
	size_t c;
	size_t d;
};

int main () {
	size_t arrSizes[3] = {100, 1000, 10000};
    size_t arrSize;
    int PID = 0;
    for (size_t i = 0; i < 3; i++) {
        arrSize = arrSizes[i];
        PID = fork();
        if(PID) {
            int status;
            wait(&status);
            if(WIFEXITED(status)) {
                printf("Test passed with %lu elements\n", arrSizes[i]);
            } else {
                printf("Test failed with %lu elements\n", arrSizes[i]);
            }
        } else {
            break;
        }
    }
    if (PID) {
        exit(0);
    }
    
	pool_context pool = pool_create(sizeof(struct obj), 4096);
	struct obj* mem = pool_alloc(&pool);

	

	mem->a = 1;
	mem->b = 2;
	mem->c = 3;
	mem->d = 4;

	pool_free(&pool, mem);

	void* arr[arrSize];

	for (size_t i = 0; i < arrSize; i++) {
		arr[i] = pool_alloc(&pool);
        if (!arr[i])
            printf("Pool failed to allocate\n");
        *(char*)arr[i] = 'A';
	}

	for (size_t i = 0; i < arrSize/2; i++) {
		pool_free(&pool, arr[i]);
	}

	for (size_t i = 0; i < arrSize/2; i++) {
		arr[i] = pool_alloc(&pool);
        if (!arr[i])
            printf("Pool failed to allocate\n");
        *(char*)arr[i] = 'A';
	}

	pool_destroy(&pool);
    
    void* test = pool_alloc(&pool);
    
    if(test) {
        printf("Pool allocated after destruction\n");
    }

	return 0;
}
