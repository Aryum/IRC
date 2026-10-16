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
		std::string _passwordVal;
		std::map<Client*, ClientChannelStatus> _clients;
		bool consumeArg(Args &args, size_t &cur, std::string &out);
		void toggleOp(std::string name, bool on);

		bool _inviteOnly;
		int _clientLimit;
};
#endif