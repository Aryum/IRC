#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <string>
#include <vector>
#include "Client.hpp"
#include "placeholder.hpp"

enum ClientChannelStatus
{
	Status_Connected,
	Status_Op,
	Status_None
};


class Channel
{
	public:
		Channel();


		std::string  getName();

		//returns reply
		std::string joinClient(Client &client, std::string password);
		std::string kickClient(Client &op, Client &user, std::string reason);
		std::string partClient(Client &client);
		std::string changeTopic(Client &client, std::string topic);
		std::string getTopic(Client &client);
		std::string changeMode(Client &client, Args args);

		std::string Channel::inviteClient(Client &op, Client &user);

		void broadcastMessage(std::string message);
		bool isInviteOnly();
		bool comparePassword(std::string); // if it has none just accept else compare
		ClientChannelStatus getClientStatus(Client &client);
	
	private:
		std::string _name;
		std::string _topicVal;
		bool _topicSet;
		std::string _passwordVal;
		bool _passwordSet;
		std::map<Client*, ClientChannelStatus> _clients;
		bool _inviteOnly;
};
#endif