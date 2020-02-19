void CauseSegfault(void) {
    int *p = (int*)0x12345678;
    *p = 0;
}

__attribute__((always_inline)) inline void foo(void) {
    CauseSegfault();
}

void bar(void) {
    foo();
}

__attribute__ ((visibility ("default"))) void baz(void) {
    bar();
}
