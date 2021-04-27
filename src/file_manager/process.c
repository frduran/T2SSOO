// Import used global libraries
#include <stdio.h>  // FILE, fopen, fclose, etc.
#include <string.h> // strtok, strcpy, etc.
#include <stdlib.h> // malloc, calloc, free, etc.

// Import the header file of this module
#include "process.h"

// void add_process(Queue* queue, Process* process){
//     int length = queue->length;
//     queue->process_queue[length] = process;
//     queue->length++;
// }

// void print_queue(Queue* queue){
//     int length = queue->length;
//     printf("Queue length %d\n",length);
//     for (int i=0; i<length; i++){
//         printf("%s\n", queue->process_queue[i]->name);
//     }
// }

// Funciones obtenidas de: https://www.programiz.com/dsa/fibonacci-heap
// Inicializa un fibonacci heap
FIB_HEAP *make_fib_heap() {
  FIB_HEAP *H;
  H = (FIB_HEAP *)malloc(sizeof(FIB_HEAP));
  H->n = 0;
  H->min = NULL;
  H->phi = 0;
  H->degree = 0;
  return H;
}

// Printing the heap
void print_heap(fib_node *n) {
  fib_node *x;
  for (x = n;; x = x->right_sibling) {
    if (x->child == NULL) {
      printf("node with no child: %s (%d) \n", x->process->name, x->key);
    } else {
      printf("fib_node: %s(%d) with child:%s (%d)\n", x->process->name, x->key, x->child->process->name, x->child->key);
      print_heap(x->child);
    }
    if (x->right_sibling == n) {
      break;
    }
  }
}

// // Imprime la lista ligada, obtenido de https://gist.github.com/ArgiesDario/da409828e81ef441186268b8ee3acd5f
// void printList(fib_node head){
//     while(head != NULL){ //Mientras head no sea NULL
//         printf("%4d",head->process->name); //Imprimimos el valor del nodo
//         head = head->next; //Pasamos al siguiente nodo
//     }
// }
 

// Inserting nodes
void insertion(FIB_HEAP *H, fib_node *new, int val) {
  new->key = val;
  new->degree = 0;
  new->mark = false;
  new->parent = NULL;
  new->child = NULL;
  new->visited = false;
  new->left_sibling = new;
  new->right_sibling = new;
  if (H->min == NULL) {
    H->min = new;
  } else {
    H->min->left_sibling->right_sibling = new;
    new->right_sibling = H->min;
    new->left_sibling = H->min->left_sibling;
    H->min->left_sibling = new;
    if (new->key < H->min->key) {
      H->min = new;
    }
  }
  (H->n)++;
}


// Linking
void fib_heap_link(FIB_HEAP *H, fib_node *y, fib_node *x) {
  y->right_sibling->left_sibling = y->left_sibling;
  y->left_sibling->right_sibling = y->right_sibling;

  if (x->right_sibling == x)
    H->min = x;

  y->left_sibling = y;
  y->right_sibling = y;
  y->parent = x;

  if (x->child == NULL) {
    x->child = y;
  }
  y->right_sibling = x->child;
  y->left_sibling = x->child->left_sibling;
  x->child->left_sibling->right_sibling = y;
  x->child->left_sibling = y;
  if ((y->key) < (x->child->key))
    x->child = y;

  (x->degree)++;
}

// Calculate the degree
int cal_degree(int n) {
  int count = 0;
  while (n > 0) {
    n = n / 2;
    count++;
  }
  return count;
}

// Consolidate function
void consolidate(FIB_HEAP *H) {
  int degree, i, d;
  degree = cal_degree(H->n);
  fib_node *A[degree], *x, *y, *z;
  for (i = 0; i <= degree; i++) {
    A[i] = NULL;
  }
  x = H->min;
  do {
    d = x->degree;
    while (A[d] != NULL) {
      y = A[d];
      if (x->key > y->key) {
        fib_node *exchange_help;
        exchange_help = x;
        x = y;
        y = exchange_help;
      }
      if (y == H->min)
        H->min = x;
      fib_heap_link(H, y, x);
      if (y->right_sibling == x)
        H->min = x;
      A[d] = NULL;
      d++;
    }
    A[d] = x;
    x = x->right_sibling;
  } while (x != H->min);

  H->min = NULL;
  for (i = 0; i < degree; i++) {
    if (A[i] != NULL) {
      A[i]->left_sibling = A[i];
      A[i]->right_sibling = A[i];
      if (H->min == NULL) {
        H->min = A[i];
      } else {
        H->min->left_sibling->right_sibling = A[i];
        A[i]->right_sibling = H->min;
        A[i]->left_sibling = H->min->left_sibling;
        H->min->left_sibling = A[i];
        if (A[i]->key < H->min->key) {
          H->min = A[i];
        }
      }
      if (H->min == NULL) {
        H->min = A[i];
      } else if (A[i]->key < H->min->key) {
        H->min = A[i];
      }
    }
  }
}


// Extract min
fib_node *extract_min(FIB_HEAP *H) {
  if (H->min == NULL)
    printf("\n The heap is empty");
  else {
    fib_node *temp = H->min;
    fib_node *pntr;
    pntr = temp;
    fib_node *x = NULL;
    if (temp->child != NULL) {
      x = temp->child;
      do {
        pntr = x->right_sibling;
        (H->min->left_sibling)->right_sibling = x;
        x->right_sibling = H->min;
        x->left_sibling = H->min->left_sibling;
        H->min->left_sibling = x;
        if (x->key < H->min->key)
          H->min = x;
        x->parent = NULL;
        x = pntr;
      } while (pntr != temp->child);
    }

    (temp->left_sibling)->right_sibling = temp->right_sibling;
    (temp->right_sibling)->left_sibling = temp->left_sibling;
    H->min = temp->right_sibling;

    if (temp == temp->right_sibling && temp->child == NULL)
      H->min = NULL;
    else {
      H->min = temp->right_sibling;
      consolidate(H);
    }
    H->n = H->n - 1;
    return temp;
  }
  return H->min;
}
// https://www.geeksforgeeks.org/doubly-linked-list/
/* Given a reference (pointer to pointer) to the head of a list
   and an int, inserts a new node on the front of the list. */
// void push(Node* head, Process* process)
// {
//     /* 1. allocate node */
//     struct Node* new_node = (struct Node*)malloc(sizeof(Node*));
 
//     /* 2. put in the data  */
//     new_node->process = process;
 
//     /* 3. Make next of new node as head and previous as NULL */
//     new_node->next = (head);
//     new_node->prev = NULL;
 
//     /* 4. change prev of head node to new node */
//     if (head != NULL)
//         head->prev = new_node;
 
//     /* 5. move the head to point to the new node */
//     head = new_node;
// }

/* Given a reference (pointer to pointer) to the head
   of a DLL and an int, appends a new node at the end  */
void append(Queue* queue, Process* process)
{
    /* 1. allocate node */
    Node* head = queue->head;
    Node* new_node = calloc(1,sizeof(Node));
 
    Node* last = head; /* used in step 5*/
 
    /* 2. put in the data  */
    new_node->process = process;
 
    /* 3. This new node is going to be the last node, so
          make next of it as NULL*/
    new_node->next = NULL;
    new_node->value = 1;
 
    /* 4. If the Linked List is empty, then make the new
          node as head */
    if (head->value == 0) {
        new_node->prev = NULL;
        head = new_node;
        queue->head = head;
        return;
    }
 
    /* 5. Else traverse till the last node */
    while (last->next != NULL)
        last = last->next;
 
    /* 6. Change the next of last node */
    last->next = new_node;
    // printf("AQQQQQQ");
    // Node* new_tail = calloc(1,sizeof(Node));
    // last->next = new_tail;
    // last->next->value = 0;
 
    /* 7. Make last node as previous of new node */
    new_node->prev = last;
 
    return;
}

// This function prints contents of linked list starting
// from the given node
void printList(Node* node)
{
  if (node->value == 1){
    while (node != NULL) {
        printf(" %s ", node->process->name);
        //printf("next value %d ", node->next->value);
        node = node->next;
    }
  } else {
    printf("EMPTY");
  }
  printf("\n");
}

void deleteHead(Queue* queue){
    // free(head);
  if (queue->head->next != NULL){
    printf("/Value %d/",queue->head->next->value);
    if (queue->head->next->value ==1){
      queue->head = queue->head->next;
      queue->head->prev = NULL;
      
    }
    else {
      Node* new_node = calloc(1,sizeof(Node));
      new_node->value = 0;
      queue->head = new_node;
    }
  }
  else {
      Node* new_node = calloc(1,sizeof(Node));
      new_node->value = 0;
      queue->head = new_node;
  }
    return;
}

void sortQueue(Queue* queue){
  Node* node1 = queue->head;
  Node* node2 = node1->next;
  Node* node1_copy = queue->head;
  while (node2 != NULL && node1 != NULL) {
    
      if (node2->process->start_time < node1->process->start_time){
        // SWAP
        printf("SWAP\n");        
        node1 = node2;
        node1->prev = NULL;
        
        node2 = node1_copy;
        node2->prev = node1;
        node2->next = node1->next; 

        node1->next = node2;
        node1_copy = node1;
        
      }
      else {
        printf("Está bien, avanzando");
        node1 = node2;
        node2 = node2->next;
        node1_copy = node1;

      } 
    }
 

}