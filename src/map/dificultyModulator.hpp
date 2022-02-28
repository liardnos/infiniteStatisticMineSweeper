#pragma once

#include "map.hpp"

class Map;

class DificultyModulator {
public:
    DificultyModulator();

    void setMap(Map *map);

    virtual bool clickOn(int x, int y);

    virtual float getProbaCellIsMine();

    Map *_map;
};