#ifndef SERVER_HPP
#define SERVER_HPP


#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cmath>
#include <string>
#include <exception>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <cerrno>

#include <arpa/inet.h>
#include <stdexcept>

#include "Client.hpp"
#include <map>


class Server
{
    public:
        Server();
        Server(int port, const std::string& password);
        Server(const Server &other);
        Server& operator=(const Server &other);
        ~Server();

        void start();
        void handleMessage(int fd);
        std::string readMessage(int fd); 
        void setupSocket();
        void run();
        void acceptClients();
        void receiveData(int index);
        void removeData(int index);

        void sendToClient(int clientFd, const std::string& message);
        void sendData(int index);

    private:
        int port;
        int serverFd;
        std::string password;
        std::vector<pollfd> pollFds;
        std::map<int, Client*> clients;

       

     
};


#endif