/// @file
/// Implementation of useful global variables.

#include <vb/Auto.h>
#include <vb/coo.h>

namespace vb {
    std::vector<Task> Auto::tasks;

    const std::vector<coo>  dz  = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1}};
    const std::vector<coo3> dz3 = {coo3(1, 0, 0), coo3(-1, 0, 0), coo3(0, 1, 0), coo3(0, -1, 0), coo3(0, 0, 1), coo3(0, 0, -1)};
} // namespace vb
