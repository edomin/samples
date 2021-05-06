#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "xmempool/xmempool.h"

typedef struct {
    char *data;
    bool  alive;
} test_t;

test_t *test_create(xmem_pool_handle pool) {
    test_t* test = (test_t *)xmem_alloc(pool);

    test->data = malloc(sizeof(char) * 10);

    test->alive = true;

    return test;
}

void test_destroy(test_t *test, xmem_pool_handle pool) {
    free(test->data);
    test->alive = false;
    xmem_free(pool, (char *)test);
}

int main(int argc, char **argv) {
    xmem_pool_handle pool = xmem_create_pool(sizeof(test_t));
    test_t          *test = test_create(pool);

    test_destroy(test, pool);

    printf("alive: %s\n", test->alive ? "true" : "false");

    xmem_destroy_pool(pool);

    return 0;
}
