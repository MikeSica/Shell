#include "tokener.h"

int main(int argc, char **argv) {
    // Buffer to hold all user input
    char buff[256];

    // Read the entire input from stdin
    if (fgets(buff, sizeof(buff), stdin) == NULL) {
        // Return 1 if theres an error
        return 1;
    }

    // Tokenize the input by calling tokenize 
    char **tokens = tokenize(buff);

    // Print out each token
    for (int i = 0; tokens[i] != NULL; i++) {
        printf("%s\n", tokens[i]);
    }

    // Free the allocated memory
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);

    return 0;
}
