#include "Channel.hpp"

Channel::Channel(Server* server, Client &client, std::string &name) :
	_server(server),
	_name(name),
	_topicVal(),
	_passwordVal(),
	_clientPassword(),
	_mode(),
	_clients(),
	_inviteOnly(false),
	_topicLocked(false),
	_clientCount(1),
	_clientLimit(-1)
{
	std::cout << MAGENTA << "Creating channel " << _name << WHITE << std::endl;
	_clients.insert(std::make_pair(&client, Status_Op));
	getTopic(client);
	_server->clientToClient(client, client, "JOIN " + _name);
}

bool Channel::operator==(const Channel &other) const
{
	return _name == other._name;
}

ClientChannelStatus Channel::getClientStatus( Client &client)
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

void Channel::joinClient(Client &client, std::string password)
{
	_clientPassword = password;
	Check checks[] = {	&Channel::clientCanEnter, 
						&Channel::check_fullChannel, 
						&Channel::check_key, NULL};
	if (!checkMultiple(client, checks))
		return ;
	switch (getClientStatus(client))
	{
		case Status_Invited:
			_clients[&client] = Status_Connected;
			break;
		case Status_None:
			_clients.insert(std::make_pair(&client, Status_Connected));
			break;
		default:
			return;
	}
	_clientCount++;
	getTopic(client);

	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		_server->clientToClient(*it->first, client, "JOIN " + _name);
		it++;
	}
}

void Channel::removeClient(Client &client)
{
	_clientCount--;
	_clients.erase(&client);
	if (_clientCount == 0)
	{
		std::cout << MAGENTA << "Removing channel " << _name << WHITE << std::endl;
		_server->removeChannel(*this);
	}
}

void Channel::quitClient(Client &client, std::string reason)
{
	leaveClient(client, "QUIT", reason);
}

void Channel::partClient(Client &client, std::string reason)
{
	leaveClient(client, "PART", reason);
}


void Channel::leaveClient(Client &client, std::string source, std::string reason)
{
	if (!check_isOnChannel(client))
		return ;
	if(reason[0] != ':')
		reason.insert(0, ":");
	broadcastMessage(client, source, reason, true);
	removeClient(client);
}
void Channel::kickClient(Client &op, Client &user, std::string reason)
{
	if (!check_isOperator(op) && !check_isOnChannel(user))
		return;
	broadcastMessage(op, "KICK", _name + user.get(Name_Nick) + ":" + reason, true);
	_clients.erase(&user);
}

void Channel::inviteClient(Client &op, Client &user)
{
	Check checks[] = {	&Channel::check_canInvite, 
						&Channel::check_fullChannel, NULL};
	if (!checkMultiple(op, checks) || !notOnChannel(op, user))
		return ;
	_clients.insert(std::make_pair(&user, Status_Invited));
	_server->rpl_INVITING(op, user.get(Name_Nick), _name);
	_server->clientToClient(user, op, "INVITE " + user.get(Name_Nick) + " :" + _name);
}

void Channel::getTopic(Client &client)
{
	if (_topicVal == "")
		_server->rpl_NOTOPIC(client, _name);
	else
		_server->rpl_TOPIC(client, _name, _topicVal);
}

void Channel::changeTopic(Client &client, std::string topic)
{
	if (_topicLocked && !check_isOperator(client))
		return ;
	_topicVal = topic;
}

bool Channel::consumeArg(Client &client, Args args, size_t &cur, std::string &out)
{
	if (cur == args.size())
		return (_server->err_NEEDMOREPARAMS(client, "MODE"), false); 
	out = args[cur];
	cur++;
	return true;
}

void Channel::toggleOp(Client &client, std::string tarName, bool on)
{
	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->first->get(Name_Nick) == tarName)
		{
			it->second = on ? Status_Op : Status_Connected;
			return ;
		}
		it++;
	}
	_server->err_USERNOTINCHANNEL(client, tarName, _name);
}

void Channel::updateMode(char c, bool adding)
{
	size_t pos = _mode.find(c);
	if (adding)
	{
		if (pos == std::string::npos)
			_mode += c;
	}
	else if (pos != std::string::npos)
		_mode.erase(pos, 1);
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
				updateMode(args[1][i], adding);
				break;
			case 't':
				_topicLocked = adding;
				updateMode(args[1][i], adding);
				break;
			case 'k':
				if (adding)
				{
					if (!consumeArg(client, args, curParam, var))
						break ;
					if (var.find(' ', 0) != std::string::npos)
						_server->err_INVALIDKEY(client, _name);
					else
						_passwordVal = var;
				} 
				else
					_passwordVal = "";
				updateMode(args[1][i], adding);
				break;
			case 'o':
				if (consumeArg(client, args, curParam, var))
					toggleOp(client, var, adding);
				break;
			case 'l':
				if (adding)
				{
					if (!consumeArg(client, args, curParam, var))
						break ;
					_clientLimit = atoi(var.c_str());
				}
				else
					_clientLimit = -1;
				updateMode(args[1][i], adding);
				break;
			default:
				_server->err_UNKNOWNMODE(client, args[1][i]);
				break;

		}
	}
}

void Channel::broadcastMessage(Client &client, std::string cmd, std::string message, bool sendSender)
{
	if (getClientStatus(client) == Status_None)
		return _server->err_CANNOTSENDTOCHAN(client, _name);
	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (sendSender || client != *(it->first))
			_server->clientToClient(*it->first, client, cmd + " " + _name + " " + message);
		it++;
	}
}

std::string Channel::getModes(Client &client)
{
	std::string ret = _mode;
	std::ostringstream stream;

	ClientChannelStatus status = getClientStatus(client);
	for (size_t i = 0; i < _mode.size(); i++)
	{
		switch (_mode[i])
		{
			case 'k':
				ret += " ";
				if (status == Status_Connected || status == Status_Op)
					ret += _passwordVal;
				else
					ret += "*";
				break;
			case 'l':
				ret += " ";
				stream << _clientLimit;
				ret += stream.str();
				break;
			default:
				break;

		}
	}
	return ret;
}

void Channel::who(Client &client)
{
	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		_server->rpl_WHOREPLY(client, this, *it->first);
		it++;
	}
}