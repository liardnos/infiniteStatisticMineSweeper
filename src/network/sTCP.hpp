/*
** EPITECH PROJECT, 2021
** Babel
** File description:
** socket TCP
*/

#ifndef SOCKET_TCP
#define SOCKET_TCP

#define DEBUGINFORMATION std::string(" | from ") + std::string(__FILE__) + std::string(" L") + std::to_string(__LINE__)

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <iostream>
#include <string.h>
#include <stdint.h>
#include <map>
#include <utility>


#include "ByteObject.hpp"


#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#elif _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif
#include "NetworkError.hpp"

#define MAX_PACKET_SIZE 1024*1024*1024

namespace sTCP {



class Client {
public:
    static void sTCPclosesocket(int a) {
        #ifdef __linux__
        ::close(a);
        #elif _WIN32
        ::closesocket(a);
        #endif
    }

    /**
     * create a TCP stcpclient
     * @param port 
     * @param rcvCallBack 
     * @param id = -1
     */
    Client(std::string const &address, int port, std::function<void(Client const &stcpclient, uint8_t const *buf, uint size)> const &rcvCallBack, int const id = -1) :
        _id(id), rcvCallBack(rcvCallBack)
    {
        struct sockaddr_in adrr;
        memset(&adrr, 0, sizeof(adrr));
        std::cout << address << std::endl;
        adrr.sin_family = AF_INET;
        #ifdef __linux__
        inet_pton(AF_INET, address.c_str(), &(adrr.sin_addr));
        #elif _WIN32
        //inetPton(AF_INET, address.c_str(), &(adrr.sin_addr));
        #endif
        adrr.sin_port = htons(port);

        if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            throw NetworkError("failed to create socket" + DEBUGINFORMATION);
        if (connect(_fd, (struct sockaddr*)&adrr, sizeof(adrr)) < 0)
            throw NetworkError("failed to connect to server" + DEBUGINFORMATION);
        std::cout << "socket conected " << std::endl;
    }

    /**
     * create a TCP stcpclient
     * @param fd
     * @param rcvCallBack 
     * @param id = -1
     */
    Client(int fd, std::function<void(Client const &stcpclient, uint8_t const *buf, uint size)> const rcvCallBack, int const id = -1) :
        _fd(fd), _id(id), rcvCallBack(rcvCallBack)
    {}

    ~Client() {
        //shutdown(_fd, SHUT_RDWR);
        Client::sTCPclosesocket(_fd);
    }

    /**
     * send a packet to the connected stcpclient
     * @param data
     */
    void send(std::vector<uint8_t> const data) const {
        int size = data.size();
        sendAll((char*)&size, sizeof(size));
        sendAll((char*)&data[0], size);
    }

    /**
     * send a std::string to another stcpclient
     * @param str
     */
    void sendStr(std::string const &str) {
        std::vector<uint8_t> buf;
        buf.resize(str.size());
        memcpy(&buf[0], str.c_str(), str.size());
        send(buf);
    }

    /**
     * update stcpclient (check for callback or disconnections)
     */
    void update() const {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(_fd, &set);
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        select(FD_SETSIZE, &set, 0, 0, &timeout);
        if (FD_ISSET(_fd, &set)) {
            int const readSize = 2048;
            int size;
            do {
                int al = buf.size();
                buf.resize(buf.size() + readSize);
                #ifdef _WIN32
                size = recv(_fd, (char *)&buf[al], readSize, 0);
                #elif __linux__
                size = recv(_fd, &buf[al], readSize, 0);
                #endif
                if (size <= 0)
                    throw NetworkError("failed to read on socket" + DEBUGINFORMATION);
                if (buf.size() > MAX_PACKET_SIZE)
                    throw NetworkError("PACKET to big" + DEBUGINFORMATION);
                buf.resize(al + size);
            } while (size == readSize);
            while (buf.size() > sizeof(int)) {
                uint msgSize = *(int*)(&buf[0]);
                if (msgSize <= buf.size() - sizeof(int)) {
                    rcvCallBack(*this, &buf[sizeof(int)], msgSize);
                    buf.erase(buf.begin(), buf.begin() + msgSize + sizeof(int));
                }
                else {
                    break;
                }
            }
        }
    }

    int _fd;
    int const _id;
    uint mutable _badPacketCount = 0;

private:
    /**
     * connect a socket to a server
     * @param ip
     * @param port
     */
    int connect_active_socket(int ip, int port) const {
        int datasock;
        if ((datasock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            throw NetworkError("faield to create socket" + DEBUGINFORMATION);
        struct sockaddr_in serv_add;
        memset(&serv_add, 0, sizeof(serv_add));
        serv_add.sin_family = AF_INET;
        serv_add.sin_addr.s_addr = htonl(ip);
        serv_add.sin_port = htons(port);
        if (connect(datasock, (struct sockaddr*)&serv_add, sizeof(serv_add)) < 0)
            throw NetworkError("faield to connect to server" + DEBUGINFORMATION);
        return datasock;
    }

    /**
     * send a char *data to the distant stcpclient
     * @param data
     * @param size
     */
    int sendAll(char *data, uint size) const {
        while (size > 0) {
            #ifdef __linux__
            int sendedsize = ::send(_fd, data, size, MSG_NOSIGNAL);
            #elif _WIN32
            int sendedsize = ::send(_fd, data, size, 0);
            #endif
            if (sendedsize < 0)
                throw NetworkError("sendall failed" + DEBUGINFORMATION);
            size -= sendedsize;
            data += sendedsize;
        }
        return 1;
    }

    std::function<void(Client const &stcpclient, uint8_t const *buf, uint size)> const rcvCallBack;
    std::vector<uint8_t> mutable buf;
};

class Server {
public:
    /**
     * create a TCP server
     * @param port 
     * @param onConnectCallBack 
     * @param onDisconnectCall 
     * @param rcvCallBack 
     */
    Server(int port, 
        std::function<void(Server const &server, Client const &stcpclient, uint id)> const &onConnectCallBack, 
        std::function<void(Server const &server, Client const &stcpclient, uint id)> const &onDisconnectCall,
        std::function<void(Client const &stcpclient, uint8_t const *buf, uint size)> const &rcvCallBack) :
        port(port), fd(binded_socket(port)),
        onConnectCallBack(onConnectCallBack),
        onDisconnectCallBack(onDisconnectCall),
        rcvCallBack(rcvCallBack)
    {}

    ~Server() {
        //shutdown(fd, SHUT_RDWR);
        Client::sTCPclosesocket(fd);
    }

    /**
     * update the server (used to triger all callback and remove disconnected users)
     */
    void update() {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd, &set);
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        while (1) {
            select(FD_SETSIZE, &set, 0, 0, &timeout);
            if (FD_ISSET(fd, &set)) {
                int clientFD = accept(fd, 0, 0);
                if (clientFD < 0)
                    throw NetworkError("Cannot accept client" + DEBUGINFORMATION);
                clients[rolingID] = std::make_shared<Client>(clientFD, rcvCallBack, rolingID);
                onConnectCallBack(*this, *clients[rolingID], rolingID);
                rolingID++;
            } else
                break;
        }
        std::vector<int> toErase;
        for (auto& c : clients) {
            try {
                c.second->update();
            } catch (NetworkError& e) {
                toErase.push_back(c.first);
                onDisconnectCallBack(*this, *c.second, c.first);
            } catch (...) {
                toErase.push_back(c.first);
                onDisconnectCallBack(*this, *c.second, c.first);
            }
        }
        for (auto& c : toErase)
            clients.erase(c);
    }

    /**
     * disconnect a stcpclient
     */
    void closeClient(uint id) {
        (void)id;
    }

    static int binded_socket(int port) {
        int server_fd;
        int opt = 1;

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
            throw NetworkError("socket failed" + DEBUGINFORMATION);
        if (setsockopt(server_fd, 1, 2 | 15, (char *)&opt, sizeof(opt)))
            throw NetworkError("setsockopt" + DEBUGINFORMATION);
        struct sockaddr_in address;
        //int addrlen = sizeof(address);
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
            throw NetworkError("bind failed" + DEBUGINFORMATION);
        listen(server_fd, 10);
        return server_fd;
    }

    int rolingID = 0;
    int const port;
    int fd;

    /**
     * call back for when an user connect
     */
    std::function<void(sTCP::Server const &serv, sTCP::Client const &stcpclient, uint id)> onConnectCallBack;
    /**
     * call back for when an user disconnect
     */
    std::function<void(sTCP::Server const &serv, sTCP::Client const &stcpclient, uint id)> onDisconnectCallBack;
    /**
     * call back for when an user rcv a packet
     */
    std::function<void(Client const &stcpclient, uint8_t const *buf, uint size)> const rcvCallBack;

    std::map<uint, std::shared_ptr<Client>> clients;

private:
    /**
     * binde a socket to a port
     * @param port
     */
};
};

#endif