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
// Inicializa un lista ligada
// Queue *make_queue() {
//   Queue *H;
//   H = (Queue *)malloc(sizeof(Queue));
//   H->n = 0;
//   H->min = NULL;
//   H->phi = 0;
//   H->degree = 0;
//   //
//   H->Q = 0;
//   H->priority = 0;
//   H->p = 0;
//   H->q = 0;
//   H->quantum = 0;
//   H->length = 0;
//   return H;
// }

// // Imprime la lista ligada, obtenido de https://gist.github.com/ArgiesDario/da409828e81ef441186268b8ee3acd5f
// void printList(NODE head){
//     while(head != NULL){ //Mientras head no sea NULL
//         printf("%4d",head->process->name); //Imprimimos el valor del nodo
//         head = head->next; //Pasamos al siguiente nodo
//     }
// }
 

// // Inserting nodes
// void insertion(Queue *H, NODE *new, int val){
//     new = (NODE *)malloc(sizeof(NODE));
//     new->key = val;
//     new->degree = 0;
//     new->mark = false;
//     new->visited = false;
//     new->previous = new;
//     new->next = new;
//     if (H->min == NULL) {
//       H->min = new;
//     } else {
//       H->min->previous->next = new;
//       new->next = H->min;
//       new->previous = H->min->previous;
//       H->min->previous = new;
//       if (new->key < H->min->key) {
//         H->min = new;
//       }
//     }
//     (H->n)++;
// }

// // Linking
// void queue_link(Queue *H, NODE *y, NODE *x) {
//   y->next->previous = y->previous;
//   y->previous->next = y->next;

//   if (x->next == x)
//     H->min = x;

//   y->previous = y;
//   y->next = y;


//   y->next = x->child;
//   y->previous = x->child->previous;
//   x->child->previous->next = y;
//   x->child->previous = y;
//   if ((y->key) < (x->child->key))
//     x->child = y;

//   (x->degree)++;
// }

// // Consolidate function
// void consolidate(Queue *H) {
//   int degree, i, d;
//   degree = cal_degree(H->n);
//   NODE *A[degree], *x, *y, *z;
//   for (i = 0; i <= degree; i++) {
//     A[i] = NULL;
//   }
//   x = H->min;
//   do {
//     d = x->degree;
//     while (A[d] != NULL) {
//       y = A[d];
//       if (x->key > y->key) {
//         NODE *exchange_help;
//         exchange_help = x;
//         x = y;
//         y = exchange_help;
//       }
//       if (y == H->min)
//         H->min = x;
//       queue_link(H, y, x);
//       if (y->next == x)
//         H->min = x;
//       A[d] = NULL;
//       d++;
//     }
//     A[d] = x;
//     x = x->next;
//   } while (x != H->min);

//   H->min = NULL;
//   for (i = 0; i < degree; i++) {
//     if (A[i] != NULL) {
//       A[i]->previous = A[i];
//       A[i]->next = A[i];
//       if (H->min == NULL) {
//         H->min = A[i];
//       } else {
//         H->min->previous->next = A[i];
//         A[i]->next = H->min;
//         A[i]->previous = H->min->previous;
//         H->min->previous = A[i];
//         if (A[i]->key < H->min->key) {
//           H->min = A[i];
//         }
//       }
//       if (H->min == NULL) {
//         H->min = A[i];
//       } else if (A[i]->key < H->min->key) {
//         H->min = A[i];
//       }
//     }
//   }
// }

// // Extract min
// NODE *extract_min(Queue *H) {
//   if (H->min == NULL)
//     printf("\n The heap is empty");
//   else {
//     NODE *temp = H->min;
//     NODE *pntr;
//     pntr = temp;
//     NODE *x = NULL;
//     if (temp->child != NULL) {
//       x = temp->child;
//       do {
//         pntr = x->next;
//         (H->min->previous)->next = x;
//         x->next = H->min;
//         x->previous = H->min->previous;
//         H->min->previous = x;
//         if (x->key < H->min->key)
//           H->min = x;
//         x->parent = NULL;
//         x = pntr;
//       } while (pntr != temp->child);
//     }

//     (temp->previous)->next = temp->next;
//     (temp->next)->previous = temp->previous;
//     H->min = temp->next;

//     if (temp == temp->next && temp->child == NULL)
//       H->min = NULL;
//     else {
//       H->min = temp->next;
//       consolidate(H);
//     }
//     H->n = H->n - 1;
//     return temp;
//   }
//   return H->min;
// }

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
        printf("%s, STATUS: %s\n", node->process->name, node->process->status);
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

void deleteNode(Node* node, Queue* queue){
  // NO ES EL ÚLTIMO

  if (node->next != NULL){
    // // Está inicializado el siguiente
      // NI ES EL PRIMERO = ES UNO DE AL MEDIO
      if (node->prev != NULL){ 
        node->prev->next = node->next;
        node->next->prev = node->prev;

      }
      // O ES LA CABEZA
      else {
        node->next->prev = NULL;
        queue->head = node->next;
      }

  //     //free(node);
  }


  // ES LA COLA
  else {
    // SI TAMBIEN SOY LA CABEZA, ES DECIR SOY EL UNICO ELEMENTO:
    if (node->prev == NULL){
      Node* new_node = calloc(1,sizeof(Node));
      new_node->value = 0;
      queue->head = new_node;
    }
    else {
      node->prev->next = NULL;
    }
   
      // free(node)
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