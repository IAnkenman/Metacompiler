#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include "varArr.h"

#define ARRSIZE 100

int main() {
    
    int PID;
    
    PID = fork();
    if (PID) {
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("Test passed\n");
        } else {
            printf("Test failed\n");
        }
        exit(0);
    }
    
    VarArr arr_mem = varArr_create(sizeof(int), ARRSIZE, 0);
    VarArr* arr = &arr_mem;
    
    if (varArr_size(arr) != ARRSIZE) {
        printf("arr did not set initial size properly\n");
    }
    
    int result;
    
    result = varArr_reserve(arr, ARRSIZE*10);
    
    if (result) {
        printf("arr failed to reserve space properly.");
    }
    
    for (size_t i = 0; i < ARRSIZE*100; i++) {
        int tmp = i;
        result = varArr_push(arr, &tmp);
        if (result) {
            printf("arr failed to push index %lu in\n", i);
        }
    }
}
