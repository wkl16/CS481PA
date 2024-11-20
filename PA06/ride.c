#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "random437.h"
#include "queue.h"
#define MAXWAITPEOPLE 800
#define SIMULATION_TIME 600

// Global variables
int waiting_line = 0;
int total_arrived = 0;
int total_rejected = 0;
int total_riders = 0;
int current_time_step = 0;
int total_wait_time = 0;
int num_cars, max_per_car;
sem_t *sems;
pthread_mutex_t lock;
Queue *queue;


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

        // TODO:
        // I dont think this prints the correct arrivals
        // The arrivals should include all the people who arrive
        // including those who are rejected
        printf("%03d arrive %d reject %d wait-line %d at %02d:%02d:%02d\n",
                current_time_step, arrivals, rejected, waiting_line,
                9 + (current_time_step / 60), current_time_step % 60, 0);

        pthread_mutex_unlock(&lock);

        // Signal all thread semaphores
        for (int i = 0; i < num_cars; i++) {
            sem_post(&sems[i]);
        }
        usleep(1e5);  // 0.1 sec
    }
    // Signal all thread semaphores
    for (int i = 0; i < num_cars; i++) {
        sem_post(&sems[i]);
    }

    return NULL;
}

void* handle_ride(void* arg) {
    int id = *((int *) arg);
    while (current_time_step < SIMULATION_TIME) {
        // Wait at this thread's semaphore
        sem_wait(&sems[id]);
        pthread_mutex_lock(&lock);

        int riders = (waiting_line >= max_per_car) ? max_per_car : waiting_line;
        total_riders += riders;
        waiting_line -= riders;

#ifdef DEBUG
        printf("[%d] handling %d riders\n", id, riders);
#endif
        for (int i=0; i < riders; i++) {
            int step = dequeue(queue);
#ifdef DEBUG
            int wait_time = current_time_step - step;
            printf("  waited for %d minutes\n", wait_time);
#endif
            total_wait_time += current_time_step - step;
        }

        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

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
    sems = malloc(num_cars * sizeof(sem_t));
    for (int i=0; i < num_cars; i++) {
        sem_init(&sems[i], 0, 0);
    }
    pthread_mutex_init(&lock, NULL);

    queue = new_queue(MAXWAITPEOPLE);
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

    float average_wait_time = ((float) total_wait_time) / total_riders;
    printf("Total arrived: %d\n", total_arrived);
    printf("Total riders: %d\n", total_riders);
    printf("Total rejected: %d\n", total_rejected);
    // TODOs:
    // the average waiting time per person (in minutes)
    // the length of the line at its worst case, and the time of day at which that
    // occurs.
    printf("Average wait time: %.2f\n", average_wait_time);

    for (int i=0; i < num_cars; i++) {
        sem_destroy(&sems[i]);
    }

    free_queue(queue);

    return EXIT_SUCCESS;
}