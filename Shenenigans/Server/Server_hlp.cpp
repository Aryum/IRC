#include "Server.hpp"

//move to a helper file probably need to make it return if it has a key or not, if it idx was already npos
std::string nextChar(const std::string &input, size_t &idx, char c, bool prefix)
{
	size_t curIdx;
	std::string ret;
	if (!prefix && input[0] == ':')
		curIdx = std::string::npos;
	else
		curIdx = input.find(c, idx);

	if (curIdx == std::string::npos)
		ret = idx != std::string::npos ? input.substr(idx) : "";
	else
	{
		ret = input.substr(idx, curIdx - idx);
		while (input[curIdx + 1] == c)
			curIdx++;
	}
	idx = curIdx + (curIdx != std::string::npos);
	return ret;
}
//if we change to throw system, this should throw a reply nosuch channel or wtv
Channel *Server::getChannel(std::string name)
{
	std::vector<Channel>::iterator it = _channels.begin();

	while (it != _channels.end())
	{
		if (it->getName() == name)
			return &(*it);
		it++;
	}
	return nullptr;
}
//if we change to throw system, this should throw a reply nosuch user or wtv

Client *Server::getClient(std::string name)
{
	ClientMap::iterator it = _clients.begin();

	while (it != _clients.end())
	{
		if (it->second->getNickName() == name)
			return it->second;
		it++;
	}
	return nullptr;
}