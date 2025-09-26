#include "spdlog/spdlog.h"

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::trace);

    spdlog::info("Welcome to spdlog with Bazel!");
    spdlog::warn("This is a warning message.");
    spdlog::error("This is an error message. Something went wrong!");

    spdlog::info("The answer is {}.", 42);

    return 0;
}
