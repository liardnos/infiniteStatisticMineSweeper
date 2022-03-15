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


class DificultyMax : public DificultyModulator {
public:
    DificultyMax();

    virtual bool clickOn(int x, int y) override;

};

class DificultyMin : public DificultyModulator {
public:
    DificultyMin();

    virtual bool clickOn(int x, int y) override;

};