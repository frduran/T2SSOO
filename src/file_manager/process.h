// Tells the compiler to compile this file once
#pragma once
#include <stdbool.h>

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
  int end_time;
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

//Structs obtenidos de: https://www.programiz.com/dsa/fibonacci-heap
typedef struct _NODE {
  int key;
  int degree;
  struct _NODE *left_sibling;
  struct _NODE *right_sibling;
  struct _NODE *parent;
  struct _NODE *child;
  bool mark;
  bool visited;
  //
  Process *process;
} NODE;

typedef struct queue {
  int n;
  NODE *min;
  int phi;
  int degree;
  //
  int Q;
  int priority;
  int p;
  int q;
  int quantum;
  int length;
} Queue;

// Declare functions
//Process* initialize(int pid, char name, int priority, char status);
// void add_process(Queue* queue, Process* process);
// void print_queue(Queue* queue);
Queue *make_queue();
void print_queue(NODE *n);
void insertion(Queue *H, NODE *new, int val);
void queue_link(Queue *H, NODE *y, NODE *x);
void consolidate(Queue *H);
NODE *extract_min(Queue *H);