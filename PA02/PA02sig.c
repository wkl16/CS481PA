#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t child_pid;
bool child_running = true;

// signal handler for SIGINT (Ctrl+C)
void handle_sigint() {
  printf("\nSIGINT caught, stopping child process %d...\n", child_pid);
  kill(child_pid, SIGKILL);
  wait(0);
  printf("child process killed, terminating parent process...\n");
  exit(EXIT_SUCCESS);
}

// signal handler for SIGTSTP (Ctrl+Z)
void handle_sigtstp() {
  const char *action = child_running ? "stopping" : "running";
  printf("\nSIGTSTP caught, %s child process %d...\n", action, child_pid);
  kill(child_pid, child_running ? SIGSTOP : SIGCONT);

  action = child_running ? "suspended" : "running";
  printf("child process now %s\n", action);

  child_running = !child_running;
}

int main(void) {
  // set up signal handlers for SIGINT and SIGTSTP
  struct sigaction sa_int, sa_tstp;

  sa_int.sa_handler = handle_sigint;
  sa_int.sa_flags = 0;
  sigemptyset(&sa_int.sa_mask);
  if (sigaction(SIGINT, &sa_int, NULL) == -1) {
    perror("error setting up SIGINT handler");
    exit(EXIT_FAILURE);
  }

  sa_tstp.sa_handler = handle_sigtstp;
  sa_tstp.sa_flags = 0;
  sigemptyset(&sa_tstp.sa_mask);
  if (sigaction(SIGTSTP, &sa_tstp, NULL) == -1) {
    perror("error setting up SIGTSTP handler");
    exit(EXIT_FAILURE);
  }

  // fork and execve to start the "yes" process in child
  child_pid = fork();
  if (child_pid == -1) {
    perror("error forking");
    exit(EXIT_FAILURE);
  } else if (child_pid == 0) {
    // child execute "yes"
    char *args[] = {"usr/bin/yes", NULL};
    execve("usr/bin/yes", args, NULL);
    perror("execve failed");  // if execve fails
    exit(EXIT_FAILURE);
  }

  // parent enter an infinite loop and wait for signals
  while (1) {
    pause();  // Wait for signals
  }
}
