#ifndef CLASSES_HPP
#	define CLASSES_HPP

class Server;
class Client;
class Channel;

#include "Server/Server.hpp"
#include "Client/Client.hpp"
#include "Channel/Channel.hpp"

void err_PASSWDMISMATCH(const Client &client);
void err_UNKNOWNCOMMAND(const Client &client, std::string cmd);
void err_ALREADYREGISTRED(const Client &client);
void err_NEEDMOREPARAMS(const Client &client, std::string cmd);
void err_NICKNAMEINUSE(const Client &client, std::string nick);
void err_NONICKNAMEGIVEN(const Client &client);
void err_NOSUCHCHANNEL(const Client &client, std::string channel);
void err_NOSUCHNICK(const Client &client, std::string nick);
void err_NOTONCHANNEL(const Client &client, std::string channel);
void err_UNKNOWNMODE(const Client &client, char c);
void err_CHANOPRIVSNEEDED(const Client &client, std::string channel);




void err_CHANNELISFULL(const Client &client, std::string channel);
void err_NOTONCHANNEL(const Client &client, std::string channel);
void err_INVITEONLYCHAN(const Client &client, std::string channel);
void err_BADCHANNELKEY(const Client &client, std::string channel);
void rpl_NOTOPIC(const Client &client, std::string channel);
void rpl_TOPIC(const Client &client, std::string channel,std::string topic);
void rpl_INVITING(const Client &client, std::string invited,std::string channel);




void err_ERRONEUSNICKNAME(const Client &client, std::string channel);
void rpl_WELCOME(const Client &client, std::string date);
#endif