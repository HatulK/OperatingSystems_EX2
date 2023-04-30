#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
/**
 * Handle the CTRL+C signal.
 * @param sig signal
 */
void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("\nCaught Ctrl+C, but not killing the shell.\n");
    }
}

/**
 * funciton which hanlde the user input. Split the input into an array of strings. a split is happening each time the inpuy is " " (space),
 * @param input - stdin input from the user.
 * @return an array of strings which each string in the array is a word that been originally seperated which a " ".
 */
char **parse_input(char *input) {
    int bufsize = 64, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;
    if (!tokens) {
        fprintf(stderr, "stshell: allocation error\n");
        exit(EXIT_FAILURE);
    }
    token = strtok(input, " ");
    while (token != NULL) {
        tokens[position] = token;
        position++;
        if (position >= bufsize) {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "stshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, " ");
    }
    tokens[position] = NULL;
    return tokens;
}

/**
 * Function used to handle commands using piping.
 * @param args1
 * @param args2
 */
void run_pipe_command(char **args1, char **args2) {
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) == -1) {
        perror("stshell");
        exit(EXIT_FAILURE);
    }
    p1 = fork();
    if (p1 == 0) {
        // Child process 1
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(args1[0], args1) == -1) {
            perror("stshell");
            exit(EXIT_FAILURE);
        }
    } else if (p1 < 0) {
        perror("stshell");
        exit(EXIT_FAILURE);
    }

    p2 = fork();
    if (p2 == 0) {
        // Child process 2
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        if (execvp(args2[0], args2) == -1) {
            perror("stshell");
            exit(EXIT_FAILURE);
        }
    } else if (p2 < 0) {
        perror("stshell");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
}


/**
 * Handle the operators "<" and "<<".
 * @param args
 */
void handle_redirection(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0) {
            FILE *file;
            if (args[i + 1] != NULL) {
                if (strcmp(args[i], ">") == 0) {
                    file = freopen(args[i + 1], "w", stdout);
                } else {
                    file = freopen(args[i + 1], "a", stdout);
                }
                if (file == NULL) {
                    perror("stshell");
                    exit(EXIT_FAILURE);
                }
                args[i] = NULL;
            } else {
                fprintf(stderr, "stshell: expected file after '%s'\n", args[i]);
                exit(EXIT_FAILURE);
            }
            break;
        }
    }
}

/**
 * a function which handle a single command and run it. Fork the main proccess and call the function "handle_redirection".
 * @param args
 */
void run_single_command(char **args) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        handle_redirection(args);

        // Execute the command
        if (execvp(args[0], args) == -1) {
            perror("stshell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Fork error
        perror("stshell");
    } else {
        // Parent process
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

/**
 * a function which manage commands. if the command is include piping it sends it to the appropriate method,run_command_pipe, else send the command to a single command functions.
 * if the input is empty returning null.
 * if the input is "exit"  terminate the program.
 * @param args
 */
void run_command(char **args) {
    if (args[0] == NULL) {
        // An empty command was entered
        return;
    }

    if (strcmp(args[0], "exit") == 0) {
        exit(EXIT_SUCCESS);
    }

    char **args_pipe = NULL;
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) {
            args[i] = NULL;
            args_pipe = &args[i + 1];
            break;
        }
    }

    if (args_pipe) {
        run_pipe_command(args, args_pipe);
    } else {
        run_single_command(args);
    }
}

int main() {
    char input[1024];
    char **args;
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    while (1) {
        printf("314993247 shell > ");
        fgets(input, 1024, stdin);
        if (feof(stdin)) {
            printf("\n");
            exit(0);
        }
        input[strcspn(input, "\n")] = 0;
        args = parse_input(input);
        run_command(args);
    }
}
