#include <string>

namespace vb {
    auto version() -> std::string { return GIT_SHA1; }
} // namespace vb
