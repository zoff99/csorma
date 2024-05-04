#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

char* generateRandomString() {
    char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789_";
    char* randomString = (char*)malloc(29 * sizeof(char));


    struct timeval time;
    gettimeofday(&time,NULL);
    // microsecond has 1 000 000
    // Assuming you did not need quite that accuracy
    // Also do not assume the system clock has that accuracy.
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    // srand(time(NULL));

    randomString[0] = charset[rand() % 26]; // First char is a-z
    for (int i = 1; i < 27; i++) {
        randomString[i] = charset[rand() % (36 + 1)]; // Random a-z, 0-9, _
    }
    randomString[27] = charset[rand() % 26]; // Last char is a-z
    randomString[28] = '\0'; // Null terminator

    return randomString;
}

int main() {
    char* randomChars = generateRandomString();
    printf("%s\n", randomChars);
    free(randomChars);

    return 0;
}
