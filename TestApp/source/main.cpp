#include "TestApp.hpp"
#include "sdl.hpp"

int main(void)
{
    romfsInit();

    TestApp testApp{};
    return testApp.run();
}