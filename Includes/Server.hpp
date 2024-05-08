#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>
#include "Clients.hpp"

class Clients;

class Server
{
    public:
        void setFd(int fd);
        void setAddrIp(std::string addrIp);
        int getFd() const;
        std::string getAddrIp() const;
        Server( std::string av );

    private:
        int _fd;
        std::string _addrIp;
        std::map<int, Clients> _clients;
        std::string _cmd[2];
};

#endif