// Tells the compiler to compile this file once
#pragma once

// Define the struct
typedef struct process {
  int pid;
  char* name;
  int priority;
  char* status;
  int cycles;
  int start_time;
  int wait;
  int waiting_delay;
  int chosen;
  int interrupted;
  int first_time;
} Process;

typedef struct queue {
  int Q;
  int priority;
  int p;
  int q;
  int quantum;
  int length;
  Process** process_queue;
} Queue;

// Declare functions
//Process* initialize(int pid, char name, int priority, char status);
void add_process(Queue* queue, Process* process);
void print_queue(Queue* queue);