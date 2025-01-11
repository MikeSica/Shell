#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "tokener.h"

// Buffer size that was said to be a global constant
extern const int globBuff;
// Just a pointer to store the last executed command
extern char *last_command;

/**
 * A function that executes a single command specified in args by forking
 * and calling execvp, if execvp fails it returns an error, if not the output goes to stdout.
 * @param args: Array of arguments/tokens
 */
void executeCommand(char **args);


/**
 * Runs the command with the contents of the file replacing the standard input.
 * @param args: array of arguments/tokens
 * @param input_file the given file which replaces stdin
 */
void inputRedir(char **args, char *input_file);

/**
 * Runs the command as normal, but the standard output is captured in the given file.
 * @param args: array of arguments/tokens
 * @param output_file the given file which replaces stdout
 */
void outputRedir(char **args, char *output_file);

/**
 * Executes piped commands by splitting the string at the |
 * The command on the LHS is executed with its standard output
 * redirected to the standard input of the command on the RHS.
 * @param inpu: inputted commands
 */
void pipeCommands(char *input);

/**
 * Executes commands separated by ;
 * The command on the LHS is executed 
 * followed by the command on the RHS.
 */
void execSequence(char *input);

/**
 * Changes the current working directory to the specified path.
 * @param path: Path to change directory to
 */
void changeDir(char *path);

/**
 * Prints the help message listing available built in shell commands.
 */
void helpMes();

/**
 * Executes a series of commands from a file.
 * @param filename: Path to the file containing the commands to be executed
 */
void execSource(char *filename);

/**
 * Custom implementation of the echo command.
 * @param args: Array of strings containing the arguments for the echo command.
 */
void helpEcho(char **args);

#endif /* SHELL_H */
