#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <string>
#include <vector>
#include "../Classes.hpp"

enum ClientChannelStatus
{
	Status_Connected,
	Status_Op,
	Status_None
};
class Channel;

typedef bool (Channel::*Check)(Client &a);

class Channel
{
	public:
		Channel(Server& server, Client &client, std::string &name);


		std::string  getName();

		void joinClient(Client &client, std::string password);
		void kickClient(Client &op, Client &user, std::string reason);
		void partClient(Client &client);
		void changeTopic(Client &client, std::string topic);
		void getTopic(Client &client);
		void changeMode(Client &client, Args args);
		void inviteClient(Client &op, Client &user);
		ClientChannelStatus getClientStatus(Client &client);
		void broadcastMessage(Client &client, std::string message);
		bool isInviteOnly();
	
	private:
		std::string _name;
		std::string _topicVal;
		std::string _passwordVal;
		std::string _clientPassword;
		std::map<Client*, ClientChannelStatus> _clients;
		Server& _server;
		bool _inviteOnly;
		int _clientLimit;
		
		bool consumeArg(Client &client, Args &args, size_t &cur, std::string &out);
		void toggleOp(Client &client, std::string name, bool on);
		bool check_isOnChannel(Client &c);
		bool check_isOperator(Client &c);
		bool check_fullChannel(Client &c);
		bool check_canInvite(Client &c);
		bool check_key(Client &c);
		bool checkMultiple(Client &c, Check *checks);
};
#endif