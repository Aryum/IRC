#include "Server.hpp"

void Server::rpl_NOTOPIC(Client &client, std::string channel)
{
	serverToClient(client, "331 " + client.get(Name_Nick) + " " +  channel + " :No topic is set");
}

void Server::rpl_TOPIC(Client &client, std::string channel,std::string topic)
{
	serverToClient(client, "332 " + client.get(Name_Nick) + " " + channel + " :" + topic);
}

void Server::rpl_INVITING(Client &client, std::string invited,std::string channel)
{
	serverToClient(client,"341 " + client.get(Name_Nick) + " " + invited + " " + channel);
}

void Server::rpl_WELCOME(Client &client, std::string date)
{
	std::string nick = client.get(Name_Nick);
	
	client.setWelcome();
	serverToClient(client, "001 " + nick + " :Welcome to the Internet Relay Network! " + client.get(Name_Full));
	serverToClient(client, "002 " + nick + " :Your host is IRC, running version 1.0");
	serverToClient(client, "003 " + nick + " :This server was created " + date);
	serverToClient(client, "004 " + nick + " IRC 1.0 o itkol");
}

void Server::rpl_CHANNELMODEIS(Client &client, Channel &channel)
{
	serverToClient(client, "324 " + client.get(Name_Nick) + " " + channel.getName() + " +" + channel.getModes(client));
}

void Server::rpl_WHOREPLY(Client &client, Channel *channel, Client &user)
{
	std::string channelName = "*";
	std::string channelStatus = "H";
	if (channel != nullptr)
	{
		channelName = channel->getName();
		channelStatus += ((channel->getClientStatus(user) == Status_Op) ? "@" : "");
	}
	serverToClient(client, "352 " + client.get(Name_Nick) + " " + channelName + " " + \
					user.get(Name_User) + " " + user.get(Name_Host) + " IRC " + user.get(Name_Nick) + " " + \
					channelStatus + " " + \
					":0 " + user.get(Name_Real));
}

void Server::rpl_ENDOFWHO(Client &client, std::string query)
{
	serverToClient(client, "315 " + client.get(Name_Nick) + " " + query + " :End of WHO list");
}