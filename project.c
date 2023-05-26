#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

volatile sig_atomic_t got_signal = 0;

enum {
    MAX_INSERT_SIZE = 100,
    MAX_ARGS_AMOUNT = 150,
};

char** CreateArgs(char mass[MAX_ARGS_AMOUNT][MAX_INSERT_SIZE], int start, int end) {
    char** array = (char**)calloc(end - start + 1, sizeof(char*));
    for (int i = start; i < end; ++i) {
        array[i - start] = (char*)calloc(strlen(mass[i]) + 1, sizeof(char));
        strcpy(array[i - start], mass[i]);
    }
    array[end - start] = NULL;
    return array;
}

void Handler(int sign) {
    got_signal = 1;
}

int main(int argc, char** argv) {
    sigaction(SIGUSR1,
     &(struct sigaction){
        .sa_handler = Handler,
        .sa_flags = SA_RESTART,
    },
      NULL);
      pid_t main_pid = getpid();
      pid_t pid = fork();
      if (!pid) {
        char mass[MAX_ARGS_AMOUNT][MAX_INSERT_SIZE];
        int count = 0;
        int progs_count = 1;
        while (scanf("%s", mass[count]) > 0) {
            if (mass[count][0] == '|') {
                ++progs_count;
            }
            ++count;
        }
        
        int** pipes = (int**)calloc(progs_count - 1, sizeof(int*));
        for (int i = 0; i < progs_count - 1; ++i) {
            pipes[i] = (int*)calloc(2, sizeof(int));
        }
        char*** progs_args = (char***)calloc(progs_count, sizeof(char**));
        int* progs_sizes = (int*)calloc(progs_count, sizeof(int));
        pid_t* array_pid = (pid_t*)calloc(progs_count, sizeof(pid_t));
        int current_arg = 0;
        
        if (progs_count > 1) {
            while (mass[current_arg][0] != '|') {
                ++current_arg;
            }
        } else {
            current_arg = count;
        }
        progs_args[0] = CreateArgs(mass, 0, current_arg);
        progs_sizes[0] = current_arg - 0;

        for (int i = 1; i < progs_count; ++i) {
            pipe(pipes[i - 1]);
            array_pid[i - 1] = fork();
            if (!array_pid[i - 1]) {
                if (i - 2 >= 0) {
                    close(pipes[i - 2][1]);
                    dup2(pipes[i - 2][0], STDIN_FILENO);
                    close(pipes[i - 2][0]);
                }
                dup2(pipes[i - 1][1], STDOUT_FILENO);
                close(pipes[i - 1][0]);
                close(pipes[i - 1][1]);
                execvp(progs_args[i - 1][0], progs_args[i - 1]);
                _exit(1);
            }
            if (i - 2 >= 0) {
                close(pipes[i - 2][0]);
                close(pipes[i - 2][1]);
            }
            if (i + 1 == progs_count) {
                continue;
            }
            ++current_arg;
            int start_arg = current_arg;
            while (mass[current_arg][0] != '|') {
                ++current_arg;
            }
            progs_args[i] = CreateArgs(mass, start_arg, current_arg);
            progs_sizes[i] = current_arg - start_arg;
        }
        if (progs_count == 2) {
            pipe(pipes[0]);
            array_pid[0] = fork();
            if (!array_pid[0]) {
                dup2(pipes[0][1], STDOUT_FILENO);
                close(pipes[0][0]);
                close(pipes[0][1]);
                execvp(progs_args[0][0], progs_args[0]);
                _exit(1);
            }
        }
        if (progs_count > 1) {

        
        ++current_arg;
        int start_arg = current_arg;
        current_arg = count;
        progs_args[progs_count - 1] = CreateArgs(mass, start_arg, current_arg);
        progs_sizes[progs_count - 1] = current_arg - start_arg;
        array_pid[progs_count - 1] = fork();
        if (!array_pid[progs_count - 1]) {
            if (progs_count - 2 >= 0) {
                close(pipes[progs_count - 2][1]);
                dup2(pipes[progs_count - 2][0], STDIN_FILENO);
                close(pipes[progs_count - 2][0]);
            }
            execvp(progs_args[progs_count - 1][0], progs_args[progs_count - 1]);
            _exit(1);
        }
        if (progs_count - 2 >= 0) {
            close(pipes[progs_count - 2][0]);
            close(pipes[progs_count - 2][1]);
        }
        }
        if (progs_count == 1) {
            array_pid[0] = fork();
            if (!array_pid[0]) {
                execvp(progs_args[0][0], progs_args[0]);
                _exit(1);
            }
        }
        
        
        kill(main_pid, SIGUSR1);
        for (int i = 0; i < progs_count; ++i) {
            waitpid(array_pid[i], NULL, 0);
        }

        for (int i = 0; i < progs_count - 1; ++i) {
            free(pipes[i]);
        }
        for (int i = 0; i < progs_count; ++i) {
            for (int j = 0; j < progs_sizes[i]; ++j) {
                free(progs_args[i][j]);
            }
            free(progs_args[i]);
        }
        free(array_pid);
        free(pipes);
        free(progs_args);
        free(progs_sizes);
        return 0;
    }
      while (!got_signal) {}
      sleep(5);
      kill(pid, SIGKILL);
    return 0;
}
