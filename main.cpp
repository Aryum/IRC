#include "Server.hpp"
#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <exception>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    char* end;
    errno = 0;
    long port = std::strtol(argv[1], &end, 10);

    if (errno || *argv[1] == '\0' || *end != '\0'
        || port < 1 || port > 65535)
    {
        std::cerr << "Error: invalid port" << std::endl;;
        return 1;
    }

    if (argv[2][0] == '\0')
    {
        std::cerr << "Error: empty password" << std::endl;
        return 1;
    }

    try
    {
        Server server(static_cast<int>(port), argv[2]);
        server.start();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;;
        return 1;
    }

    return 0;
}

