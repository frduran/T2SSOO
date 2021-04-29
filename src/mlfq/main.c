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


/* FALTA INTEGRAR ESCRITURA DE ARCHIVO, SORT  */

// Función que appendea todos los procesos de las colas 1...última a la cola 0, de mayor prioridad
Queue** update_S(Queue** queues, int total, int S, int S_passed){
  printf("///////////////////////////////////////\nPasó tiempo S, subir todos a cola 0\n////////////////////////////////////////\n");
  // Si es que hay más de 1 cola:
  if (total>=1){

    for (int i=1; i<total; i++){

      // printList(queues[i]->head);
      Node* check = queues[i]->head;
      // printList(check);
      Node* new_check;
      // Si es que el nodo está inicializado
      if (check->value != 0){
        while (true){
          // Appendea el nodo a la cola 0 y lo elimina
          new_check = append(queues[0], check->process);
          Node* check_next = check->next;
          deleteNode(check, queues[i]);  
          if (check_next != NULL){
            check = check_next;
          }
          else {
            break;
          }   
        }
      }
    }
  }
  return queues;
}

// Revisa todos los procesos y el tiempo que llevan esperando, comparando con waiting_cycle
// Si el proceso ya completó el waiting cycle, lo pasa a READY
Queue** update_waiting(Queue** queues, int total, int timer){
  // printf("////////////////////////////////////// Chequear procesos waiting\n");
  for (int i=0; i<total; i++){
    // printf("Cola %d\n", i);
    // printList(queues[i]->head);
    Node* check = queues[i]->head;
    if (check->value != 0){
      while (true){

        // Solo chequea a los procesos con status WAITING
        if (strcmp(check->process->status, "WAITING") == 0){
          int dif = timer - check->process->first_wait;
          // printf("Diferencia entre timer: %d y first_wait: %d = %i\n", timer, check->process->first_wait, dif);
          // Si completó su waiting delay, lo pasa a READY
          if (dif >= check->process->waiting_delay){
            check->process->total_time_waiting += check->process->waiting_delay;
            check->process->status="READY";
            check->process->chosen++;
            check->process->first_ready = check->process->first_wait + check->process->waiting_delay;
            check->process->first_wait = -1;
            // printf("proceso %s pasa a ready", check->process->name);
            check->process->waiting_delay_left = check->process->waiting_delay;
          }
          // Si no completó su waiting_delay, se le resta dif a waiting_delay_left
          //Este se ocupa solo en caso de que sólo queden procesos waiting en mlfq (línea 222)
          else{
            check->process->waiting_delay_left = check->process->waiting_delay - dif;
          }
        } 
        // Si no era el último nodo, avanza al siguiente
        if (check->next != NULL){
          check = check->next;
        }
        else {
          break;
      }  
    }
    }
    
    
  }
  // printf("//////////////////////////////////////Terminé de actualizar los waiting\n");
  return queues;
}

// Inicializa todas las colas señaladas
Queue** init_queues(Queue** queues, int Q, int q){
  // Genera la cantidad Q de colas y las guarda en el array de colas
  for (int j = 0; j < Q; j++){
    int quantum = (Q - (Q - j - 1)) * q;
    Queue* queue = calloc(1, sizeof(Queue));
    Node* node = calloc(1,sizeof(Node));
    queue->Q = Q;
    queue->head = node;
    queue->head->value = 0;
    queue->head->process = NULL;
    queue->p = j;
    queue->q = q;
    queue->quantum = quantum;
    queue->length = 0;
    queues[j] = queue;
  }
  return queues;
}

char* concat_array(char** array){
  int n_args = 6;
  char* line = calloc(255, sizeof(char));
  
  for (int i = 0; i < n_args; i++)
  {
    strcat(line, array[i]);
    if (i < n_args - 1){
      strcat(line, ",");
    }
  }
  printf("%s", line);
  return line;
}

void write_output(char* path, char** args) {
  FILE* file = fopen(path, "a");
  char* line = concat_array(args);
  printf("Escribiendo linea: %s en ./%s.txt\n", line, path);
  fputs(line, file);
  free(line);
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

  FIB_HEAP *heap;
  heap = make_fib_heap();
  // Inicializar los procesos, leídos del archivo input
  for (int i = 0; i < total; i++) {
    fib_node *new_node;
    new_node = (fib_node *)malloc(sizeof(fib_node));
    char** line = input->lines[i];
    Process* process = calloc(1, sizeof(Process));
    process->pid = atoi(line[1]);
    process->name = line[0];
    process->priority = 0;
    process->status = "READY";
    process->cycles = atoi(line[3]);
    process->start_time = atoi(line[2]);
    process->ready_time = 0;
    process->first_ready = atoi(line[2]);
    process->wait = atoi(line[4]);
    process->wait_left = atoi(line[4]);
    process->waiting_delay = atoi(line[5]);
    process->waiting_delay_left = atoi(line[5]);
    process->chosen = 0;
    process->interrupted = 0;
    process->first_wait = -1;
    process->first_time = -1;
    process->end_time = 0;
    new_node->process = process;
    insertion(heap, new_node, process->start_time);
    // Queue* queue = queues[0];
    // append(queue, process);
  }
  //Ahora guardamos los procesos ya ordenados en el heap en la cola de primera prioridad
  for (int j = 0; j < total; j++){
    fib_node *extracted = extract_min(heap);
    append(queues[0], extracted->process);
    free(extracted);
  }
  free(heap);

  // printList(queues[0]->head);
  // sortQueue(queues[0]);

  int timer = 0;
  int S_passed = 0;

  //////////////////////////////////////////////////////////////////////////////////////7
  // Ejecutar procesos hasta que todos hayan terminado
  while(finished < total){
    // for (int i=0; i<total; i++){
    //   printf("inicio de while---timer = %d,Cola %d\n", timer,i);
    //   printList(queues[i]->head);
    // }
    int j = 0;
    Process* current;
    Node* current_node;
    printf("\n////TIMER: %d, S_passed: %d\n", timer, S_passed);
    // Chequear que cola ejecutar

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    while(true){
      // Si recorrió todas las colas y no encontró procesos READY:
      if (j == Q){
        j = 0;
        printf("ME PASÉ, TODOS EN WAITING\n");
        int min_wait_delay = 99999;
        for (int i = 0; i < Q; i++)
        {
          if (queues[i]->head->value == 1){
            int left = queues[i]->head->process->waiting_delay_left;
            if (left <  min_wait_delay){
              //Guarda solo el waiting left para después sumarlo al timer
              //Luego de sumarlo llama a update waiting, y ahí va a cachar que el proceso que tenía el min_wait_delay tiene que pasar a READY
              min_wait_delay = left;
            }
          }
        }
        printf("Tengo que avanzar %d ciclos en el timer\n", min_wait_delay);
        timer += min_wait_delay;
        S_passed += min_wait_delay;
        queues = update_waiting(queues, total, timer);

        if (S_passed >= S){
          queues = update_S(queues, total, S, S_passed);
          S_passed = 0;
        }
        printf("\n////TIMER: %d, S_passed: %d\n", timer, S_passed);
      }
     /////////////////////////////////////////////////////////////////////////////////////////////////////
      printf("****************************\nCHEQUEANDO COLA %d\n", j);
      // Cola tiene cabeza
      int forward = 0;
      // printf("queue %d, head->value: %d\n", j, queues[j]->head->value);
      
      if (queues[j]->head->value == 1){
        
        // Revisa si hay algún proceso en estado READY
        Node* check = queues[j]->head;
        while (true){
          
          if (strcmp(check->process->status, "READY") == 0){
            current_node = check;
            current = check->process;
            printf("NODE %s IS READY\n", current->name);
            break;
          } 
          // Si no está en ready y no era el último, avanza al siguiente
          else if (check->next != NULL){
            printf("PROCESO %s ESTÁ EN ESTADO WAITING\n", check->process->name);
            check = check->next;
          // Si era el último, break y avanza a siguiente cola
          } else {
            printf("PROCESO %s ESTÁ EN ESTADO WAITING\n", check->process->name);
            printf("PROCESO %s ERA EL ULTIMO\n", check->process->name);
            forward = 1;
            break;
          }
          
        }
        if (forward){
          j++;
          // printf("ahora a chequear cola %d", j);
          if (j==Q){
            break;
          }
        }
        else {
          break;
        }
        
      }
      else {
        // printf("2Estoy en cola %d\n", j);
        j++;
        // printf("3ahora a chequear cola %d, el valor de Q es %d\n", j, Q);
      }

    }
    printf("ANTES:\n cycles %d, quantum %d, wait_left %d\n chosen %d, interrupted %d\n", current->cycles, queues[j]->quantum, current->wait_left, current->chosen, current->interrupted);
    printf("EJECUTA\n");
    current->ready_time += (timer - current->first_ready);
    if(current->first_time == -1){
      current->first_time = timer;
    }

    // AQUI ENTRA SOLO SI EL PROCESO TIENE WAIT Y CEDE LA CPU
    if (current->wait != 0){
      // Ver si el tiempo que ejecuta antes de ceder, es menor que el quantum
      // Se avanza ese tiempo en el timer
      if (current->wait_left < queues[j]->quantum){
        printf("PROCESO %s CEDE CPU\n", current->name);
        current->status="RUNNING";
        current->status="WAITING";
        current->chosen++;
        // Si termina, sumar cycles a timer y S_passed
        if (current->cycles <= current->wait_left){
          timer += current->cycles;
          S_passed += current->cycles;
        }
        // Si no, sumar wait_left
        else {
          timer += current->wait_left;
          S_passed += current->wait_left;
        }

        // if(current->first_wait == -1){
        //   current->first_wait = timer;
        // }
        // Si termina, sacarlo de la cola y guardarlo en array de terminados
        if (current->cycles <= current->wait_left){ //cycles < waiting < quantum
          current->status="FINISHED";
          current->chosen++;
          current->end_time = timer;
          finished_p[finished] = current;
          finished++; 
          // printList(queues[j]->head);
          deleteNode(current_node, queues[j]);
          printf("Proceso %s terminado\n", current->name);
        }
        else { //CEDE LA CPU! waiting < cycles < quantum
          current->first_wait = timer;
          current->cycles = current->cycles - current->wait_left;
          printf("LE QUEDA %d a PROCESO %s, timer=%d\n\n", current->cycles, current->name, timer);
          current->wait_left = current->wait;
          // Si estaba en la cola de mayor prioridad, se queda ahí, sino
          // Subirle a la cola de mayor prioridad
          if (j != 0){ 
            deleteNode(current_node, queues[j]);
            append(queues[j - 1], current);
          } else {
            deleteNode(current_node, queues[j]);
            append(queues[0], current);
          }
          // printList(queues[0]->head);
        }
      }
      // ELSE IF DE SI wait_left >= quantum
      else if (current->wait_left >= queues[j]->quantum){
        if (current->cycles <= queues[j]->quantum){ //cycles <= quantum < waiting
          timer += current->cycles;
          S_passed += current->cycles;
          current->wait_left -= current->cycles;
          current->status="RUNNING";
          current->status="FINISHED";
          current->chosen++;
          if (current->cycles == queues[j]->quantum){
            current->interrupted++;
          }
          current->end_time = timer;
          finished_p[finished] = current;
          finished++; 
          // printList(queues[j]->head);
          deleteNode(current_node, queues[j]);
        }
        else if (current->cycles > queues[j]->quantum){  //quantum <= waiting < cycles
          timer += queues[j]->quantum;
          S_passed += queues[j]->quantum;
          // current->wait_left -= queues[j]->quantum;
          current->wait_left = current->wait;
          current->chosen++;
          current->cycles = current->cycles - queues[j]->quantum;
          current->interrupted++;
          if (j == total - 1){
            append(queues[j], current);
          } else {
            append(queues[j+1], current);
          }
          deleteNode(current_node, queues[j]);
        }
      }
      // else { //ELSE DE SI quantum == wait_left
      //   if(current->first_wait == -1){
      //     current->first_wait = timer;
      //   }
      //   timer += queues[j]->quantum;
      //   S_passed += queues[j]->quantum;
      //   current->wait_left -= queues[j]->quantum;
      //   current->chosen++;
      //   current->cycles = current->cycles - queues[j]->quantum;
      //   current->interrupted++;
      //   current->status = "WAITING";
      //   append(queues[j+1], current);
      //   deleteNode(current_node, queues[j]);
      // }
    }
    // DE AQUI EN ADELANTE SIGUE SI NO HACE WAITING
    // Si cycles es menor a quantum, ejecuta cycles y termina 
    else if (current->cycles <= queues[j]->quantum){
      // Avanzar reloj del sistema
      timer += current->cycles;
      S_passed += current->cycles;
      current->status="RUNNING";
      current->status="FINISHED";
      current->chosen++;
      current->end_time = timer;
      finished_p[finished] = current;
      finished++; 
      // printList(queues[j]->head);
      deleteNode(current_node, queues[j]);
    }

    // Sino, ejecuta quantum y lo mueven a cola inferior
    else {
      // Avanzar reloj del sistema
      timer += queues[j]->quantum;
      S_passed += queues[j]->quantum;
      current->chosen++;
      current->cycles = current->cycles - queues[j]->quantum;
      current->interrupted++;
      // printf("PROCESO %s sumando a interrupted: %i\n", current->name, current->interrupted);
      append(queues[j+1], current);
      deleteNode(current_node, queues[j]);
    }
    // printf("Lista %d", j);
    // printList(queues[j]->head);
    // printf("Lista %d", j+1);
    // printList(queues[j+1]->head);
    queues = update_waiting(queues, total, timer);

    if (S_passed >= S){
      queues = update_S(queues, total, S, S_passed);
      S_passed = 0;
    }
    printf("DESPUES:\n cycles %d, quantum %d, wait_left%d\n chosen %d, interrupted %d\n", current->cycles, queues[j]->quantum, current->wait_left, current->chosen, current->interrupted);

  }
  // FIN SIMULACIÓN
  printf("TIEMPO FINAL %d\n", timer);
  printf("total: %d\n", total);
  for (int w=0; w<total; w++){
    char** args_to_file = calloc(6, sizeof(char*));
    
    Process* pr = finished_p[w];

    char chosen[3];
    char interrupted[3];
    char turn_around[3];
    char response[3];
    char total_waiting[3];
    // args_to_file[1] = pr->chosen;
    sprintf(chosen, "%d", pr->chosen);
    // args_to_file[2] = pr->interrupted;
    sprintf(interrupted, "%d", pr->interrupted);
    // args_to_file[3] = turnaround_time(pr); // calcular turnaround
    sprintf(turn_around, "%d", turnaround_time(pr));
    // args_to_file[4] = response_time(pr); // calcular response time
    sprintf(response, "%d", response_time(pr));
    // args_to_file[5] = pr->total_time_waiting; // calcular waiting time
    sprintf(total_waiting, "%d\n", pr->total_time_waiting + pr->ready_time);
    args_to_file[0] = pr->name;
    args_to_file[1] = chosen;
    args_to_file[2] = interrupted;
    args_to_file[3] = turn_around;
    args_to_file[4] = response;
    args_to_file[5] = total_waiting;
    printf("---->%s,%s,%s,%s,%s,%s\n", args_to_file[0], args_to_file[1], args_to_file[2], args_to_file[3], args_to_file[4], args_to_file[5]);
    
    // ------------- No me está funcionando la escritura
    // char* line_to_file = concat_array(args_to_file);
    write_output(output_path, args_to_file);
    free(args_to_file);
  }
  for (int f = 0; f < total; f++)
    {
      free(finished_p[f]);
    }
    free(finished_p);

    for (int d = 0; d < Q; d++)
    {
      Node* node = queues[d]->head;
      Node* node_copy;
      if (node->value == 1){
        while(true){
          if (node->next){
            node_copy = node->next;
            free(node->process);
            free(node);
            node = node_copy;
          } else{
            free(queues[d]);
            break;
          }
        
        }
      } else {
        free(node);
        free(queues[d]);
      }
      
    }
    free(queues);
  input_file_destroy(input);
  return 0;
}
