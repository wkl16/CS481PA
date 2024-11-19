#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "random437.h"
#define MAXWAITPEOPLE 800
#define SIMULATION_TIME 600

// Global variables
int waiting_line = 0;
int total_arrived = 0;
int total_rejected = 0;
int total_riders = 0;
int current_time_step = 0;
int num_cars, max_per_car;
sem_t semaphore_minute;
pthread_mutex_t lock;

void* handle_incoming(void* arg) {
  for (current_time_step = 0; current_time_step < SIMULATION_TIME;
       current_time_step++) {
    int meanArrival = (current_time_step < 120)   ? 25
                      : (current_time_step < 240) ? 45
                      : (current_time_step < 360) ? 35
                                                  : 25;

    int arrivals = poissonRandom(meanArrival);
    pthread_mutex_lock(&lock);

    total_arrived += arrivals;
    int rejected = 0;
    if (waiting_line + arrivals > MAXWAITPEOPLE) {
      rejected = (waiting_line + arrivals) - MAXWAITPEOPLE;
      arrivals -= rejected;
      total_rejected += rejected;
    }

    waiting_line += arrivals;

    printf("%03d arrive %d reject %d wait-line %d at %02d:%02d:%02d\n",
           current_time_step, arrivals, rejected, waiting_line,
           9 + (current_time_step / 60), current_time_step % 60, 0);

    pthread_mutex_unlock(&lock);
    sem_post(&semaphore_minute);
    usleep(100000);  // 0.1 sec
                     // sleep(1); 1 sec
  }
}

void* handle_ride(void* arg) {
    int id = *((int *) arg);
    while (current_time_step < SIMULATION_TIME) {
        // TODO:
        // I dont think the threads are syncornizing prperly
        sem_wait(&semaphore_minute);
        pthread_mutex_lock(&lock);

        int riders = (waiting_line >= max_per_car) ? max_per_car : waiting_line;
        total_riders += riders;
        waiting_line -= riders;

        printf("[%d] handling %d riders\n", id, riders);
        pthread_mutex_unlock(&lock);

        // TODO:
        // This probably isnt correct. This thread will sleep for most of the program
        // If the semaphore handles the syncronization, should this thread sleep at all
        // or simply wait a the semaphore
        usleep(6000000);  // 7 + 53 secs
                          // sleep(60);
    }

    pthread_mutex_unlock(&lock);

void print_usage(char *bin) {
    fprintf(stderr, "Usage: %s -N CARNUM -M MAXPERCAR\n", bin);
}

int main(int argc, char* argv[]) {
    int c, n, m;
    opterr = 0;

    while ((c = getopt(argc, argv, "N:M:")) != -1) {
        switch (c) {
            case 'N':
                n = atoi(optarg);
                if (n <= 0) {
                    fprintf(stderr, "Invalid argument for option -%c: %s.\n", c, optarg);
                    print_usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'M':
                m = atoi(optarg);
                if (m <= 0) {
                    fprintf(stderr, "Invalid argument for option -%c: %s.\n", c, optarg);
                    print_usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            case '?':
                if (optopt == 'F' || optopt == 'S') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option -%c.\n", optopt);
                }

                print_usage(argv[0]);
                exit(EXIT_FAILURE);

            default:
                exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++) {
        printf("Non-option argument %s.\n", argv[i]);
        print_usage(argv[0]);
    }

    if (n <= 0) {
        fprintf(stderr, "-N is a required argument.\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    if (m <= 0) {
        fprintf(stderr, "-M is a required argument.\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }


    num_cars = n;
    max_per_car = m;
    pthread_t incoming_thread;
    pthread_t ride_threads[num_cars];

    sem_init(&semaphore_minute, 0, 0);
    pthread_mutex_init(&lock, NULL);

    pthread_create(&incoming_thread, NULL, handle_incoming, NULL);
    int ids[num_cars];
    for (int i = 0; i < num_cars; i++) {
        ids[i] = i;
        pthread_create(&ride_threads[i], NULL, handle_ride, &ids[i]);
    }

    pthread_join(incoming_thread, NULL);
    for (int i = 0; i < num_cars; i++) {
        pthread_join(ride_threads[i], NULL);
    }

    printf("Total arrived: %d\n", total_arrived);
    printf("Total riders: %d\n", total_riders);
    printf("Total rejected: %d\n", total_rejected);
    // TODOs:
    // the average waiting time per person (in minutes)
    // the length of the line at its worst case, and the time of day at which that
    // occurs.

    sem_destroy(&semaphore_minute);

    return EXIT_SUCCESS;
}
