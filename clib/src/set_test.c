#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <stdalign.h>
#include "set.h"

#define ARRSIZE 20

int main() {
    //int PID;
    
    //PID = fork();
    
    //if (PID) {
    //    int status;
    //    wait(&status);
    //    if (WIFEXITED(status)) {
    //       printf("Test passed\n");
    //    } else {
    //        printf("Test failed\n");
    //    }
    //    exit(0);
    //}
    
    set_context mem = set_create();
    set_context* set = &mem;
    
    size_t arr[ARRSIZE];
    
    srand(time(NULL));
    
    for (int i = 0; i < ARRSIZE; i++) {
        size_t tmp;
        tmp = rand();
        
        arr[i] = tmp;
		printf("Adding element %lu to set\n", tmp);
        set_add(set, tmp);
    }
    
    for (int i = 0; i < ARRSIZE; i++) {
		printf("Searching for element\n");
        if (!set_has(set, arr[i])) {
            printf("Set failed to record element\n");
        }
    }
    
    size_t setSize = set_size(set);
    
    for (int i = 0; i < ARRSIZE; i++) {
        set_add(set, arr[i]);
    }
    
    if (setSize != set_size(set)) {
        printf("Set added duplicate element\n");
    }
    
    set_remove(set, arr[0]);
    
    if (set_has(set, arr[0])) {
        printf("Set failed to remove elements\n");
    }
    
    if (setSize-1 != set_size(set)) {
        printf("Set failed to decrement counter\n");
    }
    
    set_clear(set);
    
    if (set_size(set)) {
        printf("Set failed to reset counter after clear\n");
    }
    
    for (int i = 0; i < ARRSIZE; i++) {
        if(set_has(set, arr[i])) {
            printf("Set has item after clear\n");
        }
    }
    
    set_destroy(set);
    
    if (set_size(set)) {
        printf("Set failed to reset counter after destroy\n");
    }
    
    set_add(set, arr[0]);
    
    if (set_size(set)) {
        printf("Set incremented after destroy\n");
    }
    
    for (int i = 0; i < ARRSIZE; i++) {
        if (set_has(set, arr[i])) {
            printf("Set has items after destroy\n");
        }
    }
}
