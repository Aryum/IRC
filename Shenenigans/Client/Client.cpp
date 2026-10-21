#include "Client.hpp"

Client::Client(int socket) :
		_userName(),
		_nickName(),
		_hostName(),
		_socket(socket),
		_correctPassword(false)
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
void Client::sendReply(std::string reply) const
{

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