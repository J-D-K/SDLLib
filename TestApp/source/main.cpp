#include "Game.hpp"
#include "sdl.hpp"

int main(void)
{
    romfsInit();

    Game game{};
    return game.run();
}