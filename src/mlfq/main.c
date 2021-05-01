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


// Función que appendea todos los procesos de las colas 1...última a la cola 0, de mayor prioridad
Queue** update_S(Queue** queues, int total, int S, int S_passed, Node* execute){
  // Si es que hay más de 1 cola:
  if (total>=1){

    for (int i=1; i<total; i++){
      Node* check = queues[i]->head;
      Node* new_check;
      if (check->value != 0){
        while (true){
          Node* check_next;
          if (execute->value == 0){
            new_check = append(queues[0], check->process);
            check_next = check->next;
            deleteNode(check, queues[i]);    
            if (check_next != NULL){
              check = check_next;
            }
            else {
              break;
            }
          }
          else {
            if (check->process->pid == execute->process->pid){
              check_next = check->next;
            }
            else {
              // Appendea el nodo a la cola 0 y lo elimina
              new_check = append(queues[0], check->process);
              check_next = check->next;
              deleteNode(check, queues[i]);  
            }  
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
  }
  return queues;
}

// Revisa todos los procesos y el tiempo que llevan esperando, comparando con waiting_cycle
// Si el proceso ya completó el waiting cycle, lo pasa a READY
Queue** update_waiting(Queue** queues, int total, int timer){
  for (int i=0; i<total; i++){
    if (i == total){
      break;
    }
    Node* check = queues[i]->head;
    if (check->value != 0){
      while (true){

        // Solo chequea a los procesos con status WAITING
        if (check->process->status == "WAITING"){
          int dif = timer - check->process->first_wait;
          // Si completó su waiting delay, lo pasa a READY
          if (dif >= check->process->waiting_delay){
            check->process->total_time_waiting += check->process->waiting_delay;
            check->process->status="READY";
            check->process->first_ready = check->process->first_wait + check->process->waiting_delay;
            check->process->first_wait = -1;
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

//Concatena los elementos de un array en un string, separados por comas
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
  return line;
}

//Escribe los argumentos de un array en la linea de u archivo ''path''
void write_output(char* path, char** args) {
  FILE* file = fopen(path, "a");
  char* line = concat_array(args);
  fputs(line, file);
  free(line);
  fclose(file);
}

//Calcula el response time para un proceso
int response_time(Process* process){
  int first = process->first_time;
  int arrive = process->start_time;
  return first-arrive;
}

//Calcula el turn around time para un proceso
int turnaround_time(Process* process){
  int end = process->end_time;
  int arrive = process->start_time;
  return end-arrive;
}


int main(int argc, char **argv)
{
  //INICIALIZACION DE VARIABLES
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
  int last_start_time = 0;
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
    process->total_time_exe = 0;
    new_node->process = process;
    float new_start_time = (float)process->start_time + ((float)i / 10);

    insertion(heap, new_node, new_start_time);//process->start_time);
  }
  // INICIALIZAMOS COLA BACKUP
  Queue* backup = calloc(1, sizeof(Queue));
  Node* node = calloc(1,sizeof(Node));
  backup->Q = Q;
  backup->head = node;
  backup->head->value = 0;
  backup->head->process = NULL;
  backup->p = 0;
  backup->q = 0;
  backup->quantum = 0;
  backup->length = 0;

  //Ahora guardamos los procesos ya ordenados en el heap en la cola de primera prioridad
  for (int j = 0; j < total; j++){
    fib_node *extracted = extract_min(heap);
    append(backup, extracted->process);
    free(extracted);
  }
  free(heap);
  int timer = 0;
  int S_passed = 0;
  int S_global = 0;
  int next_S = 0;
  int S_times = 0;
  int deleted = 0;
  int move = 0; // move =1 baja - move=2 sube
  Node* backup_node = calloc(1,sizeof(Node));
  backup_node->value = 0;
  Node* current_copy;


  //WHILE DE SIMULACION PRINCIPAL
  // Ejecutar procesos hasta que todos hayan terminado
  while(finished < total){
    deleted = 0;
    move = 0;
    int j = 0;
    Process* current;
    Node* current_node;
    // printf("\n////TIMER: %d, S_passed: %d, SGLOBAL %d\n", timer, S_passed, S_global);
    //BUSCAMOS EL SIGUIENTE PROCESO A EJECUTAR SEGUN LA PRIORIDAD DE MLFQ
    while(true){
      Node* next_head = backup->head;
      // Revisar si ya llegó algún proceso e ingresarlo a la cola 0
      if (next_head->value == 1){
        while(true){
          if (next_head->process->start_time <= timer){
            append(queues[0], next_head->process);
            if (next_head->next != NULL){
              next_head = next_head->next;
              deleteNode(backup->head, backup);
            }
            else {
              deleteNode(next_head, backup);
              break;
            }
          }
          else {
            break;
          }
        }
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      // Si recorrió todas las colas y no encontró procesos READY:
      if (j == Q){
        //Ve el que le queda menos tiempo para llegar
        int start_time_arrived = 99999;
        if (backup->head->value == 1){
          start_time_arrived = backup->head->process->start_time;
        }
        j = 0;
        //Ve el que le queda menos tiempo para salir de WAITING
        int min_wait_delay = 99999;
        for (int i = 0; i < Q; i++)
        {
          Node* check = queues[i]->head;
          if (check->value != 0){
            while (true){
              int left = check->process->waiting_delay_left;
              if (left <  min_wait_delay){
                min_wait_delay = left;
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
        //Ve cual tiempo es mas corto, el que le queda menos para llegar o el que le queda menos para salir de WAITING
        int next_time = 0;
        if (min_wait_delay < start_time_arrived){
          next_time = min_wait_delay;
        }
        else {
          next_time = start_time_arrived;
        }
        //Sumamos el menor al timer
        timer += next_time;
        S_passed += next_time;
        S_global += next_time;
        queues = update_waiting(queues, Q, timer);

        if (S_passed > S){
          queues = update_S(queues, Q, S, S_passed, backup_node);
          S_times ++;
          S_passed = timer - S*S_times;
        }
        else if (S_passed == S){
          queues = update_S(queues, Q, S, S_passed, backup_node);
          S_times ++;
          S_passed = 0;
        }
      }
     /////////////////////////////////////////////////////////////////////////////////////////////////////
      int forward = 0;
      //Buscamos en las colas del mlfq el siguiente proceso en estado READY
      if (queues[j]->head->value == 1){ //Si la cola tiene cabeza
        // Revisa si hay algún proceso en estado READY
        Node* check = queues[j]->head;
        while (true){
          // Si está en ready y "llegó"
          if (check->process->status == "READY"){
            current = check->process;
            deleteNode(check, queues[j]);
            // printf("PROCESO %s IS READY\n", current->name);
            break;
          } 
          // Si no está en ready y no era el último, avanza al siguiente
          else if (check->next != NULL){
            // printf("PROCESO %s ESTÁ EN ESTADO WAITING\n", check->process->name);
            check = check->next;
          // Si era el último, break y avanza a siguiente cola
          } else {
            forward = 1;
            break;
          }
        }
        if (forward){
          j++;
          if (j==Q){
            break;
          }
        }
        else {
          break;
        }
      }
      //Si la cola esta vacia, avanzamos a la siguiente
      else {
        j++;
      }
    }
    //YA ELEGIMOS EL SIGUIENTE PROCESO A EJECUTAR
    //AHORA VEMOS CUANTO DURA EN CPU SEGUN cycles, waiting, quantum y S.

    // printf("ANTES:\n cycles %d, quantum %d, wait_left %d\n chosen %d, interrupted %d\n", current->cycles, queues[j]->quantum, current->wait_left, current->chosen, current->interrupted);

    current->chosen++;
    current->ready_time += (timer - current->first_ready);
    if(current->first_time == -1){
      current->first_time = timer;
    }

    // Chequear si cede CPU
    if (current->wait != 0){
      // Ver si el tiempo que ejecuta antes de ceder, es menor que el quantum
      // Se avanza ese tiempo en el timer
      if (current->wait_left < queues[j]->quantum){
        current->status="RUNNING";
        current->status="WAITING";
        // Si termina, sumar cycles a timer y S_passed
        if (current->cycles <= current->wait_left){
          timer += current->cycles;
          current->total_time_exe += current->cycles;
          S_passed += current->cycles;
          S_global += current->cycles;

          current->status="FINISHED";
          current->end_time = timer;
          current->cycles = 0;
          finished_p[finished] = current;
          finished++; 
          deleted =1;
        }
        // Si no, sumar wait_left, cede CPU
        else {
          timer += current->wait_left;
          current->total_time_exe += current->wait_left;
          S_passed += current->wait_left;
          S_global += current->wait_left;

          current->first_wait = timer;
          current->cycles -= current->wait_left;
          current->wait_left = current->wait; // wait left se reinicia
          // Si estaba en la cola de mayor prioridad, se queda ahí, sino
          // Subirle a la cola de mayor prioridad
          move = 2;
        }
      }

      // ELSE IF DE SI wait_left > quantum
      else if (current->wait_left > queues[j]->quantum){
        if (current->cycles <= queues[j]->quantum){ //cycles <= quantum < waiting
          timer += current->cycles;
          current->total_time_exe += current->cycles;
          S_passed += current->cycles;
          S_global += current->cycles;
          current->wait_left -= current->cycles;
          current->status="RUNNING";
          current->status="FINISHED";
  
          if (current->cycles == queues[j]->quantum){
            current->interrupted++;
          }
          current->cycles = 0;
          current->end_time = timer;
          finished_p[finished] = current;
          finished++; 
          deleted =1;
        }
        else if (current->cycles > queues[j]->quantum){  //quantum < waiting < cycles
          timer += queues[j]->quantum;
          current->total_time_exe += queues[j]->quantum;
          S_passed += queues[j]->quantum;
          S_global += queues[j]->quantum;
          current->wait_left -= queues[j]->quantum;  
          current->cycles -= queues[j]->quantum;
          current->interrupted++;
          move = 1;
        }
      }
      else { //ELSE DE SI quantum == wait_left
        timer += queues[j]->quantum;
        S_passed += queues[j]->quantum;
        S_global += queues[j]->quantum;
        current->total_time_exe += queues[j]->quantum;
        current->interrupted++;
        if(current->first_wait == -1){
          current->first_wait = timer;
        }
        // CHEQUEAR SI TERMINA PRIMERO!!
        if (queues[j]->quantum == current->cycles){
          current->status="FINISHED";
          current->cycles = 0;
          current->end_time = timer;
          finished_p[finished] = current;
          finished++; 
          deleted =1;
        }
        else {
          current->wait_left = current->wait; // se reinicia
          current->cycles = current->cycles - queues[j]->quantum;
          current->status = "WAITING";
          move = 1;
        }
      }
    }

    // DE AQUI EN ADELANTE SIGUE SI NO HACE WAITING
    // Si cycles es menor a quantum, ejecuta cycles y termina 
    else if (current->cycles <= queues[j]->quantum){
      // Avanzar reloj del sistema
      timer += current->cycles;
      S_passed += current->cycles;
      S_global += current->cycles;
      current->total_time_exe += current->cycles;
      current->status="RUNNING";
      current->status="FINISHED";
      current->end_time = timer;
      if (current->cycles == queues[j]->quantum){
        current->interrupted++;
      }
      current->cycles = 0;
      finished_p[finished] = current;
      finished++; 
      deleted =1;
    }

    // Sino, ejecuta quantum y lo mueven a cola inferior
    else {
      // Avanzar reloj del sistema
      timer += queues[j]->quantum;
      current->total_time_exe += queues[j]->quantum;
      S_passed += queues[j]->quantum;
      S_global += queues[j]->quantum;
      current->cycles = current->cycles - queues[j]->quantum;
      current->interrupted++;
      move = 1;
    }
    //TERMINÓ DE EJECUTARSEE!

    // printf("DESPUES:\n cycles %d, quantum %d, wait_left%d\n chosen %d, interrupted %d\n", current->cycles, queues[j]->quantum, current->wait_left, current->chosen, current->interrupted);
    //AQUI REVISAMOS SI DURANTE LA EJECUCION ALGUN PROCESO DEBIÓ HABER SALIDO DE WAITING
    queues = update_waiting(queues, Q, timer);
    Node* next_head = backup->head;
      if (next_head->value == 1){
        while(true){
           if (next_head->process->start_time <= timer){
            current_copy = append(queues[0], next_head->process);
            Node* next_head_copy = next_head;
            if (next_head->next != NULL){
              next_head = next_head->next;
              deleteNode(next_head_copy, backup);
            }
            else {
              deleteNode(next_head, backup);
              break;
            }
          }
          else {
            break;
          }
        }
      }
    //AQUI REVISAMOS SI DURANTE LA EJECUCION SE CUMPLIÓ EL TIEMPO S Y DEBEN SUBIR TODOS A LA COLA 0
    if (S_passed > S){
      queues = update_S(queues, Q, S, S_passed, backup_node);
      next_S = timer - S_passed + S;
      S_times++;
      S_passed = timer - S*S_times;
    }
    else if (S_passed == S){
      if (move == 1){
        if (j == Q - 1){
          current_copy = append(queues[j], current);
        } else {
          current_copy = append(queues[j+1], current);
        }
      }
      else if (move == 2){
        if (j != 0){ 
          current_copy = append(queues[j - 1], current);
        } else {
          current_copy = append(queues[0], current);
        }
      }
      move = 0;
      queues = update_S(queues, Q, S, S_passed, backup_node);
      S_times++;
      S_passed = 0;
      next_S += S;
    }

    if (move == 1){
      if (j == Q - 1){
        current_copy = append(queues[j], current);
      } else {
        current_copy = append(queues[j+1], current);
      }
    }
    else if (move == 2){
       if (j != 0){ 
        current_copy = append(queues[j - 1], current);
      } else {
        current_copy = append(queues[0], current);
      }
    }
  }

  // FIN SIMULACIÓN
  // printf("TIEMPO FINAL %d\n", timer);
  for (int w=0; w<total; w++){
    char** args_to_file = calloc(6, sizeof(char*));
    
    Process* pr = finished_p[w];

    char chosen[20];
    char interrupted[20];
    char turn_around[20];
    char response[20];
    char total_waiting[20];
    sprintf(chosen, "%d", pr->chosen);
    sprintf(interrupted, "%d", pr->interrupted);
    sprintf(turn_around, "%d", turnaround_time(pr));
    sprintf(response, "%d", response_time(pr));
    sprintf(total_waiting, "%d\n", turnaround_time(pr) - pr->total_time_exe);
    args_to_file[0] = pr->name;
    args_to_file[1] = chosen;
    args_to_file[2] = interrupted;
    args_to_file[3] = turn_around;
    args_to_file[4] = response;
    args_to_file[5] = total_waiting;    

    write_output(output_path, args_to_file);
    free(args_to_file);
  }
  free(backup_node);
  free(backup->head);
  free(backup);
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
