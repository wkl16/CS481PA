#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

struct Bank {
  int balance[2];
};

void MakeTransactions(struct Bank* Bank) {
  int i, j, tmp1, tmp2, rint;
  double dummy;
  for (i = 0; i < 100; i++) {
    rint = (rand() % 30) - 15;
    if (((tmp1 = Bank->balance[0]) + rint) >= 0 &&
        ((tmp2 = Bank->balance[1]) - rint) >= 0) {
      Bank->balance[0] = tmp1 + rint;
      for (j = 0; j < rint * 1000; j++) {
        dummy = 2.345 * 8.765 / 1.234;
      }  // spend time on purpose
      Bank->balance[1] = tmp2 - rint;
    }
  }
}

int main(int argc, char** argv) {
  key_t key = ftok("proc_race.c", 65);
  int shmid = shmget(key, sizeof(struct Bank), IPC_CREAT | 0666);
  if (shmid < 0) {
    perror("shmget failed");
    exit(EXIT_FAILURE);
  }

  struct Bank* Bank = (struct Bank*)shmat(shmid, NULL, 0);
  if (Bank == (void*)-1) {
    perror("shmat failed");
    exit(EXIT_FAILURE);
  }

  // Initialize bank balances
  Bank->balance[0] = 100;
  Bank->balance[1] = 100;

  srand(getpid());
  printf("Init balances A:%d + B:%d ==> %d!\n", Bank->balance[0],
         Bank->balance[1], Bank->balance[0] + Bank->balance[1]);

  pid_t pid = fork();
  if (pid < 0) {
    perror("Fork failed");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {  // Child process
    srand(getpid());      // Different seed for child
    MakeTransactions(Bank);
    shmdt(Bank);
    exit(0);
  } else {  // Parent process
    MakeTransactions(Bank);
    wait(NULL);  // Wait for child to finish

    printf("Let's check the balances A:%d + B:%d ==> %d ?= 200\n",
           Bank->balance[0], Bank->balance[1],
           Bank->balance[0] + Bank->balance[1]);

    shmdt(Bank);
    shmctl(shmid, IPC_RMID, NULL);  // Remove shared memory segment
  }
  return 0;
}