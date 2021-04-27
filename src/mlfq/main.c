#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include "../file_manager/process.h"
#include "../file_manager/manager.h"


// INTENTO DE GIT 
Queue** init_queues(Queue** queues, int Q, int q){
  // Generar la cantidad Q de colas y guardarlas en el array de colas
  for (int j = 0; j < Q; j++){
    int quantum = (Q - j) * q;
    // Process** process_queue = calloc(255, sizeof(Process*));
    Queue* queue = calloc(1, sizeof(Queue));
    queue->Q = Q;
    queue->p = j;
    queue->q = q;
    queue->quantum = quantum;
    queue->length = 0;
    // queue->process_queue = process_queue;
    queues[j] = queue;
  }
  return queues;
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
    process->cycles = atoi(line[2]);
    process->start_time = atoi(line[3]);
    process->wait = atoi(line[4]);
    process->waiting_delay = atoi(line[5]);
    process->chosen = 0;
    process->interrupted = 0;
    process->first_time = 0;
    process->end_time = 0;
    Queue* queue = queues[0];
    add_process(queue, process);

  
  
  // Ejecutar procesos hasta que todos hayan terminado
  }
  print_queue(queues[0]);
  int timer = 0;
  while(finished < total){
    Queue* most_prior = queues[0];
    Process* current = most_prior->process_queue[0];
    printf("cycles %d, quantum %d\n", current->cycles, most_prior->quantum);
    
    // Si cycles es mayor a quantum, ejecuta quantum y lo sacan de la cola
    if (current->cycles >= most_prior->quantum){
      timer += current->cycles;
      current->status="RUNNING";
      current->status="FINISHED";
      finished_p[finished] = current;
      finished++; 
      printf("name %s\n", finished_p[0]->name);
    }
    else {
      timer += most_prior->quantum;
      printf("---MOST PRIOR1\n");
      print_queue(most_prior);
      add_process(queues[1], current);
      delete_process(most_prior, 1);
      printf("MOST PRIOR2\n");
      print_queue(most_prior);
      printf("NEXT MOST PRIOR\n");
      print_queue(queues[1]);
      finished_p[finished] = current;
      printf("name %s\n", finished_p[0]->name);
    }


    // Sino, ejecuta cycles y termina
    break;
  }
  
  
  return 0;
}
