#include "Birdy3d.hpp"
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>

using namespace Birdy3d;

int main(int argc, char** argv)
{
    doctest::Context context;

    context.applyCommandLine(argc, argv);

    if (!core::Application::init("Birdy3d tests", 1280, 720, "gruvbox-dark.json")) {
        return -1;
    }

    int res = context.run();

    core::Application::cleanup();

    return res;
}

namespace std {

    std::ostream& operator<<(std::ostream& os, std::u32string const& value)
    {
        os << utils::Unicode::utf32_to_utf8(value);
        return os;
    }

}
