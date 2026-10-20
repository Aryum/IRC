#include "Server.hpp"

//args-> channel topic
void rpl_NOTOPIC(Client &client, std::string channel)
{
	client.sendReply("331 " + channel + " :No topic is set");
}
//args-> channel + topic
void rpl_TOPIC(Client &client, std::string channel,std::string topic)
{
	client.sendReply("332 " + channel + " :" + topic);
}

void rpl_INVITING(Client &client, std::string invited,std::string channel)
{
	client.sendReply("341 " + client.get(Name_Nick) + " " + invited + " " + channel);
}

void rpl_WELCOME(const Client &client, std::string date)
{
	std::string nick = client.get(Name_Nick);
	std::string user = client.get(Name_User);
	std::string host = client.get(Name_Host);

	client.sendReply("001 " + nick + ":Welcome to the Internet Relay Network!" + user + "@" + host);
	client.sendReply("002 " + nick + "Your host is IRC, running version 1.0");
	client.sendReply("003 " + nick + "This server was created " + date);
	client.sendReply("004 " + nick + "IRC 1.0 itkol");
}