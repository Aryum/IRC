#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <string>
#include <vector>
#include "Client.hpp"
#include "placeholder.hpp"

class Channel
{
	public:
		Channel();


		std::string  getName();

		//returns reply
		std::string addClient(Client &client, std::string password);
		std::string removeClient(Client &client);

		bool hasClient(Client &client);
		void broadcastMessage(std::string message);
		bool isInviteOnly();
		bool comparePassword(std::string); // if it has none just accept else compare
		bool isClientBanned(Client& client);

	private:
		std::string _name;
		std::string _topicVal;
		bool _topicSet;
		std::string _passwordVal;
		bool _passwordSet;

		std::vector<Client*> _connectedClients;
		std::vector<Client*> _bannedClients;

		bool _inviteOnly;


};
#endif