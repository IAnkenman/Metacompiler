#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include "vararr.h"

#define ARRSIZE 100

int main() {
    
    int PID;
    
    PID = fork();
    if (PID) {
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("Test completed\n");
        } else {
            printf("Test crashed with status %i\n", status);
        }
        exit(0);
    }
    
    VarArr arr_mem = vararr_create();
    VarArr* arr = &arr_mem;
    
    vararr_reserve(arr, ARRSIZE*10);
    
    for (size_t i = 0; i < ARRSIZE*100; i++) {
        vararr_push(arr, (void*)i);
        if ((size_t)vararr_top(arr) != i) {
            printf("arr failed to push index %lu in\n", i);
        }
    }
}
