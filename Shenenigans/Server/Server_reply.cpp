#include "Server.hpp"

void Server::err_ALREADYREGISTRED(Client &client, Args args)
{
	(void) args;
	client.sendReply(":You may not reregister");
}

void Server::err_NEEDMOREPARAMS(Client &client, Args args)
{
	client.sendReply(args[0] + ":Not enough parameters");
}

void Server::err_NICKCOLLISION(Client &client, Args args)
{
	client.sendReply(args[0] + ":Nickname collision KILL");
}
void Server::err_NONICKNAMEGIVEN(Client &client, Args args)
{
	(void) args;
	client.sendReply(":No nickname given");
}
void Server::err_NOSUCHCHANNEL(Client &client, Args args)
{
	client.sendReply(args[0] + " :No such channel");
}

void Server::err_NOSUCHNICK(Client &client, Args args)
{
	client.sendReply(args[0] + " :No such nick/channel");
}
void Server::rpl_NOTOPIC(Client &client, Args args)
{
	client.sendReply(args[0] + " :No topic is set");
}
void Server::rpl_TOPIC(Client &client, Args args)
{
	client.sendReply(args[0] + " :" + args[1]);
}