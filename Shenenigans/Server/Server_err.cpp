#include "Server.hpp"

void err_PASSWDMISMATCH(const Client &client)
{
	client.sendReply("464 * :Password incorrect");
}
void err_UNKNOWNCOMMAND(const Client &client, std::string cmd)
{
	client.sendReply("421 " + cmd + " :Unknown command");
}
//args-> empty
void err_ALREADYREGISTRED(const Client &client)
{
	client.sendReply("462 :You may not reregister");
}

//args-> cmd name
void err_NEEDMOREPARAMS(const Client &client, std::string cmd)
{
	client.sendReply("461 " + cmd + " :Not enough parameters");
}

//args-> nickname
void err_NICKNAMEINUSE(const Client &client, std::string nick)
{
	client.sendReply("433 " + nick + " :Nickname is already in use");
}

//args-> empty
void err_NONICKNAMEGIVEN(const Client &client)
{
	client.sendReply("431 :No nickname given");
}
//args-> channel name
void err_NOSUCHCHANNEL(const Client &client, std::string channel)
{
	client.sendReply("403 " + channel + " :No such channel");
}

void err_ERRONEUSNICKNAME(const Client &client, std::string nick)
{
	client.sendReply("401 " + nick + " :No such nick/channel");
}
//args-> nick name
void err_NOSUCHNICK(const Client &client, std::string nick)
{
	client.sendReply("432 " + nick + " :Erroneus nickname");
}

void err_NOTONCHANNEL(const Client &client, std::string channel)
{
	client.sendReply("442 " + client.get(Name_Nick) + " " + channel + " :You're not on that channel");
}

void err_UNKNOWNMODE(const Client &client, char c)
{
	client.sendReply("472 " + client.get(Name_Nick) + " " + std::string(1, c) + " :is unknown mode char to me");
}

void err_CHANOPRIVSNEEDED(const Client &client, std::string channel)
{
	client.sendReply("482 " + client.get(Name_Nick) + " " + channel + " :You're not channel operator");
}

void err_CHANNELISFULL(const Client &client, std::string channel)
{
	client.sendReply("471 " + client.get(Name_Nick) + " " + channel + " :Cannot join channel (+l)");
}
void err_NOTONCHANNEL(const Client &client, std::string channel)
{
	client.sendReply("442 " + client.get(Name_Nick) + " " + channel + " :You're not on that channel");
}
void err_INVITEONLYCHAN(const Client &client, std::string channel)
{
	client.sendReply("473 " + client.get(Name_Nick) + " " + channel + " :Cannot join channel (+i)");
}

void err_BADCHANNELKEY(const Client &client, std::string channel)
{
	client.sendReply("475 " + client.get(Name_Nick) + " " + channel + " :Cannot join channel (+k)");
}