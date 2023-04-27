#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_ARGS 10
#define MAX_CMD_LEN 100

struct Command {
    char* name;
    char* args[MAX_ARGS];
    char* input_file;
    char* output_file;
    int background;
    int num_args;
};

void handle_signal(int signal) {
    // ignore the signal
}

void execute_command(struct Command* cmd) {
    int in_fd = 0, out_fd = 0;
    pid_t pid;
    int pipefd[2];
    int status;
    int i, j;

    // check for input redirection
    if (cmd->input_file != NULL) {
        in_fd = open(cmd->input_file, O_RDONLY);
        if (in_fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
    }

    // check for output redirection
    if (cmd->output_file != NULL) {
        out_fd = open(cmd->output_file, O_WRONLY | O_CREAT | (cmd->background ? O_APPEND : O_TRUNC), 0644);
        if (out_fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);
    }

    // check for piping
    if (cmd->num_args > 1) {
        for (i = 0; i < cmd->num_args - 1; i++) {
            pipe(pipefd);
            pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0) {
                close(pipefd[0]);
                if (i > 0) {
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);
                }
                execvp(cmd->args[i], cmd->args + i);
                perror("execvp");
                exit(EXIT_FAILURE);
            }
            else {
                close(pipefd[1]);
                if (i == cmd->num_args - 2) {
                    dup2(pipefd[0], STDIN_FILENO);
                    close(pipefd[0]);
                }
            }
        }
    }

    // execute the final command
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        execvp(cmd->args[cmd->num_args - 1], cmd->args + cmd->num_args - 1);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else {
        if (!cmd->background) {
            waitpid(pid, &status, 0);
        }
    }
}

int parse_command(char* line, struct Command* cmd) {
    char* token;
    int i = 0;

    // check for background execution
    cmd->background = (line[strlen(line) - 2] == '&');

    // tokenize the command line
    token = strtok(line, " \n");
    while (token != NULL) {
        if (strcmp(token, ">") == 0 || strcmp(token, ">>") == 0) {
            // handle output redirection
            if (cmd->output_file != NULL) {
                fprintf(stderr, "syntax error: multiple output redirection\n");
                return 0;
            }
            cmd->output_file = strtok(NULL, " \n");
            if (cmd->output_file == NULL) {
                fprintf(stderr, "syntax error: missing output file\n");
                return 0;
            }
            if (strcmp(token, ">>") == 0) {
                cmd->background = 1;
            }
        }
        else if (strcmp(token, "<") == 0) {
            // handle input redirection
            if (cmd->input_file != NULL) {
                fprintf(stderr, "syntax error: multiple input redirection\n");
                return 0;
            }
            cmd->input_file = strtok(NULL, " \n");
            if (cmd->input_file == NULL) {
                fprintf(stderr, "syntax error: missing input file\n");
                return 0;
            }
        }
        else if (strcmp(token, "|") == 0) {
            // handle piping
            if (cmd->num_args >= MAX_ARGS) {
                fprintf(stderr, "syntax error: too many arguments\n");
                return 0;
            }
            cmd->args[cmd->num_args++] = NULL;
        }
        else {
            // add argument to the command
            if (cmd->num_args >= MAX_ARGS) {
                fprintf(stderr, "syntax error: too many arguments\n");
                return 0;
            }
            cmd->args[cmd->num_args++] = token;
        }
        token = strtok(NULL, " \n");
    }
    cmd->args[cmd->num_args] = NULL;

// check for empty command
    if (cmd->num_args == 0) {
        return 0;
    }

// check for built-in commands
    if (strcmp(cmd->args[0], "exit") == 0) {
        exit(EXIT_SUCCESS);
    }

    return 1;

}

int main() {
    char line[MAX_CMD_LEN];
    struct Command cmd;
    struct sigaction sigact;
    // set up the signal handler
    sigact.sa_handler = handle_signal;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGINT, &sigact, NULL);

    while (1) {
        // print prompt
        printf("stshell> ");
        fflush(stdout);

        // read command line
        if (fgets(line, sizeof(line), stdin) == NULL) {
            perror("fgets");
            exit(EXIT_FAILURE);
        }

        // parse command line
        if (!parse_command(line, &cmd)) {
            continue;
        }

        // execute command
        execute_command(&cmd);

        // reset command
        memset(&cmd, 0, sizeof(struct Command));
    }
}