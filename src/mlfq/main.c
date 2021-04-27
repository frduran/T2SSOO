#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "../file_manager/process.h"
#include "../file_manager/manager.h"


/* FALTA INTEGRAR ESCRITURA DE ARCHIVO, SORT, LÓGICA DE PERIODO EN EL CUAL PASAN A COLA DE MAYOR PRIORIDAD
   CALCULO DE WAITING TIME Y TAMBIÉN WAIT Y WAITING_DELAY QUE SE DA COMO ATRIBUTO
   DE UN PROCESO */

Queue** update_waiting(Queue** queues, int total, int timer){
  printf("//////////////////////////////////////vamos a actualizar los waiting\n");
  for (int i=0; i<total; i++){
    printf("Cola %d\n", i);
    printList(queues[i]->head);
    Node* check = queues[i]->head;
    if (check->value != 0){
      while (true){
      
      if (check->process->status == "WAITING"){
        printf("!!FIRST WAIT=%d", check->process->first_wait);
        int dif = timer - check->process->first_wait;
        printf("este proceso están en waiting desde %d, ha pasado %d", check->process->first_wait, dif);

        if (dif > check->process->waiting_delay){
          check->process->total_time_waiting += check->process->waiting_delay;
          check->process->status="READY";
          printf("proceso %s pasa a ready", check->process->name);
        }
        else {
          check->process->total_time_waiting += dif;
        }
      } 
      
      if (check->next != NULL){
        check = check->next;
      }
      else {
        break;
      }   
    }
    }
    
    
  }
  printf("//////////////////////////////////////terminado de actualizar los waiting\n");
  return queues;
}

Queue** init_queues(Queue** queues, int Q, int q){
  // Generar la cantidad Q de colas y guardarlas en el array de colas
  for (int j = 0; j < Q; j++){
    int quantum = (Q - j) * q;
    // Process** process_queue = calloc(255, sizeof(Process*));
    Queue* queue = calloc(1, sizeof(Queue));
    Node* node = calloc(1,sizeof(Node));
    queue->Q = Q;
    queue->head = node;
    queue->head->value = 0;
    queue->p = j;
    queue->q = q;
    queue->quantum = quantum;
    queue->length = 0;
    // queue->process_queue = process_queue;
    queues[j] = queue;
  }
  return queues;
}

char* concat_array(char** array){
  int n_args = 6;
  char* line = calloc(6, sizeof(char));
  
  for (int i = 0; i < n_args; i++)
  {
    strcat(line, array[i]);
    // if (i < n_args + 3){
    //   strcat(line, ",");
    // }
  }
  printf("%s", line);
  return line;
}

void write_output(char* path, char** args) {
  FILE* file = fopen(path, "a");
  char* line = "hhhhh";
  //char* line = concat_array(args);
  printf("Escribiendo linea: %s en ./%s.txt\n", line, path);
  fputs(line, file);
  //free(line);
  fclose(file);
}

int response_time(Process* process){
  int first = process->first_time;
  int arrive = process->start_time;
  return first-arrive;
}

int turnaround_time(Process* process){
  int end = process->end_time;
  int arrive = process->start_time;
  return end-arrive;
}

// void delete_process(Queue* queue, int pos){
//   // for (int i=0; i<queue->length; i++){
//   //   queue->process_queue[i-1] = queue->process_queue[i];
//   // }
//   memcpy (queue->process_queue, queue->process_queue+1, sizeof(Process*));
//   print_queue(queue);
// }

int main(int argc, char **argv)
{
  char* input_path = argv[1];
  char* output_path = argv[2];
  int Q = atoi(argv[3]);  // Cantidad de colas
  int q = atoi(argv[4]);  // Variable para cálculo de quantums
  int S = atoi(argv[5]);  // Período en el que los procesos pasan a cola de mayor prioridad
  Queue** queues = calloc(255, sizeof(Queue*));
  Process** finished_p = calloc(2048, sizeof(Process*));

  queues = init_queues(queues, Q, q);

  // Lectura del archivo
  InputFile* input = read_file(input_path);
  int total = input->len;
  
  int finished = 0; // contador de los procesos terminados

  // Inicializar los procesos, leídos del archivo input
  for (int i = 0; i < total; i++) {
    char** line = input->lines[i];
    Process* process = calloc(1, sizeof(Process));
    process->pid = atoi(line[1]);
    process->name = line[0];
    process->priority = 0;
    process->status = "READY";
    process->cycles = atoi(line[3]);
    process->start_time = atoi(line[2]);
    process->wait = atoi(line[4]);
    process->waiting_delay = atoi(line[5]);
    process->chosen = 0;
    process->interrupted = 0;
    process->first_wait = 0;
    process->first_time = 0;
    process->end_time = 0;
    Queue* queue = queues[0];
    append(queue, process);
  }
  printList(queues[0]->head);
  // sortQueue(queues[0]);
  // printList(queues[0]->head);

  int timer = 0;

  //////////////////////////////////////////////////////////////////////////////////////7
  // Ejecutar procesos hasta que todos hayan terminado
  while(finished < total){
    int j=0;
    Process* current;
    Node* current_node;
    printf("\n////TIMER: %d", timer);
    // Chequear que cola ejecutar
    
    Queue* exe = queues[j];
    while(true){
      printf("***CHEQUEANDO COLA %d\n", j);
      // Revisar todos los nodos y actualizar a los waiting

      // Cola tiene cabeza
      int forward = 0;
      printf("queue %d, head: %d\n", j, queues[j]->head->value);
      
      if (queues[j]->head->value == 1){
        // Revisar si hay algún proceso en estado READY
        Node* check = queues[j]->head;
        while (true){
          
          if (check->process->status == "READY"){
            current_node = check;
            current = check->process;
            printf("NODE %s IS READY\n", current->name);
            break;
          } 
          // SI NO ERA EL ÚLTIMO DE LA COLA
          else if (check->next != NULL){
            printf("PROCESO %s ESTÁ EN PROCESO WAITING\n", check->process->name);
            check = check->next;
          // SI ERA EL ÚLTIMO, BREAK Y CHEQUEAR LA SIGUIENTE COLA
          } else {
            forward = 1;
            break;
          }
          
        }
        if (forward){
          j++;
        }
        else {
          break;
        }
        
      }
      else {
        j++;
      }
    }
    // Process* current = queues[j]->head->process;
    printf("cycles %d, quantum %d\n", current->cycles, queues[j]->quantum);
    
    // Chequear si el proceso cede la CPU
    if (current->wait != 0){
      // Ver si el tiempo que ejecuta antes de ceder, es menor que el quantum
      // Se avanza ese tiempo en el timer
      if (current->wait < queues[j]->quantum){
        printf("PROCESO %s CEDE CPU", current->name);
        if(current->first_time == 0){
          current->first_time = timer;
        }
        // Avanzar reloj del sistema
        // Si le queda menos de lo que va a ejecutar, ejecutar el máximo posible
        if (current->wait > current->cycles){
          timer += current->cycles;
        }
        else {
          timer += current->wait;
        }
        
        current->status="RUNNING";
        current->status="WAITING";
        current->chosen++;
        if(current->first_wait == 0){
          current->first_wait = timer;
        }
        // SI termina
        if (current->cycles - current->wait <= 0){
          current->cycles = 0; //current->cycles - current->wait;
          current->end_time = timer;
          finished_p[finished] = current;
          finished++; 
          printList(queues[j]->head);
          deleteNode(current_node, queues[j]);

        }
        else {
          current->cycles = current->cycles - current->wait;
           printf("LE QUEDA %d a PROCESO %s, timer=%d\n\n", current->cycles, current->name, timer);
        
          // Si estaba en la cola de mayor prioridad, se queda ahí, sino
          // Subirle a la cola de mayor prioridad
          if (j != 0){
            deleteNode(current_node, queues[j]);
            append(queues[0], current);
          }
          printf("LISTA 0 DESPUÉS DE SUBIR\n");
          printList(queues[0]->head);
        }
      }
    }
    // Si cycles es menor a quantum, ejecuta cycles y termina 
    else if (current->cycles <= queues[j]->quantum){
      if(current->first_time = 0){
        current->first_time = timer;
      }
      // Avanzar reloj del sistema
      timer += current->cycles;
      current->status="RUNNING";
      current->status="FINISHED";
      current->chosen++;
      current->end_time = timer;
      finished_p[finished] = current;
      finished++; 
      printList(queues[j]->head);
      deleteNode(current_node, queues[j]);
      // deleteHead(queues[j]);

      //printf("ELIMINAR DE LA COLA1 name %s\n", finished_p[finished]->name);
    }
    // Sino, ejecuta quantum y lo mueven a cola inferior
    else {
      current->first_time = timer;
      // Avanzar reloj del sistema
      timer += queues[j]->quantum;
      current->chosen = 1;
      current->cycles = current->cycles - queues[j]->quantum;
      current->interrupted = 1;
      append(queues[j+1], current);
      deleteNode(current_node, queues[j]);
      //deleteHead(queues[j]);
    }
    printf("Lista %d", j);
    printList(queues[j]->head);
    printf("Lista %d", j+1);
    printList(queues[j+1]->head);
    queues = update_waiting(queues, total, timer);

  }
  printf("TIEMPO FINAL %d\n", timer);
  printf("total: %d\n", total);
  for (int w=0; w<total; w++){
    char** args_to_file = calloc(6, sizeof(char*));
    
    Process* pr = finished_p[w];
    args_to_file[0] = pr->name;
    args_to_file[1] = pr->chosen;
    args_to_file[2] = pr->interrupted;
    args_to_file[3] = turnaround_time(pr); // calcular turnaround
    args_to_file[4] = response_time(pr); // calcular response time
    args_to_file[5] = 0; // calcular waiting time
    printf("---->%s,%d,%d,%d,%d,%d\n", args_to_file[0], args_to_file[1], args_to_file[2], args_to_file[3], args_to_file[4],args_to_file[5] );
    
    // ------------- No me está funcionando la escritura
    //concat_array(args_to_file);
    //write_output(output_path, args_to_file);
  }
  

  
  return 0;
}
