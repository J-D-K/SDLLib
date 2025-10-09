#include "sdl.hpp"

#include <fstream>
#include <string_view>
#include <switch.h>

static constexpr sdl::Color BLACK = {0x000000FF};
static constexpr sdl::Color WHITE = {0xFFFFFFFF};

static constexpr std::string_view TEST_TEXT        = "Test Text here.";
static constexpr std::string_view INSTRUCTION_TEXT = "Press A to play the sound.";
static constexpr std::string_view EXIT_STRING      = "Press \uE0EF to exit.";

static void log(std::string_view text)
{
    std::ofstream logFile{"sdmc:/sdllib.log", std::ios::app};
    if (!logFile.is_open()) { return; }
    logFile << text << std::endl;
}

int main()
{
    if (!sdl::initialize("sdl test app", 1280, 720)) { return -1; }

    if (!sdl::text::initialize()) { return -2; }

    PadState gamePad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&gamePad);

    size_t testWidth              = sdl::text::get_width(18, TEST_TEXT);
    const std::string widthString = std::to_string(testWidth);

    sdl::SharedSound testSound = sdl::SoundManager::load("testSound", "sdmc:/test_sound.wav");
    while (appletMainLoop())
    {
        padUpdate(&gamePad);

        if (padGetButtonsDown(&gamePad) & HidNpadButton_A) { testSound->play(); }
        else if (padGetButtonsDown(&gamePad) & HidNpadButton_Plus) { break; }

        sdl::frame_begin(WHITE);

        int y = 32;
        sdl::text::render(sdl::Texture::Null, 32, y, 18, sdl::text::NO_WRAP, BLACK, TEST_TEXT);
        sdl::text::render(sdl::Texture::Null, 32, (y += 32), 18, sdl::text::NO_WRAP, BLACK, INSTRUCTION_TEXT);
        sdl::text::render(sdl::Texture::Null, 32, (y += 32), 18, sdl::text::NO_WRAP, BLACK, EXIT_STRING);
        sdl::text::render(sdl::Texture::Null, 32, (y += 32), 18, sdl::text::NO_WRAP, BLACK, widthString);
        sdl::frame_end();
    }
    sdl::text::exit();
    sdl::exit();
    return 0;
}
