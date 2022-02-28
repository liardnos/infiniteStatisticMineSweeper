/*
** EPITECH PROJECT, 2021
** infiniteStatisticMineSweeper
** File description:
** map
*/

#include <iostream>
#include <limits>
#include <stdlib.h>
#include <vector>
#include <cstdlib>
#include <deque>
#include <iomanip>
#include <unistd.h>
#include <math.h>
#include <chrono>
#include <ctime>  
#include <string.h>  
#include <fstream>  

#include "sfml.hpp"
#include "map.hpp"

Cell::Cell(float dificulty, int x, int y) {
    float a = ((float)rand()) / RAND_MAX;
    _type = (dificulty > a) ? MINE : EMPTY;
    _x = x;
    _y = y;
    _proba = dificulty;
};  

char Cell::getChar(){
    //if (_type == CellType::MINE)
    //    return 'X';
    if (_discovered == false)
        return '-';
    return '0' + _nearMine;
}

int Cell::calcNearMine(Map &map){
    int count = 0;
        for (int x = -1; x <= 1; ++x)
            for (int y = -1; y <= 1; ++y) {
                Cell *cell = map.acess(_x+x, _y+y);
                if (cell->_proba == 1)
                    count++;
                else if (cell->_proba == 0);
                else if (cell->_type == Cell::CellType::MINE)
                    count++;
            }
    _nearMine = count;
    return _nearMine;
}

Cell *&Map::acessWeak(int x, int y) {
    int pos2[2] = {x, y};
    uint64_t pos = *(uint64_t*)pos2;
    return _mapGrid[pos];
}

Cell *&Map::acess(int x, int y) {
    Vector2i vec = {x, y};
    return acess(vec);
}

Cell *&Map::acess(Vector2i &vec) {

    int pos2[2] = {vec.x, vec.y};
    uint64_t pos = *(uint64_t*)pos2;
    Cell *&ptr = _mapGrid[pos];

    if (!ptr)
        ptr = new Cell(_dificulty.getProbaCellIsMine(), vec.x, vec.y);
    return ptr;
}

Map::Map(DificultyModulator &dificultyModulator) :
    _dificulty(dificultyModulator)
{
    _dificulty.setMap(this);
    this->init();
}

void Map::init() {
    _nbCellDiscovered = 0;
    _list_size = 0;
    _toEstimate_size = 0;
    _toEstimatelv2_size = 0;

    for (std::pair<uint64_t, Cell *> cell : _mapGrid) {
        delete cell.second;
    }
    for (int x = -3; x <= 3; ++x)
        for (int y = -3; y <= 3; ++y) {
            Cell *cell = acess(x, y);
            cell->_proba = 0;
            cell->_type = Cell::CellType::EMPTY;
            cell->_certitude = 1;
            _list_mutex.lock();
            _list.emplace_back(x, y);
            _list_mutex.unlock();
        }
}

Map::~Map() {
    for (std::pair<uint64_t, Cell *> cell : _mapGrid)
        delete cell.second;
}

bool Map::loadFromFile(std::string const &filename) {
    std::cout << "loadFromFile" << std::endl;
    _grid_mutex.lock_read();
    std::ifstream file(filename);
    if (!file.is_open())
        return false;

    file.read((char *)&_nbCellDiscovered, sizeof(_nbCellDiscovered));
    //file.read((char *)&_dificulty, sizeof(_dificulty));


    size_t size;
    file.read((char *)&size, sizeof(size));
    for (std::pair<uint64_t, Cell *> cell : _mapGrid)
        if (cell.second)
            delete cell.second;
    _mapGrid.clear();
    std::cout << size << std::endl;
    for (; size; size--) {
        Cell *cell = new Cell;
        file.read((char *)cell, sizeof(*cell));
        acess(cell->_x, cell->_y) = cell;
    }
    _grid_mutex.unlock_read();
    return true;
}

bool Map::saveInFile(std::string const &filename) {
    std::cout << "saveInFile" << std::endl;
    _grid_mutex.lock_read();
    std::ofstream file(filename);

    file.write((char *)&_nbCellDiscovered, sizeof(_nbCellDiscovered));
    //file.write((char *)&_dificulty, sizeof(_dificulty));
    
    if (!file.is_open())
        return false;
    size_t size = 0;
    for (auto p : _mapGrid)
        if (p.second)
            size++;
    std::cout << size << std::endl;
    file.write((char *)&size, sizeof(size));
    for (auto p : _mapGrid) {
        Cell *cell = p.second;
        if (cell)
            file.write((char *)cell, sizeof(*cell));
    }   
    _grid_mutex.unlock_read();
    return true;
}

bool Map::clickOnCell(int x, int y){
    bool ret = false;
    std::cout << "click" << std::endl;
    while (1) {
        {
            std::lock_guard<std::mutex> lock(_list_mutex);
            if (!_need_estimate) {
                _need_estimate = true;
                break;
            }
        }
        std::cout << "wait" << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds((int)1E6/60));
    }
    std::cout << "go" << std::endl;
    _grid_mutex.lock_read();
    Cell * const &cell = acess(x, y);
    
    // statistic displacement way
    if (cell->_proba > 0) {
        // lose
        ret = true;
        cell->_hasExplode = true;
        cell->_proba = 1;
        cell->_type = Cell::CellType::MINE;
        cell->_certitude = 1;

        // displace mines acording to this
        for (int x = -1; x <= 1; ++x)
            for (int y = -1; y <= 1; ++y)
                estimatorCell({cell->_x+x, cell->_y+y});
        //
    } else {
        cell->_type = Cell::CellType::EMPTY;
        _list_mutex.lock();
        _list.push_front(Vector2i(x, y));
        _list_mutex.unlock();
    }
    //
    _grid_mutex.unlock_read();
    return ret;
}

bool Map::validate(std::deque<Cell *> &list) {
    for (Cell *cell : list){
        int count = 0;
        int risk = 0;
        int certainMine = 0;
        float minCertitude = 1;
        for (int x = -1; x <= 1; ++x)
            for (int y = -1; y <= 1; ++y){
                _grid_mutex.lock_read();
                Cell * const cell2 = acess(cell->_x+x, cell->_y+y);
                if (cell2->_discovered == false){
                    minCertitude = std::min(minCertitude, cell2->_certitude);
                    risk++;
                    if (cell2->_proba != 0){
                        count++;
                        if (cell2->_proba == 1)
                            certainMine++;
                    }
                }
                _grid_mutex.unlock_read();

            }
        if (cell->_nearMine != minCertitude){
            return false;
        }
    }
    return true;
}


struct bPStruct {
    bPStruct(Cell *cell, float d) : 
        cell(cell), d(d)
    {}
    Cell *cell;
    float d;
};

Part *Map::generatePartUndiscover(Cell *cell, Part *part, Cell *cell_o) {
    std::vector<bPStruct> tab;
    part->undiscover.push_back(cell);
    cell->_brutalised = true;
    if ((cell->_proba == 1 || cell->_proba == 0))
        return part;
    cell->_certitude = 0;
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y){
            Cell *cell2 = acess(cell->_x+x, cell->_y+y);
            if (cell2->_brutalised == false){
                if (cell2->_discovered == true){
                    tab.push_back(bPStruct(cell2, (float)abs(cell_o->_x - cell2->_x) + abs(cell_o->_y - cell2->_y)));
                }
            }
        }
    std::sort(tab.begin(), tab.end(), [](bPStruct &a, bPStruct &b){return a.d <= b.d;});
    for (bPStruct &s : tab){
        generatePart(s.cell, part);
    }
    return part;
}

Part *Map::generatePart(Cell *cell, Part *part) {
    if (!part)
        part = new Part;
    part->mirador.push_back(cell);
    cell->_brutalised = true;
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y){
            Cell *cell2 = acess(cell->_x+x, cell->_y+y);
            if (cell2->_brutalised == false){
                if (cell2->_discovered == false){
                    generatePartUndiscover(cell2, part, cell);
                }
            }
        }
    return part;
}

void Map::reset() {
    init();
}

bool simCompleted(Part *part) {
    for (Cell *cell : part->mirador)
        if (cell->_brutalised == false)
            return false;
    return true;
}

void Map::brutetalizing(Part *part, int mirador_inc = 0){
    static auto time = std::chrono::system_clock::now();
    if ((size_t)mirador_inc >= part->mirador.size()){
        part->tot++;
        /*std::chrono::duration<double> time2 = std::chrono::system_clock::now() - time;
        if (time2.count() >= 1.0/60){
            _sfml->display();
            time = std::chrono::system_clock::now();
        }*/
        for (Cell *cell2 : part->undiscover)
            if (cell2->_proba == 1)
                cell2->_bruteInc++;
        return;
    }
    Cell *cell = part->mirador[mirador_inc]; 
    int count = 0;
    int risk = 0;
    int certainMine = 0;
    float minCertitude = 1;
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y){
            _grid_mutex.lock_read();
            Cell *const cell2 = acess(cell->_x+x, cell->_y+y);
            if (cell2->_discovered == false){
                minCertitude = std::min(minCertitude, cell2->_certitude);
                risk++;
                if (cell2->_proba != 0){
                    count++;
                    if (cell2->_proba == 1)
                        certainMine++;
                }
            }
            _grid_mutex.unlock_read();
        }
    if (count < cell->_nearMine || certainMine > cell->_nearMine){ // return if suposition was false
        return;
    }
    cell->_brutalised = true;
    if (certainMine < cell->_nearMine){
        for (int x = -1; x <= 1; ++x)
            for (int y = -1; y <= 1; ++y){
                _grid_mutex.lock_write();
                Cell *cell2 = acess(cell->_x+x, cell->_y+y);
                if (cell2->_certitude != 1 && cell2->_discovered == false && cell2->_brutalised == false){
                    float backup = cell2->_proba;
                    cell2->_proba = 1;
                    cell2->_brutalised = true;
                    _grid_mutex.unlock_write();
                    brutetalizing(part, mirador_inc);
                    _grid_mutex.lock_write();
                    cell2->_brutalised = false;
                    cell2->_proba = backup;
                }
                _grid_mutex.unlock_write();
            }
    } else {
        std::deque<Cell *> backup_cell;
        std::deque<float> backup;
        for (int x = -1; x <= 1; ++x)
            for (int y = -1; y <= 1; ++y){
                _grid_mutex.lock_write();
                Cell *cell2 = acess(cell->_x+x, cell->_y+y);
                if (cell2->_brutalised == false && cell2->_certitude != 1 && cell2->_proba != 1 && cell2->_discovered == false){
                    backup_cell.push_back(cell2);
                    backup.push_back(cell2->_proba);
                    cell2->_proba = 0;
                    cell2->_brutalised = true;
                }
                _grid_mutex.unlock_write();
            }
        brutetalizing(part, mirador_inc+1);
        int i = 0;
        for (Cell *cell : backup_cell) {
            cell->_brutalised = false;
            cell->_proba = backup[i];
            i++;
        }   
    }
    cell->_brutalised = false;
}   

void Map::evaluatorlv2(Cell *cell) {

    std::cout << "CALCULATING..."<< std::endl;
    _grid_mutex.lock_write();
    Part *part = generatePart(cell);
    for (Cell *&cell : part->undiscover){
        std::cout << cell->_x << " " << cell->_y << " | ";
        cell->_brutalised = false;
        cell->_updated = 100;
        cell->_bruteInc = 0;
    }
    std::cout << std::endl;
    for (Cell *&cell : part->mirador){
        std::cout << cell->_x << " " << cell->_y << " | ";
        cell->_brutalised = false;
        cell->_updated = 100;
        cell->_bruteInc = 0;
    }
    std::cout << std::endl;
    _grid_mutex.unlock_write();
    brutetalizing(part, 0);
    _grid_mutex.lock_write();
    std::cout << "found " << part->tot << " posibility" << std::endl;
    if (part->tot) {
            for (Cell *cell : part->undiscover){
            cell->_proba = cell->_bruteInc/((float)part->tot);
            if (part->certitude == 1)
                cell->_certitude = 1;
            cell->_updated = 255;

            // auto click
            // if (cell->_proba == 0) {
            //     _grid_mutex.unlock_write();
            //     clickOnCell(cell->_x, cell->_y);
            //     _grid_mutex.lock_write();
            // }
            //

        }
    }
    _grid_mutex.unlock_write();
    std::cout << "END" << std::endl;
    delete part;
}


void Map::estimatorlv2(){
    while (_toEstimatelv2.size() > 0){
        Vector2i vec = _toEstimatelv2.front();
        _toEstimatelv2.pop_front();

        _XYminmax_mutex.lock();
        _toEstimatelv2_size = _toEstimatelv2.size();
        _XYminmax_mutex.unlock();
        _grid_mutex.lock_write();
        Cell *cell = acess(vec);
        cell->_updated = 255;
        float minCertitude = 1;
        for (int x = -1; x <= 1; ++x)
            for (int y = -1; y <= 1; ++y){
                Cell *cell = acess(vec.x+x, vec.y+y);
                if (cell->_discovered == false)
                    minCertitude = std::min(minCertitude, cell->_certitude);
            }
        _grid_mutex.unlock_write();
        if (minCertitude == 1){
            continue;
        }
        evaluatorlv2(cell);
    }
}

void Map::estimatorCell(Vector2i vec){
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y){
            Cell *&cell = acess(vec.x+x, vec.y+y);
            if (cell->_discovered == true)
                _toEstimate.push_front(Vector2i(vec.x+x, vec.y+y));
        }
}

void Map::estimator() {
    while (_toEstimate.size() > 0){
        Vector2i vec = _toEstimate.front();
        _toEstimate.pop_front();
        _XYminmax_mutex.lock();
        _toEstimate_size = _toEstimate.size();
        _XYminmax_mutex.unlock();
        _grid_mutex.lock_write();
        Cell *cell = acess(vec.x, vec.y);
        cell->_updated = 255;
        if (cell->_discovered == false){
            _grid_mutex.unlock_write();
            continue;
        }
        _grid_mutex.unlock_write();
        int count = 0;
        int risk = 0;
        int certainMine = 0;
        float minCertitude = 1;
        for (int x = -1; x <= 1; ++x)
            for (int y = -1; y <= 1; ++y){
                _grid_mutex.lock_read();
                Cell *const cell = acess(vec.x+x, vec.y+y);
                if (cell->_discovered == false){
                    minCertitude = std::min(minCertitude, cell->_certitude);
                    risk++;
                    if (cell->_proba != 0){
                        count++;
                        if (cell->_proba == 1)
                            certainMine++;
                    }
                }
                _grid_mutex.unlock_read();
            }
        //int safe = risk-count;
        if (count == cell->_nearMine){
            cell->_certitude = 1;
            for (int x = -1; x <= 1; ++x)
                for (int y = -1; y <= 1; ++y){
                    _grid_mutex.lock_write();
                    Cell *cell = acess(vec.x+x, vec.y+y);
                    if (cell->_discovered == false && cell->_proba != 0 && cell->_proba != 1){
                        cell->_proba = 1;
                        cell->_certitude = 1;
                        estimatorCell(Vector2i(vec.x+x, vec.y+y));
                    }
                    _grid_mutex.unlock_write();
                }
        } else if (certainMine == cell->_nearMine){
            for (int x = -1; x <= 1; ++x)
                for (int y = -1; y <= 1; ++y){
                    _grid_mutex.lock_write();
                    Cell *cell = acess(vec.x+x, vec.y+y);
                    if (cell->_discovered == false && cell->_proba != 1 && cell->_proba != 0){
                        cell->_proba = 0;

                        // auto click
                        // _grid_mutex.unlock_write();
                        // clickOnCell(cell->_x, cell->_y);
                        // _grid_mutex.lock_write();
                        //
                        cell->_certitude = 1;
                        estimatorCell(Vector2i(vec.x+x, vec.y+y));
                    }
                    _grid_mutex.unlock_write();
                }
        } else {
            _toEstimatelv2.push_back(vec);
        } 
    }
    int static tmp = 5;
    if (_toEstimate.size() == 0)
        tmp--;
    else
        tmp = 5;
    if (tmp < 0)
        estimatorlv2();
}

void Map::generate(){
    _list_mutex.lock();
    while (_list.size() > 0){
        Vector2i vec = _list.front();
        _list.pop_front();
        _XYminmax_mutex.lock();
        _list_size = _list.size();
        _XYminmax_mutex.unlock();
        _list_mutex.unlock();
        _grid_mutex.lock_read();
        Cell *cell = acess(vec.x, vec.y);
        if (cell->_discovered == true){
            _grid_mutex.unlock_read();
            _list_mutex.lock();
            continue;
        }
        _XYminmax_mutex.lock();
        _nbCellDiscovered++;
        _XYminmax_mutex.unlock();
        cell->_discovered = true;
        cell->calcNearMine(*this);
        int count = cell->_nearMine;
        _grid_mutex.unlock_read();
        if (count == 0){
            for (int x = -1; x <= 1; ++x)
                for (int y = -1; y <= 1; ++y)
                    if ((y != 0 || x != 0)){
                        _list_mutex.lock();
                        _list.push_back(Vector2i(vec.x+x, vec.y+y));
                        _list_mutex.unlock();
                    }
        } else {
            _grid_mutex.lock_read();
            estimatorCell(Vector2i(vec.x, vec.y));
            _grid_mutex.unlock_read();
        }
        _list_mutex.lock();
    }
    if (_list.size() == 0){
        _list_mutex.unlock();
        estimator();
    } else {
        _list_mutex.unlock();
    }
    _list_mutex.lock();
    if (_need_estimate)
        std::cout << "estimate END" << std::endl;
    _need_estimate = false;
    _list_mutex.unlock();
}
