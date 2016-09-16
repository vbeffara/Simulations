#define SPDLOG_DEBUG_ON
#define SPDLOG_TRACE_ON
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // must be included
#include <iostream>

struct my_type { int i; };

template <typename OStream> OStream& operator<< (OStream& os, const my_type &c) {
    return os << "[my_type i=" << c.i << "]";
}

int main (int, char*[]) {
    auto console = spdlog::stdout_logger_mt ("console", true);
    spdlog::set_level(spdlog::level::trace); //Set global log level to info

    console->critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    console->error("An error message example {}..", 1);
    console->warn("Easy padding in numbers like {:08d}", 12);
    console->info("Welcome to spdlog!");
    console->debug("This message should be displayed..");
    console->trace("This message is a trace");

    console->info("Support for floats {:03.2f}", 1.23456);
    console->info("Positional args are {1} {0}..", "too", "supported");

    console->info("{:<30}", "left aligned");
    console->info("{:>30}", "right aligned");
    console->info("{:^30}", "centered");

    SPDLOG_TRACE(console, "Enabled only #ifdef SPDLOG_TRACE_ON: {} ,{}", 1, 3.23);
    SPDLOG_DEBUG(console, "Enabled only #ifdef SPDLOG_DEBUG_ON: {} ,{}", 1, 3.23);

    console->info("user defined type: {}", my_type { 14 });
}
