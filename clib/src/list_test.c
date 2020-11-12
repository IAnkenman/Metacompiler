#include <stdalign.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include "list.h"

#define PRINTSIZE(list) printf("List size is %lu\n", list_size(list))

static inline void print(void* obj) {
    printf("%i", *(int*)obj);
}

static void listAll(list_context* ct, size_t buffSize) {
    void* buff = malloc(buffSize);
    if (!list_size(ct))
        return;
    list_iterator mem = list_begin(ct);
    list_iterator* it = &mem;
    do {
        list_iterator_get(it, buff);
        print(buff);
        printf(" - ");
        list_iterator_next(it);
    } while (it->current != ct->current);
    free(buff);
    printf("\n");
    list_iterator_destory(it);
}

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
    
    printf("Testing %lu elements\n", arrSize);
    int tmp;
    
	list_context list = list_create(sizeof(int), 4096);
    
    if(list_at(&list, 10, &tmp) != -1) {
        printf("List found items that did not exist\n");
    }
    
    for (int i = 0; i < arrSize; i++) {
        tmp = i;
        list_push(&list, &tmp);
    }
    for (int i = 0; i < arrSize; i++) {
        list_at(&list, i, &tmp);
        if (tmp != i)
            printf("List corrupted item. Expected %i got %i\n", i, tmp);
    }
    
    for (int i = arrSize-1; i >= 0; i--) {
        list_pop(&list, &tmp);
        if (tmp != i)
            printf("List did not pop correctly expected %i got %i\n", i, tmp);
    }
    
    list_destroy(&list);
	return 0;
}
