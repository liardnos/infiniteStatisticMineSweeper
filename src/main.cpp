
#include <string>
#include "map/map.hpp"
#include "map/sfml.hpp"
#include "map/dificultyModulator.hpp"

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    float dif = 0.6;
    if (argc == 2) {
        dif = std::stof(argv[1]);
    }
    DificultyModulator dificultyModulator(dif);
    Map map(dificultyModulator);

    #if SDL_DISPLAY
    Sdl2Display sfml(&map, 1920-20, 1080-20);
    #else
    SfmlDisplay sfml(&map, 1920-20, 1080-20);
    sfml._window->setPosition({0, 0});
    #endif
    map._sfml = &sfml;
    sfml.main_loop();
    return 0;
}