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
  int ready_time;
  int first_ready;
  int wait;
  int wait_left;
  int waiting_delay;
  int waiting_delay_left;
  int chosen;
  int interrupted;
  int first_time;
  int end_time;
  int first_wait;
  int total_time_waiting;
  int total_time_exe;
} Process;

typedef struct _fib_node {
  float key;
  int degree;
  struct _fib_node *left_sibling;
  struct _fib_node *right_sibling;
  struct _fib_node *parent;
  struct _fib_node *child;
  bool mark;
  bool visited;
  Process* process;
} fib_node;

typedef struct fibanocci_heap {
  int n;
  fib_node *min;
  int phi;
  int degree;
} FIB_HEAP;

// Structs obtenidos de: https://www.programiz.com/dsa/fibonacci-heap
typedef struct node {
    struct node* next; // Pointer to next node in DLL
    struct node* prev; // Pointer to previous node in DLL
    Process *process;
    int value;
} Node;

typedef struct queue {
  int n;
  Node *head;
  // int phi;
  // int degree;
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
FIB_HEAP *make_fib_heap();
void print_heap(fib_node *n);
void insertion(FIB_HEAP *H, fib_node *new, float val);
void fib_heap_link(FIB_HEAP *H, fib_node *y, fib_node *x);
int cal_degree(int n);
void consolidate(FIB_HEAP *H);
fib_node *extract_min(FIB_HEAP *H);
//void push(Node* head, Process* process);
Node* append(Queue* queue, Process* process);
void printList(Node* node);
void deleteHead(Queue* queue);
Node* deleteNode(Node* node, Queue* queue);
void sortQueue(Queue* queue);
