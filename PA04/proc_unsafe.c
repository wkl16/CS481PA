#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

typedef struct {
    int balance[2];
} Bank;

// Define global variables
Bank *bank;


/* Routine for thread execution */
void* make_transactions() {
    int i, j, tmp1, tmp2, rint;
    double dummy;

    for (i = 0; i < 100; i++) {
        rint = (rand() % 30) - 15;

        if (((tmp1 = bank->balance[0]) + rint) >= 0 &&
                ((tmp2 = bank->balance[1]) - rint) >= 0) {
            bank->balance[0] = tmp1 + rint;
            for (j = 0; j < rint * 1000; j++) {
                dummy = 2.345 * 8.765 / 1.234;
            }  // spend time on purpose
            bank->balance[1] = tmp2 - rint;
        }
    }

    return NULL;
}

int main(int argc, char** argv) {
    int shmid;
    char *shm;
    srand(getpid());

    // Create shared memory segment
    key_t key = ftok("proc_unsafe.c", 65);
    if ((shmid = shmget(key, sizeof(Bank), IPC_CREAT | 0666)) < 0) {
        perror("Failed to get shared memory segment");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory segment
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("Failed to attach shared memory segment");
        exit(EXIT_FAILURE);
    }

    // Initialize the bank
    bank = (Bank *) shm;
    bank->balance[0] = 100;
    bank->balance[1] = 100;

    // Print the initial state
    printf("Init balances A:%d + B:%d ==> %d!\n", 
            bank->balance[0], bank->balance[1], bank->balance[0] + bank->balance[1]);
    
    pid_t pid = fork();
    if (pid > 0) {
        // Parent process
        make_transactions();
    } else if (pid == 0) {
        // Child process
        make_transactions();
        shmdt(shm);
        exit(EXIT_SUCCESS);
    } else {
        // Fork failed
        perror("Failed to fork");
        exit(EXIT_FAILURE);
    }

    // Wait for child to finish
    wait(NULL);

    // Print the results
    printf("Let's check the balances A:%d + B:%d ==> %d ?= 200\n",
            bank->balance[0], bank->balance[1], bank->balance[0] + bank->balance[1]);

    // Cleanup
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);  // Remove shared memory segment
 
    return 0;
}
