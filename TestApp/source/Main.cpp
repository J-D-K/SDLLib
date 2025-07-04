#include "sdl.hpp"
#include <switch.h>

static constexpr sdl::Color BLACK = {0x000000FF};
static constexpr sdl::Color WHITE = {0xFFFFFFFF};

int main()
{
    if (!sdl::initialize("sdl test app", 1280, 720))
    {
        return -1;
    }

    if (!sdl::text::initialize())
    {
        return -2;
    }

    PadState gamePad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&gamePad);

    size_t testWidth = sdl::text::get_width(18, "Test text");

    while (appletMainLoop())
    {
        padUpdate(&gamePad);

        if (padGetButtonsDown(&gamePad) & HidNpadButton_Plus)
        {
            break;
        }

        sdl::frame_begin(WHITE);
        sdl::text::render(NULL, 32, 32, 18, sdl::text::NO_TEXT_WRAP, BLACK, "Text here.\nPress \uE0EF to exit.");
        sdl::frame_end();
    }
    sdl::text::exit();
    sdl::exit();
    return 0;
}
