/*
** EPITECH PROJECT, 2020
** YEP Indie Studio
** File description:
** Indie Object Class
*/

#ifndef BYTEOBJECT_OBJ_
#define BYTEOBJECT_OBJ_

#define DEBUGINFORMATION std::string(" | from ") + std::string(__FILE__) + std::string(" L") + std::to_string(__LINE__)

#include <vector>
#include <string.h>
#include <string>
#include <ostream>
#include <exception>
#include <iostream>

#ifdef _WIN32 
typedef unsigned int uint;
#endif 

class ByteObjectError : public std::exception {
    public:
        ByteObjectError(std::string const &message, std::string const &component = "ByteObjectError")
        : _message(message), _component(component) {};

        std::string const &getComponent() const noexcept { return _component;};
        const char *what() const noexcept {return _message.c_str();};
    private:
        std::string _message;
        std::string _component;
};

class ByteObject {
public:
    ByteObject() {}

    ByteObject(std::vector<uint8_t> const &msg, uint size) :
        _data(msg), _size(size)
    {}
    
    ByteObject(std::vector<uint8_t> const &msg) :
        _data(msg), _size(msg.size())
    {}

    ByteObject(uint8_t const *msg, uint size) :
        _size(size)
    {
        _data.resize(_size);
        memcpy(&_data[0], msg, _size);
    }

    ~ByteObject(){}

    ByteObject &operator+(const ByteObject &that) const 
    {
        int newSize = that._size + this->_size;
        std::vector<uint8_t> newData;
        newData.insert(newData.end(), this->_data.begin(), this->_data.end());
        newData.insert(newData.end(), that._data.begin(), that._data.end());
        ByteObject *obj = new ByteObject(newData, newSize);
        return *obj;
    }

    template <typename T>
    ByteObject &operator<<(std::vector<T> const &thing) {
        std::size_t size = thing.size();
        *this << size;
        _data.resize(_size + size);
        memcpy(&_data[_size], &thing[0], size*sizeof(T));
        _size += size*sizeof(T);
        return *this;
    };

    ByteObject &operator<<(std::string const &thing) {
        std::size_t size = thing.size();
        *this << size;
        _data.resize(_size + size);
        memcpy(&_data[_size], &thing[0], size);
        _size += size;
        return *this;
    };

    template <typename T>
    ByteObject &operator<<(T const &thing) {
        _data.resize(_size + sizeof(T));
        memcpy(&_data[_size], &thing, sizeof(T));
        _size += sizeof(T);
        return *this;
    };

    template <typename T>
    ByteObject &operator>>(std::vector<T> &thing) {
        std::size_t size;
        *this >> size;
        thing.resize(size);
        //if (_cursor + size*sizeof(T) > _size)
        //    throw ByteObjectError("ByteObject operator>> out of bound" + DEBUGINFORMATION);
        memcpy(&thing[0], &_data[_cursor] , size*sizeof(T));
        _cursor += size*sizeof(T);
        return *this;
    };

    ByteObject &operator>>(std::string &thing) {
        std::size_t size;
        *this >> size;
        thing.resize(size);
        //if (_cursor + size*1 > _size)
        //    throw ByteObjectError("BypteObject operator>> out of bound" + DEBUGINFORMATION);
        memcpy(&thing[0], &_data[_cursor] , size*1);
        _cursor += size*1;
        return *this;
    };

    template <typename T>
    ByteObject &operator>>(T &thing) {
        //if (_cursor + sizeof(T) > _size)
        //    throw ByteObjectError("ByteObject operator>> out of bound" + DEBUGINFORMATION);
        memcpy(&thing, &_data[_cursor] , sizeof(T));
        _cursor += sizeof(T);
        return *this;
    };


    std::vector<uint8_t> const &dumpForNetwork() const {
        return _data;
    }    

    void setCursor(int val) {
        _cursor = val;
    }        

    void clear() {
        _cursor = 0;
        _size = 0;
    }

    std::vector<uint8_t> _data;
    uint _size = 0;
    uint _cursor = 0;
private:
};

std::basic_ostream<char> &operator<<(std::basic_ostream<char> &stream, const ByteObject &obj);
std::basic_istream<char> &operator>>(std::basic_istream<char> &stream, ByteObject &obj);


#endif