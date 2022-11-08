/*
** EPITECH PROJECT, 2021
** infiniteStatisticMineSweeper
** File description:
** map
*/

#ifndef MAP_HPP_
#define MAP_HPP_

#include <iostream>
#include <unordered_map>
#include <limits>
#include <stdlib.h>
#include <vector>
#include <cstdlib>
#include <deque>
#include <iomanip>
#include <unistd.h>
#include <math.h>
#include <mutex>
#include <shared_mutex>
#include <setjmp.h>

#if SDL_DISPLAY
#include "sdl2.hpp"
#else
#include "sfml.hpp"
#endif
#include "mySharedMutex.hpp"
#include "dificultyModulator.hpp"

class Cell {
public:
    enum CellType
    {
        MINE = 'X',
        EMPTY = 'O'
    };
    Cell(){};
    Cell(float dificulty, int x, int y);

    char getChar();

    int calcNearMine(Map &map);

    CellType _type; // type just for map generation
    bool _discovered = false;
    bool _flaged = false;     // user flag this
    bool _hasExplode = false; // user click on this bomb

    int _x;
    int _y;

    int _nearMine = 0;    // nb of mine's neigbour
    float _proba = NAN;   // proba to be a mine
    float _certitude = 0; // certitude of the proba (if == 1 and (_proba == 0 or _proba == 1) don't recalculate this cell)

    bool _brutalised = false; // debug // is curently behing brutalised
    int _bruteInc = -1;       // debug

    unsigned char _updated = false; // debug // fading blue
    bool _mark = false;             // debug // red mark on cell
};

struct Vector2i {
    Vector2i(int x, int y) : x(x), y(y) {}
    int x;
    int y;
};

struct Part {
    std::deque<Cell *> mirador;
    std::deque<Cell *> undiscover;
    int tot = 0;
    float certitude = 1;
};

class AMap {
    virtual Cell *&acessWeak(int x, int y) = 0;
    virtual Cell *&acess(int x, int y) = 0;
    virtual Cell *&acess(Vector2i &vec) = 0;

    virtual bool clickOnCell(int x, int y) = 0;
};

class DificultyModulator;

class Map : public AMap {
public:
    Map(DificultyModulator &dificultyModulator);
    void init();
    ~Map();

    bool loadFromFile(std::string const &filename);
    bool saveInFile(std::string const &filename);

    Cell *&acessWeak(int x, int y) override;
    Cell *&acess(int x, int y) override;
    Cell *&acess(Vector2i &vec) override;

    bool clickOnCell(int x, int y) override;

    bool validate(std::deque<Cell *> &list);

    Part *generatePartUndiscover(Cell *cell, Part *part, Cell *cell_o);
    Part *generatePart(Cell *cell, Part *part = 0);

    void reset();

    void brutetalizing(Part *part, int mirador_inc);
    void evaluatorlv2(Cell *cell);
    void estimatorlv2();
    void estimatorCell(Vector2i vec);
    void estimator();
    void generate();

    std::mutex _XYminmax_mutex;
    int _nbCellDiscovered = 0;
    int _list_size = 0;
    int _toEstimate_size = 0;
    int _toEstimatelv2_size = 0;

    bool _need_estimate = false;
    std::mutex _list_mutex;
    std::deque<Vector2i> _list;
    std::deque<Vector2i> _toEstimate;
    std::deque<Vector2i> _toEstimatelv2;
    MySharedMutex _grid_mutex;

    std::unordered_map<uint64_t, Cell *> _mapGrid;

    DificultyModulator &_dificulty;
    #if SDL_DISPLAY
    Sdl2Display *_sfml;
    #else
    SfmlDisplay *_sfml;
    #endif
};

//static jmp_buf env_signal;

#endif /* !MAP_HPP_ */
