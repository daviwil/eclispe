#include "./greatest.h"

TEST foo_should_foo(void) {
    PASS();
}

SUITE(suite) {
    RUN_TEST(foo_should_foo);
}

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite);

    GREATEST_MAIN_END();
}
