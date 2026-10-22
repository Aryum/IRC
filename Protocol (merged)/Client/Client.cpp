#include "Client.hpp"

Client::Client(int socket) :
		_userName(),
		_nickName(),
		_hostName(),
		_socket(socket),
		_correctPassword(false),
		_curChannel(nullptr)
{}

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

void Client::appendInput(const std::string& data) //append data to buffer
{
    inputBuffer += data; 
}

bool Client::hasCompleteLine() const 
{
    return inputBuffer.find("\r\n") != std::string::npos; 
}

void Client::appendOutput(const std::string& data)
{
    outputBuffer += data;
}

bool Client::hasPendingOutput() const
{
    return !outputBuffer.empty();
}

const std::string& Client::getOutputBuffer() const
{
    return outputBuffer;
}

void Client::removeSentOutput(std::string::size_type bytes)
{
    outputBuffer.erase(0, bytes);
}

std::string Client::extractLine() 
{
    std::string::size_type pos = inputBuffer.find("\r\n"); 

    if (pos == std::string::npos) 
        return ""; // empty string if no change

    std::string line = inputBuffer.substr(0, pos); 

    inputBuffer.erase(0, pos + 2); //erase from buffer

    return line; // 
}
