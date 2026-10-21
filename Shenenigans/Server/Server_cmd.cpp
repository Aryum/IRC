#include "Server.hpp"
void Server::cmd_PASS(Client &client, Args args)
{
	if (client.isRegistred())
		return err_ALREADYREGISTRED(client);
	else if (args.size() == 0)
		return err_NEEDMOREPARAMS(client, "PASS");
	if (args[0] != _password)
		return err_PASSWDMISMATCH(client);
	client.setPasswordVerified();
}

void Server::cmd_NICK(Client &client, Args args)
{
	if (args.size() == 0)
		return err_NONICKNAMEGIVEN(client);
	if (args[0].length() < 1 || args[0].length() > 9)
		return err_ERRONEUSNICKNAME(client, args[0]);
	char c = args[0][0];
	if (!std::isalpha(c) && c != '_' && c != '.')
		return err_ERRONEUSNICKNAME(client, args[0]);
	ClientMap::iterator it = _clients.begin();
	bool newConnection = false;
	while (it != _clients.end())
	{
		if (it->first == client.getSocket())
			newConnection = false;
		if (it->second->get(Name_Nick) == args[0])
			return err_NICKNAMEINUSE(client, args[0]);
		it++;
	}
	client.changeNick(args[0]);
	if (client.isRegistred())
		rpl_WELCOME(client, _date);
}
void Server::cmd_USER(Client &client, Args args)
{
	if (args.size() < 4)
		return err_NEEDMOREPARAMS(client, "USER");
	ClientMap::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->first == client.getSocket())
			return err_ALREADYREGISTRED(client);
		it++;
	}
	client.setNames(args[0], args[1], args[3]);

	_clients.insert(std::make_pair(client.getSocket(), &client));
}



void Server::cmd_QUIT(Client &client, Args args)
{
	_clients.erase(client.getSocket());
	std::string reason;
	if (args.size() != 0)
		reason = args[0];
	else
		reason = "Quit";
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i].getClientStatus(client) != Status_None)
		{
			_channels[i].broadcastMessage(client, reason);
			_channels[i].partClient(client);
		}
	}
	//close connection
}

void Server::cmd_JOIN(Client &client, Args args)
{
	if (args.size() < 1)
	{
		err_NEEDMOREPARAMS(client, "JOIN");
		return ;
	}
	size_t channelIdx = 0;
	size_t keyIdx = 0;

	while (channelIdx != std::string::npos)
	{
		std::string name = nextChar(args[0], channelIdx, ',', true);
		std::string password = nextChar(args[0], channelIdx, ',', true);

		Channel *channel = getChannel(name);
		std::string reply;
		if (channel == nullptr)
			_channels.push_back(Channel(client, name));
		else
			channel->joinClient(client, password);
		
	}
}

void Server::cmd_PART(Client &client, Args args)
{
	if (args.size() < 1)
	{
		err_NEEDMOREPARAMS(client, "PART");
		return ;
	}
	size_t channelIdx = 0;
	bool foundChannel = false;
	while (channelIdx != std::string::npos)
	{
		std::string name = nextChar(args[0], channelIdx, ',', true);
		Channel *channel = nullptr;
		if (getChannel(client, name, &channel))
			return ;
		else
			channel->partClient(client);
	}
}

void Server::cmd_KICK(Client &client, Args args)
{
	if (args.size() < 2)
		return err_NEEDMOREPARAMS(client, "KICK");

	size_t channelIdx = 0;
	size_t userIdx = 0;
	bool foundChannel = false;
	while (channelIdx != std::string::npos)
	{
		std::string channelName = nextChar(args[0], channelIdx, ',', true);
		std::string nickName = nextChar(args[1], userIdx, ',', true);
		if (userIdx == std::string::npos && nickName == "")
			break ;
		Channel *channel = nullptr;
		Client *user = nullptr;
		if (!getChannel(client, channelName, &channel) || !getClient(client, nickName, &user));
			continue ; 
		channel->kickClient(client, *user, args.size() >= 3 ? args[2] : "");
	}
}

void Server::cmd_INVITE(Client &client, Args args)
{
	if (args.size() < 2)
		return err_NEEDMOREPARAMS(client, "INVITE");
	Client *user = nullptr;
	Channel *channel = nullptr;
	if (!getClient(client, args[1], &user) || !getChannel(client, args[2], &channel))
		return ; 
	channel->inviteClient(client, *user);
}

void Server::cmd_TOPIC(Client &client, Args args)
{
	if (args.size() < 1)
		return err_NEEDMOREPARAMS(client, "TOPIC");
	Channel *channel = nullptr;
	if (!getChannel(client, args[0], &channel))
		return ;
	if (args.size() == 1)
		channel->getTopic(client);
	else
		channel->changeTopic(client, args[1]);
}

void Server::cmd_MODE(Client &client, Args args)
{
	if (args.size() < 2)
		return err_NEEDMOREPARAMS(client, "MODE");
	Channel *channel = nullptr;
	if (!getChannel(client, args[0], &channel))
		return ;
	channel->changeMode(client, args);
}
