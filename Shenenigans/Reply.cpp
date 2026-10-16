#include "Reply.hpp"

Reply::Reply(std::string msg) :
	_message(msg)
{}

std::string Reply::getMessage()
{
	return _message;
}