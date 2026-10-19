#include "Server.hpp"

//args-> channel topic
void rpl_NOTOPIC(Client &client, std::string channel)
{
	client.sendReply(331, channel + " :No topic is set");
}
//args-> channel + topic
void rpl_TOPIC(Client &client, std::string channel,std::string topic)
{
	client.sendReply(332, channel + " :" + topic);
}

void rpl_INVITING(Client &client, std::string invited,std::string channel)
{
	client.sendReply(341, client.getNickName() + " " + invited + " " + channel);
}