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


Client::Client(int socket) :
		_userName(),
		_nickName(),
		_hostName(),
		_socket(socket),
		_correctPassword(false),
		_curChannel(nullptr)
{}

Client::~Client() 
{
}
/*
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
*/


bool Client::operator==(Client &other) const
{
	return _socket == other._socket;
}

bool Client::operator!=(Client &other) const
{
	return !((*this) == other);
}

int Client::getSocket()
{
	return _socket;
}

void Client::changeNick(std::string nick)
{
	_nickName = nick;
}

void Client::setNames(std::string user, std::string host, std::string nick)
{
	_hostName = host;
	_nickName = nick;
	_userName = user;
}


void Client::setPasswordVerified()
{
	_correctPassword = true;
}

bool Client::isRegistred()
{
	return (_hostName != "" && _userName != "" && _nickName != "" && _correctPassword);
}
std::string Client::get(NameType type) const
{
	switch (type)
	{
		case Name_Host:
			return _hostName;
		case Name_Nick:
			return _nickName;
		case Name_User:
			return _userName;
		default:
			return "";
	}
}