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

static bool parseNick(std::string name)
{
	if (!std::isalpha(name[0]))
		return false;
	std::string allowed = "-[]\\`^{}";
	for (size_t i = 1; i < name.size(); i++)
	{
		if (std::isalpha(name[i]))
			continue;
		if (std::isdigit(name[i]))
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
	if (parseNick(args[0]))
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
		std::string password = nextChar(args[0], channelIdx, ',');

		if (!parseChannel(name))
			err_NOSUCHCHANNEL(client, name);
		else
		{
			Channel *channel = getChannel(name);
			std::string reply;
			if (channel == nullptr)
				_channels.push_back(Channel(client, name));
			else
				channel->joinClient(client, password);
		}
		
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
		std::string name = nextChar(args[0], channelIdx, ',');
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
		std::string channelName = nextChar(args[0], channelIdx, ',');
		std::string nickName = nextChar(args[1], userIdx, ',');
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
			tarChannel->broadcastMessage(client, args[1]);
		else if (getClient(client, receiver, &tarClient))
			client.sendReply(args[1]);
	}


	/*

      Command: PRIVMSG
   Parameters: <receiver>{,<receiver>} <text to be sent>

   PRIVMSG is used to send private messages between users.  <receiver>
   is the nickname of the receiver of the message.  <receiver> can also
   be a list of names or channels separated with commas.

   The <receiver> parameter may also me a host mask  (#mask)  or  server
   mask  ($mask).   In  both cases the server will only send the PRIVMSG
   to those who have a server or host matching the mask.  The mask  must
   have at  least  1  (one)  "."  in it and no wildcards following the
   last ".".  This requirement exists to prevent people sending messages
   to  "#*"  or "$*",  which  would  broadcast  to  all  users; from
   experience, this is abused more than used responsibly and properly.
   Wildcards are  the  '*' and  '?'   characters.   This  extension  to
   the PRIVMSG command is only available to Operators.

   Numeric Replies:

           ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
           ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
           ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
           ERR_NOSUCHNICK
           RPL_AWAY
	
	*/
}