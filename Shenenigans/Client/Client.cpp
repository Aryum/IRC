#include "Client.hpp"

Client::Client(int socket) :
		_userName(),
		_nickName(),
		_hostName(),
		_socket(socket),
		_correctPassword(false)
{

}
bool Client::operator==(const Client &other) const
{
	return _socket == other._socket;
}
int Client::getSocket()
{
	return _socket;
}
void Client::setNames(NameType type, std::string value)
{
	switch (type)
	{
		case Name_Host:
			_hostName = value;
			return ;
		case Name_Nick:
			_nickName = value;
			return ;
		case Name_User:
			_userName = value;
			return ;
	}
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