#include "stdio.h"

typedef struct {
    char *text;
    int  *numeric;
} test_t;

int main(int argc, char **argv) {
    test_t test_struct = {
        "test",
        (int[]){0, 1, 2, 3},
    };

    printf("sizeof(test_t): %zu\n", sizeof(test_t));
    printf("test_struct->text: %s\n", test_struct.text);
    printf("test_struct->numeric[1]: %i\n", test_struct.numeric[1]);

    return 0;
}
