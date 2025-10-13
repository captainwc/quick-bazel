#include "logger.h"

#include "fmt/format.h"

void Log(std::string s) {
    fmt::println("From fmt logger: {}", s);
}
