/*
** EPITECH PROJECT, 2021
** infiniteStatisticMineSweeper
** File description:
** sfml
*/



#if SDL_DISPLAY

#include <iostream>
#include <string>
#include <thread>
#include <deque>

#include "sdl2.hpp"
#include "map.hpp"

Sdl2Display::Sdl2Display(Map *map, int win_width, int win_heigh) : 
    _width(win_width),
    _height(win_heigh),
    _map(map)
{
    
    _window = SDL_CreateWindow(
        "Infinite statistical MineSweeper",                  // window title
        0,           // initial x position
        0,           // initial y position
        win_width,                               // width, in pixels
        win_heigh,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
    _sdlKeyboardState = SDL_GetKeyboardState(NULL);

    TTF_Init();
    _font = TTF_OpenFont("./font/font.ttf", 24);
    std::cout << _font << std::endl;
    if (!_font)
        throw "cannot load font";
}

Sdl2Display::~Sdl2Display() {
    SDL_DestroyWindow(_window);
}

void Sdl2Display::main_loop(){
    bool go = true;
    std::thread predition_thread([this, &go](){
        while (go) {
            this->_map->generate();
            std::this_thread::sleep_for(std::chrono::microseconds((int)1E6/60));
        }
    });
    while (_windowOpen) {
        display();
        // for benchamrk
        // if (_map->_list.size() == 0 && _map->_toEstimate.size() == 0 && _map->_toEstimatelv2.size() == 0)
        //     _window->close();
    }
    go = false;
    predition_thread.join();
}

unsigned char Sdl2Display::colorR(unsigned char a, unsigned char b, float r){
    return pow(pow(a, 2)*r + pow(b, 2)*(1-r), 0.5);
}

#if 0
SDL_Color Sdl2Display::colorRatio(SDL_Color a, SDL_Color b, float r)
{
    SDL_Color c = {
        (unsigned char)colorR(a.r, b.r, r),
        (unsigned char)colorR(a.g, b.g, r),
        (unsigned char)colorR(a.b, b.b, r)
    };
    return c;
}
#endif

void SDL_draw_text(SDL_Renderer *renderer, SDL_Color color, TTF_Font *font, int x, int y, std::string const &text, float height) {
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), color); 
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    // SDL_Rect origine_rect;
    // origine_rect.x = 0;
    // origine_rect.y = 0;
    // origine_rect.w = 4*height;
    // origine_rect.h = height;
    int w;
    int h;
    TTF_SizeText(font, text.c_str(), &w, &h);
    SDL_Rect Message_rect;
    Message_rect.x = x;
    Message_rect.y = y;
    Message_rect.w = height*w/20;
    Message_rect.h = height*h/20;
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

bool Sdl2Display::display(){
    if (!_windowOpen)
        return false;

    SDL_PumpEvents();
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0) {
        if (e.type == SDL_QUIT) {
            _windowOpen = false;
        } else if (e.type == SDL_MOUSEBUTTONDOWN) { 
            SDL_MouseButtonEvent &m = e.button;
            int x = round((m.x-_posx*_cellSize - _width/2.f)/_cellSize-0.5);;
            int y = round((m.y-_posy*_cellSize - _height/2.f)/_cellSize-0.5);
            std::cout << "click=" << (int)m.button << std::endl;
            Cell *cell = _map->acess(x, y);
            if (m.button == SDL_BUTTON_RIGHT) {
                if (!cell->_flaged && _map->clickOnCell(x, y)) {
                    //_map->reset();
                }
            } else if (m.button == SDL_BUTTON_LEFT) {
                if (cell && !cell->_discovered) {
                    std::cout << "place flag" << std::endl;
                    cell->_flaged = !cell->_flaged;
                }
                if (cell->_discovered) {
                    int count = cell->_nearMine;
                    for (int xx = -1; xx <= 1; ++xx)
                        for (int yy = -1; yy <= 1; ++yy) {
                            Cell *nearCell = _map->acess(x+xx, y+yy);
                            if (nearCell->_flaged || nearCell->_hasExplode)
                                count--;
                        }
                    std::cout << "mine dif=" << count << std::endl;
                    if (!count) {
                        for (int xx = -1; xx <= 1; ++xx)
                            for (int yy = -1; yy <= 1; ++yy) {
                                Cell *nearCell = _map->acess(x+xx, y+yy);
                                if (!nearCell->_flaged && !nearCell->_hasExplode && !nearCell->_discovered)
                                    _map->clickOnCell(x+xx, y+yy);
                            }
                    }
                }
            }
        } else if (e.type == SDL_MOUSEWHEEL) {
            SDL_MouseWheelEvent &m = e.wheel;
            float d = m.y;
            if (_fontSize + d > 1){
                _fontSize += d;
                _cellSize += d;
            }
        } else if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                _focus = true;
            } else if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                _focus = false;
            }
        } else if (e.type == SDL_KEYDOWN) {
            SDL_KeyboardEvent &m = e.key;
            std::cout << m.keysym.scancode << std::endl;
            if (m.keysym.scancode == 15) {
                _map->saveInFile("map.isms");
            } else if (m.keysym.scancode == 51) {
                _map->loadFromFile("map.isms");
            }
        }
    }
    #if 0
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
    #else
    if (_focus){
        if (_sdlKeyboardState[SDL_SCANCODE_LEFT] || _sdlKeyboardState[SDL_SCANCODE_A])
            _posx += 0.25;
        if (_sdlKeyboardState[SDL_SCANCODE_RIGHT] || _sdlKeyboardState[SDL_SCANCODE_D])
            _posx -= 0.25;
        if (_sdlKeyboardState[SDL_SCANCODE_UP] || _sdlKeyboardState[SDL_SCANCODE_W])
            _posy += 0.25;
        if (_sdlKeyboardState[SDL_SCANCODE_DOWN] || _sdlKeyboardState[SDL_SCANCODE_S])
            _posy -= 0.25;
    }
    #endif

    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
    SDL_RenderClear(_renderer);

    //draw
    SDL_Rect rectangle1; // {50, 50, 50}
    rectangle1.w = _cellSize-2; rectangle1.h = _cellSize-2;
    // SDL_Rect rectangle2; // {100, 100, 100}
    // rectangle2.w = _cellSize-2; rectangle2.h = _cellSize-2;
    // SDL_Rect rectangle3; // {0, 0, 255}
    // rectangle3.w = _cellSize-2; rectangle3.h = _cellSize-2;
    

    SDL_Rect rectangle_has_explode; // {255, 0, 0, 255}
    rectangle_has_explode.w = _cellSize-2; rectangle_has_explode.h = _cellSize-2;
    SDL_Rect rectangle_is_flaged; // {0xFF, 0xA5, 0, 255}
    rectangle_is_flaged.w = _cellSize-2; rectangle_is_flaged.h = _cellSize-2;

    SDL_Color colors[10] = {
        {0, 0, 0, 255},
        {0, 0, 255, 255}, {0, 255, 0, 255}, {255, 0, 0, 255},
        {125, 0, 255, 255}, {0, 255, 125, 255}, {255, 125, 0, 255},
        {0, 255, 255, 255}, {255, 255, 0, 255}, {255, 0, 255, 255}
    };

    float fontScale = 1.0/80;
    SDL_Rect *rect = &rectangle1;
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

            if (cell->_discovered == true) {
                #if 0
                text->setCharacterSize(_fontSize);
                rect = &rectangle2;
                /rect->setPosition(_posx*_cellSize + x*_cellSize + _width/2, _posy*_cellSize + y*_cellSize + _height/2);
                _window->draw(*rect);
                #else
                rectangle1.x = _posx*_cellSize + x*_cellSize + _width/2;
                rectangle1.y =  _posy*_cellSize + y*_cellSize + _height/2;
                SDL_SetRenderDrawColor(_renderer, 100, 100, 100, 255);
                SDL_RenderFillRect(_renderer, &rectangle1);
                #endif
                //draw mine quantity
                if (!cell->_nearMine) {
                    _map->_grid_mutex.unlock_read();
                    continue;
                }
                #if 0
                text->setFillColor(colors[cell->_nearMine]);
                text->setString(cell->getChar());
                auto bounds = text->getGlobalBounds();
                text->setPosition((int)(_posx*_cellSize + (x+0.5)*_cellSize + _width/2 - bounds.width/2), (int)(_posy*_cellSize + (y+0.5)*_cellSize + _height/2 - bounds.height/2*1.75));
                _window->draw(*text);
                #else
                char text[2] = {0, 0};
                text[0] = cell->getChar();
                SDL_draw_text(_renderer, colors[cell->_nearMine], _font, _posx*_cellSize + (x+0.5)*_cellSize + _width/2, _posy*_cellSize + (y+0.5)*_cellSize + _height/2, text, _fontSize/3);
                #endif
            } else {
                if (cell->_hasExplode)
                    SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
                else if (cell->_flaged)
                    SDL_SetRenderDrawColor(_renderer, 0xFF, 0xA5, 0, 255);
                else
                    SDL_SetRenderDrawColor(_renderer, 50, 50, 50, 255);

                #if 0
                rect->setPosition(_posx*_cellSize + x*_cellSize + _width/2, _posy*_cellSize + y*_cellSize + _height/2);
                _window->draw(*rect);
                #else
                rectangle1.x = _posx*_cellSize + x*_cellSize + _width/2;
                rectangle1.y =  _posy*_cellSize + y*_cellSize + _height/2;
                SDL_RenderFillRect(_renderer, &rectangle1);
                #endif

                if (1) {
                    //draw proba
                    #if 0
                    text->setCharacterSize(_fontSize/3);
                    text->setFillColor(colorRatio(sf::Color::Red, sf::Color::Green, cell->_proba));
                    text->setString(std::to_string(cell->_proba).substr(0, 5));
                    auto bounds = text->getGlobalBounds();
                    text->setPosition((int)(_posx*_cellSize + (x+0.5)*_cellSize + _width/2 - bounds.width/2), (int)(_posy*_cellSize + (y+0.5)*_cellSize + _height/2 - bounds.height/2*1.75));
                    _window->draw(*text);
                    #else
                    SDL_draw_text(_renderer, colors[cell->_nearMine], _font, _posx*_cellSize + (x+0.5)*_cellSize + _width/2, _posy*_cellSize + (y+0.5)*_cellSize + _height/2, std::to_string(cell->getChar()), _fontSize/3);
                    #endif
                    //draw certitude
                    #if 0
                    text->setFillColor(colorRatio(sf::Color::Red, sf::Color::Green, 1-cell->_certitude));
                    text->setCharacterSize(_fontSize/4);
                    text->setString(std::to_string(cell->_certitude).substr(0, 5));
                    auto bounds2 = text->getGlobalBounds();
                    text->setPosition((int)(_posx*_cellSize + (x+0.5)*_cellSize + _width/2 - bounds2.width/2), (int)(_posy*_cellSize + (y+0.5)*_cellSize + _height/2 + bounds.height*0.75));
                    _window->draw(*text);
                    #else
                    #endif
                }
            }
            if (0) {
                // draw pos
                #if 0
                text->setCharacterSize(_fontSize/4);
                text->setFillColor(sf::Color::White);
                text->setString(" (" + std::to_string(cell->_x).substr(0, 3) + ", " + std::to_string(cell->_y).substr(0, 3) + ")");
                text->setPosition((int)(_posx*_cellSize + x*_cellSize + _height/2), (int)(_posy*_cellSize + (y)*_cellSize + _width/2));
                _window->draw(*text);
                #else
                #endif
            }

            if (cell->_updated){
                _map->_grid_mutex.unlock_read();
                _map->_grid_mutex.lock_write();
                cell->_updated *= 0.9;
                _map->_grid_mutex.unlock_write();
                _map->_grid_mutex.lock_read();
                #if 0
                rect = &rectangle3;
                rect->setFillColor({0, 0, 255, cell->_updated});
                rect->setPosition(_posx*_cellSize + x*_cellSize + _width/2, _posy*_cellSize + y*_cellSize + _height/2);
                _window->draw(*rect);
                #else
                rectangle1.x = _posx*_cellSize + x*_cellSize + _width/2;
                rectangle1.y =  _posy*_cellSize + y*_cellSize + _height/2;
                SDL_SetRenderDrawColor(_renderer, 0, 0, 255, cell->_updated);
                SDL_RenderFillRect(_renderer, &rectangle1);
                #endif
            }
            if (cell->_mark){
                #if 0
                rect = &rectangle3;
                rect->setFillColor({255, 0, 0, 50});
                rect->setPosition(_posx*_cellSize + x*_cellSize + _width/2, _posy*_cellSize + y*_cellSize + _height/2);
                _window->draw(*rect);
                #else
                rectangle1.x = _posx*_cellSize + x*_cellSize + _width/2;
                rectangle1.y =  _posy*_cellSize + y*_cellSize + _height/2;
                SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 50);
                SDL_RenderFillRect(_renderer, &rectangle1);
                #endif
            }
            if (cell->_brutalised){
                #if 0
                rect = &rectangle3;
                rect->setFillColor({255, 255, 255, 75});
                rect->setPosition(_posx*_cellSize + x*_cellSize + _width/2, _posy*_cellSize + y*_cellSize + _height/2);
                _window->draw(*rect);
                #else
                rectangle1.x = _posx*_cellSize + x*_cellSize + _width/2;
                rectangle1.y =  _posy*_cellSize + y*_cellSize + _height/2;
                SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 75);
                SDL_RenderFillRect(_renderer, &rectangle1);
                #endif
            }
            _map->_grid_mutex.unlock_read();
        }
    }    
    #if 0
    text->setCharacterSize(_interfaceFontSize);
    _map->_XYminmax_mutex.lock();
    text->setString(std::to_string(_map->_nbCellDiscovered));
    auto bounds = text->getGlobalBounds();
    text->setFillColor({255, 0, 0, 255});
    text->setPosition(_width/2-bounds.width/2, 0);
    _window->draw(*text);
    #else
    #endif

    #if 0
    text->setString(std::to_string(_map->_list_size) + " | " + std::to_string(_map->_toEstimate_size) + " | " + std::to_string(_map->_toEstimatelv2_size));
    _map->_XYminmax_mutex.unlock();
    bounds = text->getGlobalBounds();
    text->setFillColor({255, 255, 255, 255});
    text->setPosition(_width/5-bounds.width/2, 0);
    _window->draw(*text);
    #else
    #endif

    #if 0
    _window->display();
    #else
    SDL_RenderPresent(_renderer);
    #endif
    return true;
}


#endif
