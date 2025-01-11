#ifndef _TOKENER_H
#define _TOKENER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A const containing special characters we are looking for that help identify tokens.
extern const char special[];

// Function that returns if a character is special (including quote handling).
int isSpecial(char ch);

/**
* Creates a copy of the inputted string by allocating memory for the copy and copying the  
* content from the original string into it. It returns a pointer to the newly allocated copy of the string.
* Returns Null if the allocation fails.
*/
char *my_strdup(const char *s);

/**
* The tokenize function takes in a string as an input (char *) which it then parses
* into an array of tokens using criteria like special characters(dignified by the const char special),
* quotes and by managing white space correctly. Then it returns the array of tokens(char **).
 */ 
char **tokenize(char *input);

#endif