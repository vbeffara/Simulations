#define SPDLOG_DEBUG_ON
#define SPDLOG_TRACE_ON
#include <iostream>
#include <vb/Hub.h>

using namespace vb;

int main (int, char*[]) {
    spdlog::set_level(spdlog::level::trace); //Set global log level to info

    H.L->critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    H.L->error("An error message example {}..", 1);
    H.L->warn("Easy padding in numbers like {:08d}", 12);
    H.L->info("Support for floats {:03.2f}", 1.23456);
    H.L->debug("This message should be displayed..");
    H.L->trace("This message is a trace");
    H.L->info("Positional args are {1} {0}..", "too", "supported");
    H.L->info("{:<30}", "left aligned");
    H.L->info("{:>30}", "right aligned");
    H.L->info("{:^30}", "centered");

    SPDLOG_TRACE(H.L, "Enabled only #ifdef SPDLOG_TRACE_ON: {} ,{}", 1, 3.23);
    SPDLOG_DEBUG(H.L, "Enabled only #ifdef SPDLOG_DEBUG_ON: {} ,{}", 1, 3.23);
}
