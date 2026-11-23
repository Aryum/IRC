#include "Server.hpp"

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

std::string strToLower(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
		str[i] += (str[i] >= 'A' && str[i] <= '^') * 32;
	return str;
}

std::string strToUpper(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
		str[i] -= (str[i] >= 'a' && str[i] <= 'z') * 32;
	return str;
}
Channel *Server::getChannel(std::string name)
{
	std::vector<Channel>::iterator it = _channels.begin();

	name = strToLower(name);
	while (it != _channels.end())
	{
		if (!it->getClientCount() == 0)
		{
			if(strToLower(it->getName()) == name)
				return &(*it);
		} 
		it++;
	}
	return NULL;
}

bool Server::getChannel(Client &client, std::string name, Channel **out)
{
	*out = getChannel(name);
	if (*out == NULL)
		err_NOSUCHCHANNEL(client, name);
	return (*out != NULL);
}

Client *Server::getClient(std::string name)
{
	ClientMap::iterator it = _clients.begin();

	strToLower(name);
	while (it != _clients.end())
	{
		if (strToLower(it->second->get(Name_Nick)) == name)
			return it->second;
		it++;
	}
	return NULL;
}

bool Server::getClient(Client &client, std::string name, Client **out)
{
	*out = getClient(name);
	if (*out == NULL)
		err_NOSUCHNICK(client, name);
	return (*out != NULL);
}

void Server::quitClient(Client & client, std::string reason)
{
	clientToPeers(client, "QUIT :" + reason);
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i].isOnChannel(client))
			_channels[i].quitClient(client);
	}
}