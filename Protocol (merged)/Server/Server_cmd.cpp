#include "Server.hpp"

void Server::cmd_PASS(Client &client, Args args)
{
	if (client.isRegistred())
		return err_ALREADYREGISTRED(client);
	else if (args.size() == 0)
		return (err_NEEDMOREPARAMS(client, "PASS"), client.setQuiting());
	if (args[0] != _password)
		return (err_PASSWDMISMATCH(client), client.setQuiting());
	client.setPasswordVerified();
	if (client.isRegistred())
		rpl_WELCOME(client, _date);
}

static bool parseNick(std::string name)
{
	if (!std::isalpha(name[0]))
		return false;
	std::string allowed = "-[]\\`^{}";
	for (size_t i = 1; i < name.size(); i++)
	{
		if (std::isalpha(name[i]) || std::isdigit(name[i]))
			continue;
		if (allowed.find(name[i]) != std::string::npos)
			continue;
		return false;
	}
	return true;
}

void Server::cmd_NICK(Client &client, Args args)
{
	if (args.size() == 0)
		return err_NONICKNAMEGIVEN(client);
	if (args[0].length() < 1 || args[0].length() > 9)
		return err_ERRONEUSNICKNAME(client, args[0]);
	if (!parseNick(args[0]))
		return err_ERRONEUSNICKNAME(client, args[0]);
	ClientMap::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->second->get(Name_Nick) == args[0])
			return err_NICKNAMEINUSE(client, args[0]);
		it++;
	}
	client.cmd_NICK(args[0]);
	if (client.isRegistred() && !client.isWelcome())
	{
		_clients.insert(std::make_pair(client.getSocket(), &client));
		rpl_WELCOME(client, _date);
	}
}

void Server::cmd_USER(Client &client, Args args)
{
	if (args.size() < 4)
		return err_NEEDMOREPARAMS(client, "USER");
	ClientMap::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->first == client.isRegistred())
			return err_ALREADYREGISTRED(client);
		it++;
	}
	for (size_t i = 0; i < args.size(); i++)
		std::cout << MAGENTA << args[i] << WHITE<< "\n";
	
	client.cmd_USER(args[0], args[1], args[3]);
	if (client.isRegistred() && !client.isWelcome())
		rpl_WELCOME(client, _date);
}

void Server::cmd_QUIT(Client &client, Args args)
{
	std::string reason;
	if (args.size() != 0)
		reason = args[0];
	else
		reason = "Quit";
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i].getClientStatus(client) != Status_None)
			_channels[i].partClient(client, reason);
	}
	client.setQuiting();
}

static bool parseChannel(std::string name)
{
	return (name[0] == '&' || name[0] == '#') && name.size() < 200;
}

void Server::cmd_JOIN(Client &client, Args args)
{
	if (args.size() < 1)
		return err_NEEDMOREPARAMS(client, "JOIN");
	size_t channelIdx = 0;
	size_t keyIdx = 0;

	while (channelIdx != std::string::npos)
	{
		std::string name = nextChar(args[0], channelIdx, ',');
		std::string password = "";
		if (args.size() > 1)
			password = nextChar(args[1], keyIdx, ',');
		if (!parseChannel(name))
			err_NOSUCHCHANNEL(client, name);
		else
		{
			Channel *channel = getChannel(name);
			std::string reply;
			if (channel == nullptr)
				_channels.push_back(Channel(this, client, name));
			else
				channel->joinClient(client, password);
		}
	}
}

void Server::cmd_PART(Client &client, Args args)
{
	if (args.size() < 1)
		return err_NEEDMOREPARAMS(client, "PART");
	size_t channelIdx = 0;
	while (channelIdx != std::string::npos)
	{
		std::string name = nextChar(args[0], channelIdx, ',');
		Channel *channel = nullptr;
		if (!getChannel(client, name, &channel))
			return ;
		else
			channel->partClient(client, args.size() < 2 ? "" : args[1]);
	}
}

void Server::cmd_KICK(Client &client, Args args)
{
	if (args.size() < 2)
		return err_NEEDMOREPARAMS(client, "KICK");

	size_t channelIdx = 0;
	size_t userIdx = 0;
	while (channelIdx != std::string::npos)
	{
		std::string channelName = nextChar(args[0], channelIdx, ',');
		std::string nickName = nextChar(args[1], userIdx, ',');
		if (userIdx == std::string::npos && nickName == "")
			break ;
		Channel *channel = nullptr;
		Client *user = nullptr;
		if (!getChannel(client, channelName, &channel) || !getClient(client, nickName, &user))
			continue ; 
		channel->kickClient(client, *user, args.size() >= 3 ? args[2] : "");
	}
}

void Server::cmd_INVITE(Client &client, Args args)
{
	if (args.size() < 2)
		return err_NEEDMOREPARAMS(client, "INVITE");
	Client *user = nullptr;
	Channel *channel = getChannel(args[1]);
	if (!getClient(client, args[0], &user) || channel == nullptr)
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
	if (args.size() < 1)
		return err_NEEDMOREPARAMS(client, "MODE");
	
	Channel *channel = nullptr;
	if (!getChannel(client, args[0], &channel))
		return ;
	if (args.size() == 1)
		return rpl_CHANNELMODEIS(client, *channel);
	channel->changeMode(client, args);
}

void Server::cmd_PRIVMSG(Client &client, Args args)
{
	if (args.size() == 0)
		return err_NORECIPIENT(client);
	if (args.size() == 1)
	{
		if (args[0][0] == ':')
			return err_NORECIPIENT(client);
		else
			return err_NOTEXTTOSEND(client);
	}
	if (args[1].size() == 1)
		return err_NOTEXTTOSEND(client);

	size_t targetIdx = 0;
	while (targetIdx != std::string::npos)
	{
		Channel *tarChannel = nullptr;
		Client *tarClient = nullptr;

		std::string receiver = nextChar(args[0], targetIdx, ',');
		if (parseChannel(receiver) && getChannel(client, receiver, &tarChannel))
			tarChannel->broadcastMessage(client, "PRIVMSG",args[1], false);
		else if (getClient(client, receiver, &tarClient))
			clientToClient(*tarClient, client, "PRIVMSG " + client.get(Name_Nick) + " " + args[1]);
	}
}


void Server::cmd_WHO(Client &client, Args args)
{
	if (args.size() == 0)
	{
		ClientMap::iterator it = _clients.begin();
		while (it != _clients.end())
		{
			rpl_WHOREPLY(client, nullptr, *it->second);
			it++;
		}
		return rpl_ENDOFWHO(client, "*");
	}
	Channel *tarChannel = getChannel(args[0]);
	if (tarChannel != nullptr)
		return tarChannel->who(client);
	else
	{
		Client *tarClient = getClient(args[0]);
		if (tarClient != nullptr)
			rpl_WHOREPLY(client, nullptr, *tarClient);
	}
	rpl_ENDOFWHO(client, args[0]);
}