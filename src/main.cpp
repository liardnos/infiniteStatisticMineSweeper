
#include <string>
#include "map/map.hpp"
#include "map/sfml.hpp"
#include "map/dificultyModulator.hpp"

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    float dif = 0.2;
    if (argc == 2) {
        dif = std::stof(argv[1]);
    }
    DificultyModulator dificultyModulator;
    Map map(dificultyModulator);

    SfmlDisplay sfml(&map, 1920-20, 1080-20);
    sfml._window->setPosition({0, 0});
    map._sfml = &sfml;
    sfml.main_loop();
    return 0;
}