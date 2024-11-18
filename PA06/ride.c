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
  while (current_time_step < SIMULATION_TIME) {
    sem_wait(&semaphore_minute);
    pthread_mutex_lock(&lock);

    int riders = (waiting_line >= *((int*)arg)) ? *((int*)arg) : waiting_line;
    total_riders += riders;
    waiting_line -= riders;

    pthread_mutex_unlock(&lock);

    usleep(6000000);  // 7 + 53 secs
                      // sleep(60);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 5) {
    fprintf(stderr, "Usage: %s -N CARNUM -M MAXPERCAR\n", argv[0]);
    return EXIT_FAILURE;
  }

  int CARNUM = atoi(argv[2]);
  int MAXPERCAR = atoi(argv[4]);
  pthread_t incoming_thread;
  pthread_t ride_threads[CARNUM];

  sem_init(&semaphore_minute, 0, 0);
  pthread_mutex_init(&lock, NULL);

  pthread_create(&incoming_thread, NULL, handle_incoming, NULL);
  for (int i = 0; i < CARNUM; i++) {
    pthread_create(&ride_threads[i], NULL, handle_ride, &MAXPERCAR);
  }

  pthread_join(incoming_thread, NULL);
  for (int i = 0; i < CARNUM; i++) {
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
