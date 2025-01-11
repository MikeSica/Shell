Hello!

Core Functionality of the Shell

1. Interactive Command Handling:
  - Continuously receives and processes commands from the user.
  - Tokenizes the input to construct an abstract syntax tree (AST) for command execution.

2. Process Management:
  - Uses Linux system calls like fork() and execvp() to create and manage processes.
  - Parent process waits for child processes to finish unless a background operation (&) is specified.
3. Built-in Commands:
  - cd: Changes the working directory.
  - exit: Exits the shell.


This shell supports the following operations:

> (Redirects standard output to a file)
< (Redirects standard input from a file.)
| (Connects the output of one command to the input of another)
& (Runs a command in the background)
&& (and)
|| (or)
; (do left, then right)

Also supports the cd and exit commands.

Installation
To install and run this program, use navigate into the folder in your finder than double click the shell executable. Otherwise you can navigate
into the folder on your terminal and then run ./shell
