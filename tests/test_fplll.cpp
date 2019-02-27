#include <vb/NumberTheory.h>
#include <vb/util/Hub.h>

using namespace vb;
using namespace std;

int main(int argc, char **argv) {
    Hub H("Testing various MP choices", argc, argv);
    real_t::default_precision(100);
    complex_t::default_precision(100);

    vector<real_t> xs;
    xs.emplace_back("0.9162918442410306144165008200767499077603397502333144975769802641182380808885019256331544308341889255");
    xs.emplace_back("1.36602540378443864676372317075293618347140262690519031402790348972596650845440001854057309338");
    xs.emplace_back("0.5877852522924731291687059546390727685976524376431459910722724807572784741623519575085040499");
    xs.emplace_back("0.8090169943749474241022934171828190588601545899028814310677243113526302314094512248536036021");
    xs.emplace_back("798303.3673469387755102040816326530612244897959183673469387755102040816326530612244897959183673469");
    xs.emplace_back("1.12266701157648291040117446489401135452719640061624479423083713099312644668872851836902659616");
    xs.emplace_back("0.63827397417446081629048447976042972714028217652392199657870122677085361940416547100605619666");
    xs.emplace_back("646.57075744998934067917908899466389773483433056006707491873238242675958808933605915556193840685637786");

    for (const auto &x : xs) {
        spdlog::info("x = {}", x);
        if (auto P = guess(real_t{x}, 80)) spdlog::info("    P(z) = {}", *P);
    }

    real_t    r("0.1722882583776278670500267959231284336682007863854856624427574750255049273322927690638923632");
    real_t    i("0.5302487364574217190358808797265653491226567421626168710631761419479819886565504921987031543");
    complex_t z{r, i};
    spdlog::info("x = {}", z);
    if (auto P = guess(z, 80)) spdlog::info("    P(z) = {}", *P);
}
