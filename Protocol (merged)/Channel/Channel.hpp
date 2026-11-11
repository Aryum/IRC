#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "../Classes.hpp"
#include "../Libs.hpp"

enum ClientChannelStatus
{
	Status_Connected,
	Status_Invited,
	Status_Op,
	Status_None
};

class Channel;

typedef bool (Channel::*Check)(Client &);
class Channel
{
	public:
		Channel(Server* server, Client &client, std::string &name);
		bool operator==(const Channel &other) const;


		std::string getName();
		ClientChannelStatus getClientStatus(Client &client);
		std::string getModes(Client &client);
		void joinClient(Client &client, std::string password);
		void kickClient(Client &op, Client &user, std::string reason);
		void partClient(Client &client, std::string reason);
		void changeTopic(Client &client, std::string topic);
		void getTopic(Client &client);
		void removeClient(Client &client);
		void who(Client &client);
		void changeMode(Client &client, Args args);
		void inviteClient(Client &op, Client &user);
		void broadcastMessage(Client &client, std::string cmd, std::string message, bool sendSender);
		bool openToEnter(Client &c);
	
	private:
		Server* _server;
		std::string _name;
		std::string _topicVal;
		std::string _passwordVal;
		std::string _clientPassword;
		std::string _mode;
		std::map<Client*, ClientChannelStatus> _clients;
		bool _inviteOnly;
		bool _topicLocked;
		size_t _clientCount;
		long unsigned int _clientLimit;
		
		void updateMode(char c, bool adding);
		bool consumeArg(Client &client, Args args, size_t &cur, std::string &out);
		void toggleOp(Client &client, std::string tarName, bool on);
		bool check_isOnChannel(Client &c);
		bool check_isOperator(Client &c);
		bool check_fullChannel(Client &c);
		bool check_canInvite(Client &c);
		bool check_key(Client &c);
		bool checkMultiple(Client &c, Check *checks);
		bool notOnChannel(Client &c, Client &tar);

};
#endif