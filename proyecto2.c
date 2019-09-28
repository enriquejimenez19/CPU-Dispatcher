#define _GNU_SOURCE
#include <sys/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Required by for routine
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


#define DEBUG 1

void despachador();

TAILQ_HEAD(tailhead, Proceso) head;
TAILQ_HEAD(tailheadrealtime, Proceso) head_real_time;

struct Proceso{
    int id;
    int time_arrive;
    int priority;
    int processor_time;
    int printer;
    int modem;
    int dvd_blueray;
    int webcam;
    int cornet;
    TAILQ_ENTRY(Proceso) entries;
};

void add_to_proccess_queue(int id, int time_arrive, int priority, int processor_time, int printer, int modem, int dvd_blueray, int webcam, int cornet){

    struct Proceso *new_element;
    new_element = malloc(sizeof(struct Proceso));
    
    if (new_element) {
        new_element->id = id;
        new_element->time_arrive = time_arrive;
        new_element->priority = priority;
        new_element->processor_time = processor_time;
        new_element->printer = printer;
        new_element->modem = modem;
        new_element->dvd_blueray = dvd_blueray;
        new_element->webcam = webcam;
        new_element->cornet = cornet;
    }
    TAILQ_INSERT_TAIL(&head, new_element, entries);
}


void add_to_real_time_queue(struct Proceso *element){
     struct Proceso *new_element;
    new_element = malloc(sizeof(struct Proceso));
    
    if (new_element) {
        new_element->id = element->id;
        new_element->time_arrive = element->time_arrive;
        new_element->priority = element->priority;
        new_element->processor_time = element->processor_time;
        new_element->printer = element->printer;
        new_element->modem = element->modem;
        new_element->dvd_blueray = element->dvd_blueray;
        new_element->webcam = element->webcam;
        new_element->cornet = element->cornet;
    }

    TAILQ_INSERT_TAIL(&head_real_time, new_element, entries);
}

int main(void)
{   
    int time_arrive, priority, processor_time, printer, modem, dvd_blueray, webcam, cornet;
    int i, id = 1;
    int number;
    FILE * fp;
    char * line = NULL;
    char *token;
    size_t len = 0;
    ssize_t read;
    struct Proceso *element;
    element = malloc(sizeof(struct Proceso));
    //Leemos el archivo línea por línea, luego llamaremos una función para 
    fp = fopen("archivo.txt", "r");
    if (fp == NULL){
        exit(EXIT_FAILURE);
    }

    TAILQ_INIT(&head);
    TAILQ_INIT(&head_real_time);

    while ((read = getline(&line, &len, fp)) != -1) {
        token = strtok(line,",");
        i = 0;
        while(token != NULL){
            sscanf(token, "%d", &number);
            if(i == 0){
                time_arrive = number;
            }else if(i == 1){
                priority = number;
            }else if(i == 2){
                processor_time = number;
            }else if(i == 3){
                printer = number;
            }else if(i == 4){
                modem = number;
            }else if(i == 5){
                dvd_blueray = number;
            }else if(i == 6){
                webcam = number;
            }else if(i == 7){
                cornet = number;
            }
            i++;
            token = strtok(NULL,",");
        }
        add_to_proccess_queue(id,time_arrive, priority, processor_time, printer, modem, dvd_blueray, webcam, cornet);
        id++;
    }

    fclose(fp);
    despachador();
    
    if(DEBUG){
        while (head.tqh_first != NULL){
            element = head.tqh_first;
            printf("ID: %d - %d %d %d %d %d %d %d %d \n",element->id, element->time_arrive, element->priority, element->processor_time, element->printer, element->modem, element->dvd_blueray, element->webcam, element->cornet);
            TAILQ_REMOVE(&head, head.tqh_first, entries);
        }
    }
    
    
    
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}


void despachador(){
    
    char *child_time;
    struct Proceso *element;
    pid_t pID;
    int time_program = 0;
    element = malloc(sizeof(struct Proceso));
    while(head.tqh_first != NULL){

        element = head.tqh_first;
        if(element -> priority == 0){
            add_to_real_time_queue(element);
        }

        while( head_real_time.tqh_first != NULL){
            element = head.tqh_first;
            pID = fork();
            if(pID == 0){

                printf("Iniciando proceso ID: %d - %d %d %d %d %d %d %d %d \n",element->id, element->time_arrive, element->priority, element->processor_time, element->printer, element->modem, element->dvd_blueray, element->webcam, element->cornet);
                sprintf(child_time, "%d", element->processor_time);
                if(execlp("./child", "child", child_time, NULL) == -1){
                    perror("Error in exec;");
                }
                
                return;
            }else{
                wait(0);
                time_program += element -> processor_time;  
                TAILQ_REMOVE( &head_real_time, head_real_time.tqh_first, entries);
                printf("Terminado proceso con ID: %d \n", element->id);
                while(head.tqh_first != NULL){
                    
                }    
            }
        }

        TAILQ_REMOVE(&head, head.tqh_first, entries);
    }

}