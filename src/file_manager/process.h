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
  int first_wait;
  int total_time_waiting;
} Process;

// typedef struct queue {
//   int Q;
//   int priority;
//   int p;
//   int q;
//   int quantum;
//   int length;
//   Process** process_queue;
// } Queue;

// Structs obtenidos de: https://www.programiz.com/dsa/fibonacci-heap
typedef struct node {
    struct node* next; // Pointer to next node in DLL
    struct node* prev; // Pointer to previous node in DLL
    Process *process;
    int value;
} Node;

// typedef struct _NODE {
//   int key;
//   int degree;
//   struct _NODE *previous;
//   struct _NODE *next;
//   bool mark;
//   bool visited;
//   Process *process;
// } NODE;

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
// Queue *make_queue();
// void print_queue(NODE *n);
// void insertion(Queue *H, NODE *new, int val);
// void queue_link(Queue *H, NODE *y, NODE *x);
// void consolidate(Queue *H);
// NODE *extract_min(Queue *H);
//void push(Node* head, Process* process);
void append(Queue* queue, Process* process);
void printList(Node* node);
void deleteHead(Queue* queue);
void deleteNode(Node* node, Queue* queue);
void sortQueue(Queue* queue);
