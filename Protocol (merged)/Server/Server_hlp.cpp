#include "Server.hpp"

//move to a helper file probably need to make it return if it has a key or not, if it idx was already npos
std::string nextChar(const std::string &input, size_t &idx, char c, bool prefix)
{
	size_t curIdx;
	std::string ret;
	if (!prefix && input[idx] == ':')
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

std::string nextChar(const std::string &input, size_t &idx, char c)
{
	return nextChar(input, idx, c, true);
}
//already sends message if doesnt find
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
//already sends message if doesnt find
bool Server::getChannel(Client &client, std::string name, Channel **out)
{
	*out = getChannel(name);
	if (*out == nullptr)
		err_NOSUCHCHANNEL(client, name);
	return (*out != nullptr);
}
//if we change to throw system, this should throw a reply nosuch user or wtv

Client *Server::getClient(std::string name)
{
	ClientMap::iterator it = _clients.begin();

	while (it != _clients.end())
	{
		if (it->second->get(Name_Nick) == name)
			return it->second;
		it++;
	}
	return nullptr;
}
bool Server::getClient(Client &client, std::string name, Client **out)
{
	*out = getClient(name);
	if (*out == nullptr)
		err_NOSUCHNICK(client, name);
	return (*out != nullptr);
}

