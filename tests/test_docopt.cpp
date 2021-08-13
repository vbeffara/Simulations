#include <docopt/docopt.h>
#include <fmt/ostream.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>

static const char USAGE[] = R"(Usage: DDLA [options]

Options:
  -f, --fill     Fill holes and build a CoarseImage
  -n <size>      Size of the simulation [default: 750]
  -p <drift>     Random walk parameter [default: 0.5]
  -v, --version  Print version number and exit)";

auto main(int argc, const char **argv) -> int {
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE, {argv + 1, argv + argc},
                                                               true,              // show help if requested
                                                               "Naval Fate 2.0"); // version string

    for (auto const &arg : args) spdlog::info("{} = {}", arg.first, arg.second);
    spdlog::info("Simulation size: {}", std::stoi(args["-n"].asString()));
    spdlog::info("Walk parameter: {}", std::stod(args["-p"].asString()));
}
