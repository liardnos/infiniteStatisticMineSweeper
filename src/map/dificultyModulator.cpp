#include "dificultyModulator.hpp"


// DificultyModulator
DificultyModulator::DificultyModulator(float mineRatio) :
    _mineRatio(mineRatio)
{}

void DificultyModulator::setMap(Map *map) {
    _map = map;
}

bool DificultyModulator::clickOn(int x, int y) {
    Cell * const &cell = _map->acess(x, y);
    if (rand()%100000 < cell->_proba*100000) {
        return 1;
    } else {
        return 0;
    }
}

float DificultyModulator::getProbaCellIsMine() {
    return _mineRatio;
}
//

// DificultyMax
DificultyMax::DificultyMax(float mineRatio) :
    DificultyModulator(mineRatio)
{}

bool DificultyMax::clickOn(int x, int y) {
    Cell * const &cell = _map->acess(x, y);
    if (0 > cell->_proba) {
        return 1;
    } else {
        return 0;
    }
}
//

// DificultyMin
DificultyMin::DificultyMin(float mineRatio) :
    DificultyModulator(mineRatio)
{}

bool DificultyMin::clickOn(int x, int y) {
    Cell * const &cell = _map->acess(x, y);
    if (1 == cell->_proba) {
        return 1;
    } else {
        return 0;
    }
}