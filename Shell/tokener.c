#include "tokener.h"

// Look at header file for function signatures/Comments
// I will include comments in my code but signatures are written strictly in headers

const char special[] = "<>|();\"";

int isSpecial(char ch) {
  // Loops over all special characters to determine a match
  for (size_t i = 0; i < strlen(special); i++) {
    if (ch == special[i]) {
      if (ch == '"') {
        // return 2 if quotes 
        return 2; 
      }
      // return 1 if special but not quotes
      return 1;
    }
  }
  // return 0 if no match
  return 0;
}

char *my_strdup(const char *s) {
    // Allocate +1 memory for the duplicate string to account for terminator
    char *copy = malloc(strlen(s) + 1);
    // Copy string if malloc was successful
    if (copy != NULL) {
        strcpy(copy, s); 
    }
    return copy;  
}


char **tokenize(char *input) {
  // Used to see if we are inside quotes
  int quoteStat = 0;
  // A buffer to store the current token being constructed
  char currentToken[256];
  // Index we are appending the current tokens characters on
  int tokenIndex = 0;
  // Initial number of tokens we can store, doesn't matter what it is tbh
  int max_tokens = 10;

  // Allocate initial token array
  char **tokens = malloc(max_tokens * sizeof(char*));
  // check for malloc failure
  if (tokens == NULL) {
    perror("malloc");
    exit(1);
  }

  // A count for the number of tokens we are at
  int token_count = 0; 

  // Iterate over the string over the string until \0
  for (int i = 0; input[i] != '\0'; i++) {
    char ch = input[i];

    // If we are inside quotes and the next character is a quote then append \0 to the current token and move to the next
    // If we are inside quotes and the next character isn't a quote just append it to the current token we are at
    if (quoteStat) {
      if (ch == '"') {
        quoteStat = 0;
        currentToken[tokenIndex] = '\0';
        tokens[token_count++] = my_strdup(currentToken);
        tokenIndex = 0;
      } else {
          currentToken[tokenIndex++] = ch;
        }
        // Goes to next if we aren't in quotes
    } else {
      // Checks to see if their is \n or \t characters because if you dont have this it appends it as if its a normal character
      // Treats it like a space if it is \n or \t
        if (ch == '\\') {
            i++;
            if (input[i] == 'n' || input[i] == 't') {
                currentToken[tokenIndex++] = ' ';
            } else {
                currentToken[tokenIndex++] = input[i];
            }
        }
        else {
        // Returns - 2 if quotes, 1 if other special char, 0 if not special
        int p = isSpecial(ch);

        if (p == 2) {
          quoteStat = 1;
        } else if (p == 1) {
          // If we have a previously built token than store the token then reset the token index and increment the count
          // Then we allocate memory for the special character, store the token and then increment the token count.
            if (tokenIndex > 0) {
              currentToken[tokenIndex] = '\0';
              tokens[token_count++] = my_strdup(currentToken);
              tokenIndex = 0;
            }

            tokens[token_count] = malloc(2 * sizeof(char));
            tokens[token_count][0] = ch;
            tokens[token_count][1] = '\0';
            token_count++;

            } else if (ch == ' ') {
              // Logic for spaces, can break tokens - thats what it does here
              if (tokenIndex > 0) {
                currentToken[tokenIndex] = '\0';
                tokens[token_count++] = my_strdup(currentToken);
                tokenIndex = 0;
              }
            } 
            else {
              // If none of the above append it to the current token
                currentToken[tokenIndex++] = ch;
              }
            // If we ran out of space than double the amount of space
            if (token_count >= max_tokens) {
              max_tokens *= 2;
              tokens = realloc(tokens, max_tokens * sizeof(char*));
              // check if there is an realloc failure
              if (tokens == NULL) {
                perror("realloc");
                exit(1);
              }
            }
          }
        }
    }

    // In case there is a token left, store it
    if (tokenIndex > 0) {
        currentToken[tokenIndex] = '\0';
        tokens[token_count++] = my_strdup(currentToken);
    }
    //Indicate the end of the array
    tokens[token_count] = NULL;
    return tokens;
}

