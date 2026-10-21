#include "Channel.hpp"

Channel::Channel(Client &client, std::string &name) :
	_name(name),
	_topicVal(),
	_passwordVal(),
	_clients(),
	_inviteOnly(false),
	_clientLimit(-1)
{
	_clients.insert(std::make_pair(&client, Status_Op));
}

ClientChannelStatus Channel::getClientStatus(const Client &client)
{
	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (&client == it->first)
			return it->second;
		it++;
	}
	return Status_None;
}



//maybbe just return an int that will be read by the map reply
void Channel::joinClient(Client &client, std::string password)
{
	_clientPassword = password;
	Check checks[] {check_isOnChannel, check_canInvite, check_fullChannel, check_key, NULL};
	if (checkMultiple(client, checks))
		return ;
	_clients.insert(std::make_pair(&client, Status_Connected));
}

void Channel::partClient(Client &client)
{
	if (!check_isOnChannel(client))
		return ;
	_clients.erase(&client);
}

void Channel::kickClient(Client &op, Client &user, std::string reason)
{
	if (!check_isOperator(op) || !check_isOnChannel(user))
		return;
	_clients.erase(&user);
}

void Channel::inviteClient(Client &op, Client &user)
{
	Check checks[] {check_canInvite, check_fullChannel, NULL};
	if (checkMultiple(op, checks) || check_isOnChannel(user))
		return ;
	rpl_INVITING(op, user.get(Name_Nick), _name);
}


void Channel::getTopic(Client &client)
{
	if (!check_isOperator(client))
		return ;
	if (_topicVal == "")
		rpl_NOTOPIC(client, _name);
	else
		rpl_TOPIC(client, _name, _topicVal);
}

void Channel::changeTopic(Client &client, std::string topic)
{
	if (!check_isOperator(client))
		return ;
	_topicVal = topic;
}

bool Channel::consumeArg(Client &client, Args &args, size_t &cur, std::string &out)
{
	if (cur == args.size())
		return (err_NEEDMOREPARAMS(client, "MODE"), false); 
	out = args[cur];
	cur++;
	return true;
}

void Channel::toggleOp(Client &client, std::string name, bool on)
{
	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->first->get(Name_Nick) == name)
		{
			it->second = on ? Status_Op: Status_Connected;
			return ;
		}
		it++;
	}
	return err_NOTONCHANNEL(client, _name);
}


void Channel::changeMode(Client &client, Args args)
{
	if (!check_isOperator(client))
		return;
	bool adding = true;
	size_t curParam = 2;
	for (size_t i = 0; args[1][i] != '\0'; i++)
	{
		if (args[1][i] == '+' || args[1][i] == '-')
		{
			adding = args[1][i] == '+';
			continue ;
		}
		std::string var;
		switch (args[1][i])
		{
			case 'i':
				_inviteOnly = adding;
				break;
			case 't':
				if (adding)
					consumeArg(client, args, curParam, _topicVal);
				else
					_topicVal = "";
				break;
			case 'k':
				if (adding)
					consumeArg(client, args, curParam,_passwordVal);
				else
					_passwordVal = "";
				break;
			case 'o':
				if (consumeArg(client, args, curParam, var))
					toggleOp(client, var, adding);
				break;
			case 'l':
				if (adding && consumeArg(client, args, curParam, var))
					_clientLimit = atoi(var.c_str());
				else
					_clientLimit = -1;
				break;
			default:
				err_UNKNOWNMODE(client, args[1][i]);
				break;

		}
	}
}


void Channel::broadcastMessage(Client &client, std::string message)
{
	if (getClientStatus(client) == Status_None)
		return err_CANNOTSENDTOCHAN(client, _name);
	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (client != *(it->first))
			it->first->sendReply(message);
		it++;
	}
}