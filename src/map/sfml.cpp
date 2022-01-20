/*
** EPITECH PROJECT, 2021
** infiniteStatisticMineSweeper
** File description:
** sfml
*/

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <deque>

#include "sfml.hpp"
#include "map.hpp"

SfmlDisplay::SfmlDisplay(Map *map, int win_width, int win_heigh) : 
    _width(win_width),
    _height(win_heigh),
    _map(map)
{
    _window = new sf::RenderWindow(sf::VideoMode(win_width, win_heigh), "Infinite statistique MineSweeper", sf::Style::Close);
    _window->setFramerateLimit(60);

    if (!_font.loadFromFile("./font/font.TTF")){
        std::cerr << "cannot load font" << std::endl;
        throw "cannot load font";
    }
}

SfmlDisplay::~SfmlDisplay() {
    delete _window;
}

void SfmlDisplay::main_loop(){
    bool go = true;
    std::thread predition_thread([this, &go](){
        while (go) {
            this->_map->generate(1E6);
            std::this_thread::sleep_for(std::chrono::microseconds((int)1E6/60));
        }
    });
    while (_window->isOpen()) {
        display();

        // for benchamrk
        // if (_map->_list.size() == 0 && _map->_toEstimate.size() == 0 && _map->_toEstimatelv2.size() == 0)
        //     _window->close();
    }
    go = false;
    predition_thread.join();
}

unsigned char SfmlDisplay::colorR(unsigned char a, unsigned char b, float r){
    return pow(pow(a, 2)*r + pow(b, 2)*(1-r), 0.5);
}

sf::Color SfmlDisplay::colorRatio(sf::Color a, sf::Color b, float r)
{
    sf::Color c = {
        (unsigned char)colorR(a.r, b.r, r),
        (unsigned char)colorR(a.g, b.g, r),
        (unsigned char)colorR(a.b, b.b, r)
    };
    return c;
}

bool SfmlDisplay::display(){
    if (!_window->isOpen())
        return false;

    sf::Event event;
    while (_window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            _window->close();
        else if (event.type == sf::Event::MouseButtonPressed) {
            auto m = event.mouseButton;
            int x = round((m.x-_posx*_cellSize - _width/2.f)/_cellSize-0.5);
            int y = round((m.y-_posy*_cellSize - _height/2.f)/_cellSize-0.5);
            
            if (m.button == sf::Mouse::Button::Left)
                if (_map->clickOnCell(x, y));   
                    //_map->reset();
            else if (m.button == sf::Mouse::Button::Right)
                ;
            
        } else if (event.type == sf::Event::MouseWheelMoved){
            float d = event.mouseWheel.delta;
            if (_fontSize + d > 1){
                _fontSize += d;
                _cellSize += d;
            }
        } else if(event.type == sf::Event::GainedFocus)
                _focus = true;
        else if(event.type == sf::Event::LostFocus)
                _focus = false;
    }
    if (_focus){
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
            _posx += 0.25;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            _posx -= 0.25;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
            _posy += 0.25;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            _posy -= 0.25;
    }


    /*sf::Vector2i m = sf::Mouse::getPosition(*_window);
    for (ASfmlModule *mod : _modules){
        if (mod->x < m.x && m.x < mod->x + mod->width && mod->y < m.y && m.y < mod->y + mod->height) {
            mod->mouse_over(mod->x - m.x, mod->y - m.y);
        }
    }*/
    _window->clear();
    //draw
    sf::RectangleShape rectangle1(sf::Vector2f(_cellSize-2, _cellSize-2));
    rectangle1.setFillColor({50, 50, 50});
    sf::RectangleShape rectangle2(sf::Vector2f(_cellSize-2, _cellSize-2));
    rectangle2.setFillColor({100, 100, 100});
    sf::RectangleShape rectangle3(sf::Vector2f(_cellSize-2, _cellSize-2));
    rectangle3.setFillColor({0, 0, 255});

    sf::RectangleShape rectangle_has_explode(sf::Vector2f(_cellSize-2, _cellSize-2));
    rectangle_has_explode.setFillColor({255, 0, 0, 255});

    static sf::Text *text = 0;
    if (!text) {
        text = new sf::Text();
        text->setFont(_font);
    }
    text->setCharacterSize(_fontSize);

    sf::Color colors[10] = {
        {0, 0, 0},
        {0, 0, 255, 255}, {0, 255, 0, 255}, {255, 0, 0, 255},
        {125, 0, 255, 255}, {0, 255, 125, 255}, {255, 125, 0},
        {0, 255, 255, 255}, {255, 255, 0, 255}, {255, 0, 255, 255}
        };

    sf::RectangleShape *rect = &rectangle1;
    _map->_XYminmax_mutex.lock();
    int Xstart = round((0-_posx*_cellSize - _width/2.f)/_cellSize-0.5) - 1;
    int Xend = round((_width-_posx*_cellSize - _width/2.f)/_cellSize-0.5) + 1;
    int Ystart = round((0-_posy*_cellSize - _height/2.f)/_cellSize-0.5) - 1;
    int Yend = round((_height-_posy*_cellSize - _height/2.f)/_cellSize-0.5) + 1;
    _map->_XYminmax_mutex.unlock();
    for (int x = Xstart; x <= Xend; x++) {
        for (int y = Ystart; y <= Yend; y++){
            _map->_grid_mutex.lock_read();
            _map->_XYminmax_mutex.lock();
            Cell *const cell = _map->acessWeak(x, y);
            _map->_XYminmax_mutex.unlock();
            if (!cell) {
                _map->_grid_mutex.unlock_read();
                continue;
            }

            if (cell->_discovered == true){
                //text->setCharacterSize(_fontSize);
                rect = &rectangle2;
                rect->setPosition(_posx*_cellSize + x*_cellSize + _width/2, _posy*_cellSize + y*_cellSize + _height/2);
                _window->draw(*rect);
                //draw mine quantity
                if (!cell->_nearMine){
                    _map->_grid_mutex.unlock_read();
                    continue;
                }
                text->setFillColor(colors[cell->_nearMine]);
                text->setString(cell->getChar());
                auto bounds = text->getGlobalBounds();
                text->setPosition((int)(_posx*_cellSize + (x+0.5)*_cellSize + _width/2 - bounds.width/2), (int)(_posy*_cellSize + (y+0.5)*_cellSize + _height/2 - bounds.height/2*1.75));
                _window->draw(*text);
            } else {
                if (cell->_hasExplode)
                    rect = &rectangle_has_explode;
                else 
                    rect = &rectangle1;

                rect->setPosition(_posx*_cellSize + x*_cellSize + _width/2, _posy*_cellSize + y*_cellSize + _height/2);
                _window->draw(*rect);
                if (cell->_hasExplode)
                    rect->setFillColor({255, 0, 0, 255});
                
                if (0) {
                    //draw proba
                    text->setCharacterSize(_fontSize/3);
                    text->setFillColor(colorRatio(sf::Color::Red, sf::Color::Green, cell->_proba));
                    text->setString(std::to_string(cell->_proba).substr(0, 5));
                    auto bounds = text->getGlobalBounds();
                    text->setPosition((int)(_posx*_cellSize + (x+0.5)*_cellSize + _width/2 - bounds.width/2), (int)(_posy*_cellSize + (y+0.5)*_cellSize + _height/2 - bounds.height/2*1.75));
                    _window->draw(*text);
                    //draw certitude
                    text->setFillColor(colorRatio(sf::Color::Red, sf::Color::Green, 1-cell->_certitude));
                    text->setCharacterSize(_fontSize/4);
                    text->setString(std::to_string(cell->_certitude).substr(0, 5));
                    auto bounds2 = text->getGlobalBounds();
                    text->setPosition((int)(_posx*_cellSize + (x+0.5)*_cellSize + _width/2 - bounds2.width/2), (int)(_posy*_cellSize + (y+0.5)*_cellSize + _height/2 + bounds.height*0.75));
                    _window->draw(*text);
                }
            }
            if (0) {
                // draw pos
                text->setCharacterSize(_fontSize/4);
                text->setFillColor(sf::Color::White);
                text->setString(" (" + std::to_string(cell->_x).substr(0, 3) + ", " + std::to_string(cell->_y).substr(0, 3) + ")");
                text->setPosition((int)(_posx*_cellSize + x*_cellSize + _height/2), (int)(_posy*_cellSize + (y)*_cellSize + _width/2));
                _window->draw(*text);
            }

            if (cell->_updated){
                _map->_grid_mutex.unlock_read();
                _map->_grid_mutex.lock_write();
                cell->_updated *= 0.9;
                _map->_grid_mutex.unlock_write();
                _map->_grid_mutex.lock_read();
                rect = &rectangle3;
                rect->setFillColor({0, 0, 255, cell->_updated});
                rect->setPosition(_posx*_cellSize + x*_cellSize + _width/2, _posy*_cellSize + y*_cellSize + _height/2);
                _window->draw(*rect);
            }
            if (cell->_mark){
                rect = &rectangle3;
                rect->setFillColor({255, 0, 0, 50});
                rect->setPosition(_posx*_cellSize + x*_cellSize + _width/2, _posy*_cellSize + y*_cellSize + _height/2);
                _window->draw(*rect);
            }
            if (cell->_brutalised){
                rect = &rectangle3;
                rect->setFillColor({255, 255, 255, 75});
                rect->setPosition(_posx*_cellSize + x*_cellSize + _width/2, _posy*_cellSize + y*_cellSize + _height/2);
                _window->draw(*rect);
            }
            _map->_grid_mutex.unlock_read();
        }
    }    
    text->setCharacterSize(_interfaceFontSize);
    _map->_XYminmax_mutex.lock();
    text->setString(std::to_string(_map->_nbCellDiscovered));
    auto bounds = text->getGlobalBounds();
    text->setFillColor({255, 0, 0, 255});
    text->setPosition(_width/2-bounds.width/2, 0);
    _window->draw(*text);

    text->setString(std::to_string(_map->_list_size) + " | " + std::to_string(_map->_toEstimate_size) + " | " + std::to_string(_map->_toEstimatelv2_size));
    _map->_XYminmax_mutex.unlock();
    bounds = text->getGlobalBounds();
    text->setFillColor({255, 255, 255, 255});
    text->setPosition(_width/5-bounds.width/2, 0);
    _window->draw(*text);

    _window->display();
    return true;
}
