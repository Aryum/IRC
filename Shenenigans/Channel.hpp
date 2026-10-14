#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <string>
#include <vector>
#include "Client.hpp"
#include "placeholder.hpp"

enum ClientStatus
{
	Connected,
	Banned,
	Op,
	None
};


class Channel
{
	public:
		Channel();


		std::string  getName();

		//returns reply
		std::string joinClient(Client &client, std::string password);
		std::string kickClient(Client &op, std::string name, std::string reason);
		std::string partClient(Client &client);
		bool hasClient(Client &client);
		bool isOperator(Client &client);
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

		std::map<Client*, ClientStatus> _clients;


		bool _inviteOnly;
		ClientStatus getClientStatus(Client &client);
};
#endif