/**
 * @file simulator.c
 * @author Oceania.eth
 * @date 2022-05-07
 * 
 * @brief main simulator program simulating how a disk scheduling algorithms works with POSIX Threads and Semaphores
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "scheduler.h"

struct Result {
    char method[10];
    int steps;
};

struct Args {
    int *reqs;
    struct Result *res;
    char method[10];

    sem_t *sem_end;       // semaphore for indicating end of a input cycle
    sem_t *sem_start;    // semaphore for indicating start of a input cycle
    sem_t *sem_read;    // semaphore for allowing read from buff2 from parent thread
    sem_t *sem_write;  // semaphore for allowing write to buff2 from child thread
    pthread_mutex_t *mutex_buff2;  // mutex for buff2
};

void *queue(void *arg) {

    // get the arguments
    struct Args *args = (struct Args *) arg;
    struct Result *res = args->res;

    // get the semaphores from the arguments
    sem_t *sem_end   = args->sem_end;
    sem_t *sem_start = args->sem_start;
    sem_t *sem_read  = args->sem_read;
    sem_t *sem_write = args->sem_write;
    pthread_mutex_t *mutex_buff2 = args->mutex_buff2;

    // keep handling requests until we get input size 0
    while(1) {
        // wait for the start signal
        sem_wait(sem_start);

        // get the number of requests from the buffer
        int n = 0;
        while(args->reqs[n] != -1) {
            n++;
        }

        // if we got input size 0, break
        if (n == 0) {
            //  printf("Thread %c has terminated.\n", args->id); (This use to print Thread Unique ID)
            // Print (Thread_ID) is terminated
            printf("Thread %ld is terminated.\n", pthread_self());
            pthread_exit(NULL);
        }

        // get the request from the buffer
        int *reqs = (int *) malloc(n * sizeof(int));

        memcpy(reqs, args->reqs, n * sizeof(int));

        // calculate the steps based on the method
        int steps = -1;

        // if method is equal to "FCFS" then run FCFS function 
        if (strcmp(args->method, "FCFS") == 0) {
            steps = FCFS(reqs, n);
        }
        // if method is equal to "SSTF" then run SSTF function 
        else if (strcmp(args->method, "SSTF") == 0) {
            steps = SSTF(reqs, n);
        }

        // if method is equal to "SCAN" then run SCAN function 
        else if (strcmp(args->method, "SCAN") == 0) {
            steps = SCAN(reqs, n);
        }
        
        // if method is equal to "C-SCAN" then run C-SCAN function 
        else if (strcmp(args->method, "C-SCAN") == 0) {
            steps = CSCAN(reqs, n);
        } 
        
        // if method is equal to "LOOK" then run LOOK function 
        else if (strcmp(args->method, "LOOK") == 0) {
            steps = LOOK(reqs, n);
        } 
        
        // if method is equal to "C-LOOK" then run C-LOOK function 
        else if (strcmp(args->method, "C-LOOK") == 0) {
            steps = CLOOK(reqs, n);
        }
        
        // set the result
        pthread_mutex_lock(mutex_buff2);

        // write to the buffer
        res->steps = steps;
        strcpy(res->method, args->method);

        // send the signal to the parent thread
        sem_post(sem_write);

        // wait for the signal from the parent thread
        sem_wait(sem_read);

        // release the mutex
        pthread_mutex_unlock(mutex_buff2);

        // destroy the mutex after using
        pthread_mutex_destroy(mutex_buff2);

        // free pointer (reqs) 
        free(reqs);

        // wait for the end signal
        sem_wait(sem_end);
    }   
}


int main() {

    int arr[100];
    int n = 0;

    // initialize all semaphores
    sem_t sem_end;
    sem_t sem_start;
    sem_t sem_read;
    sem_t sem_write;
    pthread_mutex_t mutex_buff2;

    sem_init(&sem_end, 0, 0);
    sem_init(&sem_start, 0, 0);
    sem_init(&sem_read, 0, 0);
    sem_init(&sem_write, 0, 0);
    pthread_mutex_init(&mutex_buff2, NULL);

    // pointer to an array of character
    char *names[] = {"FCFS", "SSTF", "SCAN", "C-SCAN", "LOOK", "C-LOOK"};
    int size = 6;
    int times[6];

    // initialize all threads
    pthread_t threads[6];

    struct Args arg[6];
    struct Result results[1];

    // initialize all arguments
    for(int i = 0; i < 6; i++) {
        arg[i].reqs = arr;
        arg[i].res = results;

        arg[i].sem_end = &sem_end;
        arg[i].sem_start = &sem_start;
        arg[i].sem_read = &sem_read;
        arg[i].sem_write = &sem_write;
        arg[i].mutex_buff2 = &mutex_buff2;

        // set the method
        strcpy(arg[i].method, names[i]);
    }
    
    // start all threads
    for (int i = 0; i < size; i++) {
        pthread_create(&threads[i], NULL, queue, &arg[i]);
    }
    
    char filename[100];

    printf("------------------------\n");
    printf("DISK SCHEDULER SIMULATOR\n");
    printf("------------------------\n");

    printf("Enter filename: ");
    scanf("%s", filename);

    // WHILE loop to ask for user input
    while(strcmp(filename, "QUIT") != 0) {

        FILE *fp = fopen(filename, "r");

        if (fp == NULL) {
            printf("File not found.\n");
            return 0;
        }

        n = 0;       

        // read integers until end of file
        while(!feof(fp)) {
            int temp;
            fscanf(fp, "%d", &temp);
            arr[n] = temp;
            n++;
        }
        arr[n] = -1;
        fclose(fp);

        // send start signal to all threads
        for (int i = 0; i < size; i++) {
            sem_post(&sem_start);
        }

        // wait for each thread to write to the buffer
        for (int i = 0; i < size; i++) {
            sem_wait(&sem_write);
            // match method name with buffer and store the steps
            for (int j = 0; j < size; j++) {
                // comparing method with names 
                if (strcmp(results[0].method, names[j]) == 0) {
                    times[j] = results[0].steps;
                }
            }
            sem_post(&sem_read);
        }

        // indicate end of input cycle
        for (int i = 0; i < size; i++) {
            sem_post(&sem_end);
        }

        // display results
        printf("For file (%s): \n", filename);

        for (int i = 0; i < size; i++) {
            printf("%s: %d\n", names[i], times[i]);
        }

        printf("------------------------------- \n");
        printf("    DISK SCHEDULER SIMULATOR    \n");
        printf("Type 'QUIT' to EXIT Application \n");
        printf("------------------------------- \n");

        printf("Enter filename: ");
        scanf("%s", filename);
    }

    // terminate all threads
    arr[0] = -1;
    for (int i = 0; i < size; i++) {
        sem_post(&sem_start);
    }

    for (int i = 0; i < size; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}


