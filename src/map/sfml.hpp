/*
** EPITECH PROJECT, 2021
** infiniteStatisticMineSweeper
** File description:
** sfml
*/


#ifndef SFML_HPP_
#define SFML_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <deque>

class Map;

class SfmlDisplay {
public:
    SfmlDisplay(Map *map, int win_width = 800, int win_heigh = 800);

    ~SfmlDisplay();

    void main_loop();

    unsigned char colorR(unsigned char a, unsigned char b, float r);

    sf::Color colorRatio(sf::Color a, sf::Color b, float r);

    bool display();

    int _width;
    int _height;
    float _posx = 0;
    float _posy = 0;
    Map *_map;
    sf::Font _font;
    int _interfaceFontSize = 30;
    int _cellSize = 40;
    int _fontSize = _cellSize-2;
    sf::RenderWindow *_window;
    bool _focus = true;
};

#include "map.hpp"

#endif /* !SFML_HPP_ */
