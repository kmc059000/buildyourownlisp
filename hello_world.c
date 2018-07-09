#include <stdio.h>

void printNTimes(int times, char* text) {
    int i = 0;
    while(i < times) {
        puts(text);
        i++;
    }
}

int main(int argc, char** argv) {
    printNTimes(100, "Hello Kenneth");
    return 0;
}
