#include "Server.hpp"

void Server::err_PASSWDMISMATCH(Client &client)
{
	serverToClient(client , "464 * :Password incorrect");
}
void Server::err_UNKNOWNCOMMAND(Client &client, std::string cmd)
{
	serverToClient(client , "421 " + client.get(Name_Nick) + " " + cmd + " :Unknown command");
}

void Server::err_ALREADYREGISTRED(Client &client)
{
	serverToClient(client , "462 "+ client.get(Name_Nick) + " :You may not reregister");
}

void Server::err_NEEDMOREPARAMS(Client &client, std::string cmd)
{
	serverToClient(client , "461 " + client.get(Name_Nick) + " " + cmd + " :Not enough parameters");
}

void Server::err_NICKNAMEINUSE(Client &client, std::string nick)
{
	serverToClient(client , "433 " + client.get(Name_Nick) + " " + nick + " :Nickname is already in use");
}

void Server::err_NONICKNAMEGIVEN(Client &client)
{
	serverToClient(client , "431 " + client.get(Name_Nick) + " :No nickname given");
}

void Server::err_NOSUCHCHANNEL(Client &client, std::string channel)
{
	serverToClient(client , "403 " + client.get(Name_Nick) + " " + channel + " :No such channel");
}

void Server::Server::err_ERRONEUSNICKNAME(Client &client, std::string nick)
{
	serverToClient(client , "432 " + client.get(Name_Nick) + " " + nick + " :Erroneus nickname");
}

void Server::err_NOSUCHNICK(Client &client, std::string nick)
{
	serverToClient(client , "401 " + client.get(Name_Nick) + " " + nick + " :No such nick/channel");
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
	serverToClient(client , "404 " + client.get(Name_Nick) + " " + channel + " :Cannot send to channel");

}

void Server::err_NORECIPIENT(Client &client)
{
	serverToClient(client , "411 " + client.get(Name_Nick) + " :No recipient given (PRIVMSG)");
}


void Server::err_NOTEXTTOSEND(Client &client)
{
	serverToClient(client , "412 " + client.get(Name_Nick) + " :No text to send");
}

void Server::err_USERNOTINCHANNEL(Client &client, std::string user, std::string channel)
{
	serverToClient(client , "441 " + user + " " + channel + " :They aren't on that channel");
}

void Server::err_USERONCHANNEL(Client &client, Client &tar, std::string channel)
{
	serverToClient(client , "443 " + tar.get(Name_Nick) + " " + channel + " :is already on channel");
}

void Server::err_INPUTTOOLONG(Client &client)
{
	serverToClient(client , "417 " + client.get(Name_Nick) + " :Input line was too long");
}

void Server::err_INVALIDKEY(Client &client, std::string channel)
{
	serverToClient(client , "525 " + client.get(Name_Nick) + " " + channel + " :Key is not well-formed");

}