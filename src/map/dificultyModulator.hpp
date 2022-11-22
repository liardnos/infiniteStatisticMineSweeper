#pragma once

#include "map.hpp"

class Map;

class DificultyModulator {
public:
    DificultyModulator(float mineRatio = 0.2);

    void setMap(Map *map);

    virtual bool clickOn(int x, int y);

    virtual float getProbaCellIsMine();

    float _mineRatio;
    Map *_map;
};


class DificultyMax : public DificultyModulator {
public:
    DificultyMax(float mineRatio = 0.2);

    virtual bool clickOn(int x, int y) override;

};

class DificultyMin : public DificultyModulator {
public:
    DificultyMin(float mineRatio = 0.2);

    virtual bool clickOn(int x, int y) override;

};