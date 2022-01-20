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

#include "sfml.hpp"
#include "mySharedMutex.hpp"

class Cell {
public:
    enum CellType{
        MINE = 'X',
        EMPTY = 'O'
    };
    Cell(float dificulty, int x, int y);

    char getChar();

    int calcNearMine(Map &map);

    bool _discovered = false;
    CellType _type;
    bool _hasExplode = false; // user click on this bomb
    bool _flaged = false; // user flag this
    int _x;
    int _y;
    int _nearMine = 0; // nb of mine's neigbour
    float _proba = NAN; // proba to be a mine
    float _certitude = 0; // certitude of the proba (if == 1 and (_proba == 0 or _proba == 1) don't recalculate this cell)

    bool _brutalised = false; // debug // is curently behing brutalised
    int _bruteInc = -1; // debug

    unsigned char _updated = false; // debug // fading blue
    bool _mark = false; // debug // red mark on cell
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

class Map {
public:
    //Cell *acessRead(int x, int y);

    Cell *&acessWeak(int x, int y);
    Cell *&acess(int x, int y);
    Cell *&acess(Vector2i &vec);
    Map(float dificulty = 0.2);
    void init();


    ~Map();
    bool clickOnCell(int x, int y);
    void mineDisplacement(int x, int y, bool mine_here);


    bool validate(std::deque<Cell *> &list);
    
    Part *generatePartUndiscover(Cell *cell, Part *part, Cell *cell_o);
    Part *generatePart(Cell *cell, Part *part = 0);

    void reset();

    void brutetalizing(Part *part, int mirador_inc);
    void evaluatorlv2(Cell *cell);
    void estimatorlv2(int nb = 1);
    void estimatorCell(Vector2i vec);
    void estimator(int nb = -1);
    void generate(int nb = -1);
    void insertXMin();
    void insertXMax();
    void insertYMin();
    void insertYMax();
    void displayOnTerm();


    std::mutex _XYminmax_mutex;
        int _Xmin = -1;
        int _Ymin = -1;
        int _Xmax = 1;
        int _Ymax = 1;
        int _nbCellDiscovered = 0;
        int _list_size = 0;
        int _toEstimate_size = 0;
        int _toEstimatelv2_size = 0;

    std::mutex _list_mutex;
    std::deque<Vector2i> _list;
    std::deque<Vector2i> _toEstimate;
    std::deque<Vector2i> _toEstimatelv2;
    MySharedMutex _grid_mutex;

    //std::vector<std::vector<Cell *> *> _grid; //X Y
    std::unordered_map<uint64_t, Cell *> _mapGrid;

    float _dificulty;
    Cell *_center = 0;
    SfmlDisplay *_sfml;
};

//static jmp_buf env_signal;

#endif /* !MAP_HPP_ */
