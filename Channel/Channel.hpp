#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "../Classes.hpp"
#include "../Libs.hpp"

enum ClientChannelStatus
{
	Status_Connected,
	Status_Op,
	Status_Invited,
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
		int getClientCount();

		ClientChannelStatus getClientStatus(Client &client);
		std::string getModes(Client &client);
		bool isOnChannel(Client &client);
		void joinClient(Client &client, std::string password);
		void kickClient(Client &op, Client &user, std::string reason);
		void partClient(Client &client, std::string reason);
		void quitClient(Client &client);
		void changeTopic(Client &client, std::string topic);
		void getTopic(Client &client);
		void removeClient(Client &client);
		void who(Client &client);
		void changeMode(Client &client, Args args);
		void inviteClient(Client &op, Client &user);
		void broadcastMessage(Client &client, std::string cmd, std::string message, bool sendSender);
	
	private:
		Server* _server;
		std::string _name;
		std::string _topicVal;
		std::string _passwordVal;
		std::string _mode;
		std::map<Client*, ClientChannelStatus> _clients;
		bool _inviteOnly;
		bool _topicLocked;
		int _clientCount;
		int _opCount;
		int _clientLimit;
		
		void sendJoinMessage(Client &client);
		void setModeRpl(char c, bool adding, std::string val, std::string &options, std::string &values);
		void updateMode(char c, bool adding);
		bool consumeArg(Client &client, Args args, size_t &cur, std::string &out);
		bool toggleOp(Client &client, std::string tarName, bool on);
		bool test_senderOnChannel(Client &c);
		bool test_isOperator(Client &c);
		bool test_canInvite(Client &c);
		bool test_targetNotOnChannel(Client &c, Client &tar);
		bool test_clientCanEnter(Client &c, std::string password);
		bool test_canChangeTopic(Client &c);
		bool test_targetOnChannel(Client &c, Client &tar);
		void leaveClient(Client &client, std::string source, std::string reason);

};
#endif