/*
** EPITECH PROJECT, 2021
** Babel - TCP
** File description:
** Error Management
*/

#ifndef ERROR_TCP_HPP
#define ERROR_TCP_HPP

#include <string>
#include <exception>

class NetworkError : public std::exception {
    public:
        NetworkError(std::string const &message, std::string const &component = "NetWorkError")
        : _message(message), _component(component) {};

        std::string const &getComponent() const noexcept { return _component;};
        const char *what() const noexcept {return _message.c_str();};
    private:
        std::string _message;
        std::string _component;
};

#endif