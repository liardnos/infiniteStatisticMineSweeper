/*
** EPITECH PROJECT, 2021
** infiniteStatisticMineSweeper
** File description:
** sfml
*/


#define SDL_DISPLAY 1

#pragma once

#if SDL_DISPLAY

#include <iostream>
#include <string>
#include <thread>
#include <deque>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Map;

class Sdl2Display {
public:
    Sdl2Display(Map *map, int win_width = 800, int win_heigh = 800);

    ~Sdl2Display();

    void main_loop();

    unsigned char colorR(unsigned char a, unsigned char b, float r);

    #if 0
    sf::Color colorRatio(sf::Color a, sf::Color b, float r);
    #endif

    bool display();

    int _width;
    int _height;
    float _posx = 0;
    float _posy = 0;
    Map *_map;
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    TTF_Font *_font;
    Uint8 const *_sdlKeyboardState; 

    bool _windowOpen = true;

    int _interfaceFontSize = 30;
    int _cellSize = 40;
    int _fontSize = _cellSize-2;
    bool _focus = true;
};

#include "map.hpp"

#endif
