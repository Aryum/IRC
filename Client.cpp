#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

/*using namespace std;

int main()
{
    // creating socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // sending connection request

	string input;
	while (1)
	{

		getline(cin, input);
    	connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
		send(clientSocket, input.c_str(), strlen(input.c_str()), 0);
		if (input == "close")
			break;
	    close(clientSocket);
	}
    return 0;
}

*/

#include "Client.hpp"


Client::Client(int fd, const std::string& hostname, int port) : fd(fd), hostname(hostname), port(port) 
{
}

Client::~Client() 
{
}

int Client::getFd() const 
{
    return fd;
}

const std::string& Client::getHostname() const 
{
    return hostname;
}

int Client::getPort() const 
{
    return port;
}

void Client::appendInput(const std::string& data) //append data to buffer
{
    inputBuffer += data; 
}

bool Client::hasCompleteLine() const 
{
    return inputBuffer.find("\r\n") != std::string::npos; 
}

std::string Client::extractLine() // Extracts the next complete IRC line from the input buffer and returns it without the line ending.
{
    std::string::size_type pos = inputBuffer.find("\r\n"); 

    if (pos == std::string::npos) 
        return ""; // empty string if no change

    std::string line = inputBuffer.substr(0, pos); 

    inputBuffer.erase(0, pos + 2); //erase from buffer

    return line; // 
}
