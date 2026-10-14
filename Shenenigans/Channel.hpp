#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <string>
#include <vector>
#include "Client.hpp"

class Channel
{
	public:
		Channel();

		bool hasClient(Client &client);
		void addClient(Client &client);
		void removeClient(Client &client);
		void broadcastMessage(std::string message);

	private:
		std::string _topic;
		std::vector<Client*> _connectedClients;



};
#endif