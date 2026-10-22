
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        std::cerr << "socket failed" << std::endl;
        return 1;
    }

    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (connect(fd, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1)
    {
        std::cerr << "connect failed" << std::endl;
        close(fd);
        return 1;
    }

    std::cout << "Connected. Type IRC commands (quit to exit)." << std::endl;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line == "quit")
            break;

        line += "\r\n";

        if (send(fd, line.c_str(), line.size(), 0) == -1)
        {
            std::cerr << "send failed" << std::endl;
            break;
        }
    }

    close(fd);
    return 0;
}
