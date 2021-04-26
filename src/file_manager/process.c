// Import used global libraries
#include <stdio.h>  // FILE, fopen, fclose, etc.
#include <string.h> // strtok, strcpy, etc.
#include <stdlib.h> // malloc, calloc, free, etc.

// Import the header file of this module
#include "process.h"

void add_process(Queue* queue, Process* process){
    int length = queue->length;
    queue->process_queue[length] = process;
    queue->length++;
}

void print_queue(Queue* queue){
    int length = queue->length;
    printf("Queue length %d\n",length);
    for (int i=0; i<length; i++){
        printf("%s\n", queue->process_queue[i]->name);
    }
}