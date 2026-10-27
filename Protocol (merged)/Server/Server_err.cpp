#include "Server.hpp"

void Server::err_PASSWDMISMATCH(Client &client)
{
	serverToClient(client , "464 * :Password incorrect");
}
void Server::err_UNKNOWNCOMMAND(Client &client, std::string cmd)
{
	serverToClient(client , "421 " + cmd + " :Unknown command");
}
//args-> empty
void Server::err_ALREADYREGISTRED(Client &client)
{
	serverToClient(client , "462 :You may not reregister");
}

//args-> cmd name
void Server::err_NEEDMOREPARAMS(Client &client, std::string cmd)
{
	serverToClient(client , "461 " + cmd + " :Not enough parameters");
}

//args-> nickname
void Server::err_NICKNAMEINUSE(Client &client, std::string nick)
{
	serverToClient(client , "433 " + nick + " :Nickname is already in use");
}

//args-> empty
void Server::err_NONICKNAMEGIVEN(Client &client)
{
	serverToClient(client , "431 :No nickname given");
}
//args-> channel name
void Server::err_NOSUCHCHANNEL(Client &client, std::string channel)
{
	serverToClient(client , "403 " + channel + " :No such channel");
}

void Server::Server::err_ERRONEUSNICKNAME(Client &client, std::string nick)
{
	serverToClient(client , "432 " + nick + " :Erroneus nickname");
}
//args-> nick name
void Server::err_NOSUCHNICK(Client &client, std::string nick)
{
	serverToClient(client , "401 " + nick + " :No such nick/channel");
}


void Server::err_UNKNOWNMODE(Client &client, char c)
{
	serverToClient(client , "472 " + client.get(Name_Nick) + " " + std::string(1, c) + " :is unknown mode char to me");
}

void Server::err_CHANOPRIVSNEEDED(Client &client, std::string channel)
{
	serverToClient(client , "482 " + client.get(Name_Nick) + " " + channel + " :You're not channel operator");
}

void Server::err_CHANNELISFULL(Client &client, std::string channel)
{
	serverToClient(client , "471 " + client.get(Name_Nick) + " " + channel + " :Cannot join channel (+l)");
}
void Server::err_NOTONCHANNEL(Client &client, std::string channel)
{
	serverToClient(client , "442 " + client.get(Name_Nick) + " " + channel + " :You're not on that channel");
}
void Server::err_INVITEONLYCHAN(Client &client, std::string channel)
{
	serverToClient(client , "473 " + client.get(Name_Nick) + " " + channel + " :Cannot join channel (+i)");
}

void Server::err_BADCHANNELKEY(Client &client, std::string channel)
{
	serverToClient(client , "475 " + client.get(Name_Nick) + " " + channel + " :Cannot join channel (+k)");
}

void Server::err_CANNOTSENDTOCHAN(Client &client, std::string channel)
{
	serverToClient(client , "404 " + channel + " :Cannot send to channel");

}

void Server::err_NORECIPIENT(Client &client)
{
	serverToClient(client , "411 :No recipient given (PRIVMSG)");
}


void Server::err_NOTEXTTOSEND(Client &client)
{
	serverToClient(client , "412 :No text to send");
}