#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int balance[2];
} Bank;

// Define global variables
Bank bank = {{100, 100}};

/* Routine for thread execution */
void* make_transactions() {
    int i, j, tmp1, tmp2, rint;
    double dummy;

    for (i = 0; i < 100; i++) {
        rint = (rand() % 30) - 15;

        if (((tmp1 = bank.balance[0]) + rint) >= 0 &&
                ((tmp2 = bank.balance[1]) - rint) >= 0) {
            bank.balance[0] = tmp1 + rint;
            for (j = 0; j < rint * 1000; j++) {
                dummy = 2.345 * 8.765 / 1.234;
            }  // spend time on purpose
            bank.balance[1] = tmp2 - rint;
        }
    }

    return NULL;
}

int main(int argc, char** argv) {
    int i;
    void* voidptr = NULL;
    pthread_t tid[2];
    srand(getpid());

    // Print initial state
    printf("Init balances A:%d + b:%d ==> %d!\n", 
            bank.balance[0], bank.balance[1], bank.balance[0] + bank.balance[1]);

    // Create threads
    for (i = 0; i < 2; i++) {
        if (pthread_create(&tid[i], NULL, make_transactions, NULL)) {
            perror("Error in thread creating\n");
            return (1);
        }
    }

    // Join threads
    for (i = 0; i < 2; i++) {
        if (pthread_join(tid[i], (void*)&voidptr)) {
            perror("Error in thread joining\n");
            return (1);
        }
    }

    // Print results
    printf("Let's check the balances A:%d + b:%d ==> %d ?= 200\n",
            bank.balance[0], bank.balance[1], bank.balance[0] + bank.balance[1]);

    return 0;
}
