#ifndef CLIENT_HPP
#define CLIENT_HPP


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



class Client 
{
    public:
        Client();
        Client(int fd, const std::string& hostname, int port);
        Client(const Client &other);
        Client& operator=(const Client &other);
        ~Client();

        int getFd() const;
        const std::string& getHostname() const;
        int getPort() const;

        void appendInput(const std::string& data);
        bool hasCompleteLine() const;
        std::string extractLine();

        
        void appendOutput(const std::string& data);
        bool hasPendingOutput() const;
        const std::string& getOutputBuffer() const;
        void removeSentOutput(std::string::size_type bytes);


    private:
        int fd;
        std::string hostname;
        int port;
        std::string inputBuffer;
        std::string outputBuffer;

};


#endif