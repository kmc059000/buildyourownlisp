#include <stdio.h>

/* Declare  a buffer for user input of size 2048 */
static char input[2048];

int main(int argc, char** argv) {
    /* Print version and exit information */
    puts("BYOL Version 0.0.0.0.1");
    puts("Ctrl+C to exit\n");

    while (1) {
        fputs("byol> ", stdout);

        fgets(input, 2048, stdin);

        printf("Hello %s", input);
    }

    return 0;
}