
#include <string>
#include "map/map.hpp"
#include "map/sfml.hpp"

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    float dif = 0.15;
    if (argc == 2) {
        dif = std::stof(argv[1]);
    }
    Map map(dif);

    SfmlDisplay sfml(&map, 800, 800);
    map._sfml = &sfml;
    sfml.main_loop();

    std::cout << "END" << std::endl;
}