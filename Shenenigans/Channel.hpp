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
class Channel;

typedef bool (Channel::*Check)(const Client &a);

class Channel
{
	public:
		Channel(const Client &client,const std::string &name);


		std::string  getName();

		void joinClient(Client &client, std::string password);
		void kickClient(Client &op, Client &user, std::string reason);
		void partClient(Client &client);
		void changeTopic(Client &client, std::string topic);
		void getTopic(Client &client);
		void changeMode(Client &client, Args args);
		void inviteClient(Client &op, Client &user);
		ClientChannelStatus getClientStatus(const Client &client);
		void broadcastMessage(std::string message);
		bool isInviteOnly();
	
	private:
		std::string _name;
		std::string _topicVal;
		std::string _passwordVal;
		std::string _clientPassword;
		std::map<const Client*, ClientChannelStatus> _clients;
		bool _inviteOnly;
		int _clientLimit;
		
		bool consumeArg(Client &client, Args &args, size_t &cur, std::string &out);
		void toggleOp(std::string name, bool on);
		void toggleOp(Client &client, std::string name, bool on);
		bool isOnChannel(const Client &c);
		bool isOperator(const Client &c);
		bool fullChannel(const Client &c);
		bool canInvite(const Client &c);
		bool checkKey(const Client &c);
		bool hasError(const Client &c, Check *checks);
};
#endif