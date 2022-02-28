#include "dificultyModulator.hpp"


// DificultyModulator
DificultyModulator::DificultyModulator() {}

void DificultyModulator::setMap(Map *map) {
    _map = map;
}

bool DificultyModulator::clickOn(int x, int y) {
    Cell * const &cell = _map->acess(x, y);
    if (cell->_proba > 0) {
        return 1;
    } else {
        return 0;
    }
}

float DificultyModulator::getProbaCellIsMine() {
    return 0.2;
}
//