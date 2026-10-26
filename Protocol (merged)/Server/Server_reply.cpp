#include "Server.hpp"

//args-> channel topic
void Server::rpl_NOTOPIC(Client &client, std::string channel)
{
	serverToClient(client.getSocket(), "331 " + channel + " :No topic is set");
}
//args-> channel + topic
void Server::rpl_TOPIC(Client &client, std::string channel,std::string topic)
{
	serverToClient(client.getSocket(),"332 " + channel + " :" + topic);
}

void Server::rpl_INVITING(Client &client, std::string invited,std::string channel)
{
	serverToClient(client.getSocket(),"341 " + client.get(Name_Nick) + " " + invited + " " + channel);
}

void Server::rpl_WELCOME(Client &client, std::string date)
{
	std::string nick = client.get(Name_Nick);
	std::string user = client.get(Name_User);
	std::string host = client.get(Name_Host);

	serverToClient(client.getSocket(), "001 " + nick + " :Welcome to the Internet Relay Network!" + user + "@" + host);
	serverToClient(client.getSocket(), "002 " + nick + " Your host is IRC, running version 1.0");
	serverToClient(client.getSocket(), "003 " + nick + " This server was created " + date);
	serverToClient(client.getSocket(), "004 " + nick + " IRC 1.0 itkol");
}