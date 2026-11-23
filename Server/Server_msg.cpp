#include "Server.hpp"

void Server::clientToPeers(Client& client, const std::string& message)
{
	ClientMap::iterator it = _clients.begin();

	for (it = _clients.begin(); it !=  _clients.end(); it++)
	{
		Client *curClient = it->second;
		if (*curClient == client)
			continue;
		for (size_t i = 0; i < _channels.size(); i++)
		{
			Channel curChannel = _channels[i];
			if (!curChannel.isOnChannel(client) || !curChannel.isOnChannel(*curClient))
				continue ;
			clientToClient(*curClient, client, message);
			break ;
		}
	}
}

void Server::serverToClient(Client& target, const std::string& message)
{
	std::cout << YELLOW;
	sendToClient(target, ":IRC", message);
}
void Server::clientToClient(Client& target, Client& sender, const std::string& message)
{
	std::cout << GREEN; 
	sendToClient(target, ":" + sender.get(Name_Full), message);
}

void Server::sendToClient(Client &client, const std::string& sender, const std::string& message)
{
	client.appendOutput(sender + " " + message + "\r\n");

	std::cout << sender << " message to " << client.get(Name_Nick) << ": " << WHITE << message  << std::endl;

	for (std::vector<pollfd>::size_type i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].fd == client.getSocket())
		{
			_pollFds[i].events |= POLLOUT;
			break;
		}
	}
}