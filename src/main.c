#include "include/logger/logger.h"
#include <stdio.h>

int main(void) {
    LOG_INFO("Application started");
    LOG_WARN("This is a warning with value: %d", 42);
    LOG_ERROR("Something went wrong: code %d", -1);
    return 0;
}
