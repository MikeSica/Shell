#include "shell.h"


const int globBuff = 256;
char *last_command = NULL;


void executeCommand(char **args) {
    pid_t pid = fork();
    // Check for forking error
    if (pid < 0) {
        perror("fork error");
    // GO into child process
    } else if (pid == 0) {
        // Execute command, if it returns -1 print error
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "%s: command not found\n", args[0]);
            exit(1);
        }
    } else {
        // wait for child to complete
        waitpid(pid, NULL, 0);
        // Empty output stream
        fflush(stdout);
    }
}


void inputRedir(char **args, char *input_file) {
    pid_t pid = fork();
    // Check for forking error
    if (pid < 0) {
        perror("fork error");
        // Child process
    } else if (pid == 0) {
        // Redirect standard input to the specified input file
        FILE *fp = freopen(input_file, "r", stdin);

        // If error happened opening file print error
        if (fp < 0) {
            perror("open error");
            exit(1);
        }
        // Execute the command with redirected input and print error if it returns -1
        if (execvp(args[0], args) == -1) {
            perror("execvp error");
            exit(1);
        }
    } else {
        // Wait for child process to finish up
        waitpid(pid, NULL, 0);
    }
}


void outputRedir(char **args, char *output_file) {
    pid_t pid = fork();
    // Check forking error
    if (pid < 0) {
        perror("fork error");
        // Child process
    } else if (pid == 0) {
        // Redirect standard output to the output file
        FILE *fp = freopen(output_file, "w", stdout);

        // If unsuccessful print error
        if (fp == NULL) {
            perror("freopen error");
            exit(1);
        }

        // Execute the command with redirected output
        if (execvp(args[0], args) == -1) {
            perror("execvp error");
            exit(1);
        }
    } else {
        // wait for child process
        waitpid(pid, NULL, 0);
    }
}

void pipeCommands(char *cmd) {
    char *commands[globBuff];
    int count = 0;

    // Tokenize the input command string into individual commands based on '|'
    char *command = strtok(cmd, "|");
    while (command != NULL && count < globBuff) {
        commands[count++] = command;
        command = strtok(NULL, "|");
    }

    // Create an array for the pipe file descriptors
    int pipes[2 * (count - 1)];
    for (int i = 0; i < count - 1; i++) {
        // Create a pipe for each pair of commands
        if (pipe(pipes + i * 2) < 0) {
            perror("pipe error");
            return;
        }
    }

    // Loop through each command to fork and execute
    for (int i = 0; i < count; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork error");
            return;
        } else if (pid == 0) {
            // Handle input redirection for the first command
            if (i == 0) {
                char *args[globBuff];
                int inRedir = 0;
                char *inFile = NULL;

                // Check for input redirection in the command
                char *token = strtok(commands[i], " ");
                int idx = 0;
                while (token != NULL) {
                    if (strcmp(token, "<") == 0) {
                        inRedir = 1;
                        token = strtok(NULL, " ");
                        if (token != NULL) {
                            inFile = token;
                        }
                    } else {
                        args[idx++] = token;
                    }
                    token = strtok(NULL, " ");
                }
                args[idx] = NULL;

                // Perform input redirection if specified
                if (inRedir && inFile) {
                    FILE *fp = freopen(inFile, "r", stdin);
                    if (fp == NULL) {
                        perror("freopen error");
                        exit(1);
                    }
                }

                // Handle piping for the first command
                if (count > 1) {
                    dup2(pipes[1], STDOUT_FILENO);
                }

                // Close all pipes in the child process
                for (int j = 0; j < 2 * (count - 1); j++) {
                    close(pipes[j]);
                }

                // Execute the command
                if (execvp(args[0], args) == -1) {
                    perror("execvp error");
                    exit(1);
                }
            } else if (i == count - 1) { // Handle output redirection for the last command
                char *args[globBuff];
                int outRedir = 0;
                char *outFile = NULL;

                // Check for output redirection in the command
                char *token = strtok(commands[i], " ");
                int idx = 0;
                while (token != NULL) {
                    if (strcmp(token, ">") == 0) {
                        outRedir = 1;
                        token = strtok(NULL, " ");
                        if (token != NULL) {
                            outFile = token;
                        }
                    } else {
                        args[idx++] = token;
                    }
                    token = strtok(NULL, " ");
                }
                args[idx] = NULL;

                // Perform output redirection if specified
                if (outRedir && outFile) {
                    FILE *fp = freopen(outFile, "w", stdout);
                    if (fp == NULL) {
                        perror("freopen error");
                        exit(1);
                    }
                }

                // Handle piping for the last command
                if (i > 0) {
                    dup2(pipes[(i - 1) * 2], STDIN_FILENO);
                }

                // Close all pipes in the child process
                for (int j = 0; j < 2 * (count - 1); j++) {
                    close(pipes[j]);
                }

                // Execute the command
                if (execvp(args[0], args) == -1) {
                    perror("execvp error");
                    exit(1);
                }
            } else { // Handle intermediate commands
                char **args = tokenize(commands[i]);

                // Redirect input and output appropriately
                dup2(pipes[(i - 1) * 2], STDIN_FILENO);
                dup2(pipes[i * 2 + 1], STDOUT_FILENO);

                // Close all pipes in the child process
                for (int j = 0; j < 2 * (count - 1); j++) {
                    close(pipes[j]);
                }

                // Execute the command
                if (execvp(args[0], args) == -1) {
                    perror("execvp error");
                    exit(1);
                }
            }
        }
    }

    // Close all pipes in the parent process
    for (int i = 0; i < 2 * (count - 1); i++) {
        close(pipes[i]);
    }

    // Wait for all child processes to complete
    for (int i = 0; i < count; i++) {
        wait(NULL);
    }
}



void execSequence(char *input) {
    // strstr simply returns first point to | and if there is one than go to pipeCommands
        if (strstr(input, "|") != NULL) {
        pipeCommands(input);
        return;
    }

    // Tokenize the input command string into commands based on ;
    char *command = strtok(input, ";");

    while (command != NULL) {
    char **args = tokenize(command);
    if (args[0] != NULL) {
        int inRedir = 0;
        int outRedir = 0;
        char *inFile = NULL;
        char *outFile = NULL;

         // Loop through the arguments to check for redirection symbols
        for (int i = 0; args[i] != NULL; i++) {
            if (strcmp(args[i], "<") == 0) {
                inRedir = 1;
                inFile = args[i + 1];
                args[i] = NULL;
            } else if (strcmp(args[i], ">") == 0) {
                outRedir = 1;
                outFile = args[i + 1];
                args[i] = NULL;
            }
        }

        if (inRedir) {
            inputRedir(args, inFile);
        } else if (outRedir) {
            outputRedir(args, outFile);
        } else {
            executeCommand(args);
        }
    }

    // Free tokenized args
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);

    // Get the next command
    command = strtok(NULL, ";");
}

}

// Change the directory to a given path
void changeDir(char *path) {
    if (chdir(path) != 0) {
        perror("cd error");
    }
}

// Display a help message
void helpMes() {
    printf("Built-in commands:\n");
    printf("cd - Change directory to the specified directory\n");
    printf("source - executes a script, takes a filename as an argument\n");
    printf("prev - Prints the previous command line and executes it again, without becomming the new command line.\n");
    printf("help - Shows this\n");
}

// Execute a series of commands from a file
void execSource(char *filename) {

    // Included this because some tests failed due to over 30sec, not sure why so changed permissions just in case
    if (chmod(filename, S_IRUSR | S_IRGRP | S_IROTH) < 0) {
        perror("chmod error");
        return;
    }
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("source error");
        return;
    }

    char buffer[globBuff];
    while (fgets(buffer, globBuff, file) != NULL) {
        // Remove newline character
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }
        execSequence(buffer);
    }
    fclose(file);
}

void helpEcho(char **args) {
    int interpret_escapes = 0; // Flag to indicate whether to interpret escape sequences (if we have -e)
    int start_index = 1;       // Start processing arguments from index 1

    // Check for the -e flag
    if (args[1] && strcmp(args[1], "-e") == 0) {
        interpret_escapes = 1;
        start_index = 2;
    }

    // Loop through the arguments and print them
    for (int i = start_index; args[i] != NULL; i++) {
        if (interpret_escapes) {
            // Interpret escape sequences
            for (char *p = args[i]; *p; p++) {
                if (*p == '\\') {
                    p++; // Look at the next character after '\'
                    switch (*p) {
                        case 'n':
                            putchar('\n');
                        break;
                        case 't':
                            putchar('\t');
                        break;
                        case '\\':
                            putchar('\\');
                        break;
                        default:
                                putchar('\\');
                        putchar(*p);
                        break;
                    }
                } else {
                    putchar(*p);
                }
            }
        } else {
            // Print the argument as is
            fputs(args[i], stdout);
        }

        // Print a space between arguments (except for the last one)
        if (args[i + 1] != NULL) {
            putchar(' ');
        }
    }
    putchar('\n'); // Print a newline at the end
}


int main(int argc, char **argv) {

    char buffer[globBuff];

    printf("Welcome to mini-shell.\n");

    while (1) {
        printf("shell $ ");
        fflush(stdout);

        // Get the User input
        if (fgets(buffer, globBuff, stdin) == NULL) {
            printf("Bye bye.\n");
            break;
        }

        // If final character is new line set it as \0 so we recognize it as a new input
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }

        // If input is exit then exit the program
        if (strncmp(buffer, "exit", 4) == 0 && buffer[4] == '\0') {
            printf("Bye bye.\n");
            break;
        }

        // if prev execute last command
        if (strcmp(buffer, "prev") == 0) {
            if (last_command) {
                execSequence(last_command);
            } else {
                printf("No previous command found.\n");
            }
            continue; 
        }



          // Store the command as last_command if it's not prev
        if (last_command) {
            free(last_command);
        }
        last_command = my_strdup(buffer);

        char **args = tokenize(buffer);

        if (args[0] != NULL) {
            // If cd go to changeDir
            if (strcmp(args[0], "cd") == 0) {
                changeDir(args[1]);
            // If source go to execSource
            } else if (strcmp(args[0], "source") == 0) {
                execSource(args[1]);
            // If help do the help command
            } else if (strcmp(args[0], "help") == 0) {
                helpMes();
            // If echo do the echo command
            } else if (strcmp(args[0], "echo") == 0) {
                helpEcho(args);
                continue;
            } else {
                // else call execSequence
                execSequence(buffer);
            }
        }
        // Free args
        for (int i = 0; args[i] != NULL; i++) {
            free(args[i]);
        }
        free(args);
    }
    // free last_command
    free(last_command);
    return 0;
}
