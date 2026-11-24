#include "Channel.hpp"

Channel::Channel(Server* server, Client &client, std::string &name) :
	_server(server),
	_name(name),
	_topicVal(),
	_passwordVal(),
	_mode(),
	_clients(),
	_inviteOnly(false),
	_topicLocked(false),
	_clientCount(1),
	_opCount(1),
	_clientLimit(-1)
{
	std::cout << MAGENTA << "Creating channel " << WHITE << _name  << std::endl;
	_clients.insert(std::make_pair(&client, Status_Op));
	_server->clientToClient(client, client, "JOIN " + _name);
	sendJoinMessage(client);
}

bool Channel::operator==(const Channel &other) const
{
	return strToLower(_name) == strToLower(other._name);
}

std::string Channel::getName()
{
	return _name;
}

int Channel::getClientCount()
{
	return _clientCount;
}
ClientChannelStatus Channel::getClientStatus(Client &client)
{
	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	std::string tarName = strToLower(client.get(Name_Nick));
	while (it != _clients.end())
	{
		if (strToLower(it->first->get(Name_Nick)) == tarName)
			return it->second;
		it++;
	}
	return Status_None;
}
void Channel::sendJoinMessage(Client &client)
{
	getTopic(client);

	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	std::string clientNames = "";
	while (it != _clients.end())
	{
		if (it->second == Status_Invited)
			continue ;
		if(it != _clients.begin())
			clientNames += " ";
		if (it->second == Status_Op)
			clientNames += "@";
		clientNames += it->first->get(Name_Nick);
		it++;
	}
	_server->rpl_NAMREPLY(client, _name, clientNames);
}

void Channel::joinClient(Client &client, std::string password)
{
	if (!test_clientCanEnter(client, password))
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

	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->second != Status_Invited)
			_server->clientToClient(*it->first, client, "JOIN " + _name);
		it++;
	}
	sendJoinMessage(client);
}

void Channel::removeClient(Client &client)
{
	ClientChannelStatus status = getClientStatus(client);
	_clients.erase(&client);
	if (status == Status_Invited || status == Status_None)
		return ; 
	_clientCount--;
	if (_clientCount == 0)
		return _server->removeChannel(*this);
	if (status != Status_Op)
		return ;
	_opCount--;
	if (_opCount != 0)
		return ;
	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	std::map<Client*, ClientChannelStatus>::iterator nextOp = _clients.end();
	while (it != _clients.end())
	{
		if (it->second == Status_Op)
			return ;
		if (nextOp == _clients.end() && it->second == Status_Connected)
			nextOp = it;
		it++;
	}
	broadcastMessage(*nextOp->first, "MODE", "+o " + nextOp->first->get(Name_Nick), true);
	nextOp->second = Status_Op;
	_opCount++;
}

void Channel::quitClient(Client &client)
{
	if (getClientStatus(client) == Status_Invited)
	{
		_clients.erase(&client);
		return ;
	}
	removeClient(client);
}

void Channel::partClient(Client &client, std::string reason)
{
	leaveClient(client, "PART", reason);
}

void Channel::leaveClient(Client &client, std::string source, std::string reason)
{
	if (!test_senderOnChannel(client))
		return ;
	if(reason[0] != ':')
		reason.insert(0, ":");
	broadcastMessage(client, source, reason, true);
	removeClient(client);
}
void Channel::kickClient(Client &op, Client &user, std::string reason)
{
	if (!test_isOperator(op) || !test_targetOnChannel(op, user))
		return;
	broadcastMessage(op, "KICK", user.get(Name_Nick) + " :" + reason, true);
	removeClient(user);
}

void Channel::inviteClient(Client &op, Client &user)
{
	if (!test_canInvite(op) || !test_targetNotOnChannel(op, user))
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
	if (!test_canChangeTopic(client))
		return ;
	_topicVal = topic;
	broadcastMessage(client, "TOPIC", ":" + topic, true);
}

bool Channel::consumeArg(Client &client, Args args, size_t &cur, std::string &out)
{
	if (cur == args.size())
		return (_server->err_NEEDMOREPARAMS(client, "MODE"), false); 
	out = args[cur];
	cur++;
	return true;
}

bool Channel::toggleOp(Client &client, std::string tarName, bool on)
{
	if (!on && _opCount == 1)
		return false;
	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	std::string cmp = strToLower(tarName);
	while (it != _clients.end())
	{
		if (strToLower(it->first->get(Name_Nick)) == cmp)
		{
			if (it->second == Status_Invited)
				break ;
			it->second = on ? Status_Op : Status_Connected;
			_opCount += 1 * (on ? 1 : -1);
			return true;
		}
		it++;
	}
	return (_server->err_USERNOTINCHANNEL(client, tarName, _name), false);
}

void Channel::setModeRpl(char c, bool adding, std::string val, std::string &options, std::string &values)
{
	size_t sign = options.find_last_of("+-");
	if (sign == std::string::npos)
		options += adding ? '+' : '-';
	else
	{
		char curMode = options[sign];
		if (curMode == '-' && adding)
			options += '+';
		if (curMode == '+' && !adding)
			options += '-';
	}
	options += c;

	if ((adding || c == 'o') && val != "")
		values += (values.size() != 0 ? " " : "") + val;
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

static bool extractNumber(std::string &str, int &result)
{
	if (str.size() > 10)
		return false;
	char *end;
	long value = std::strtol(str.c_str(), &end, 10);
	if (value > INT_MAX || value <= 0 || end == str.c_str() || *end != '\0')
		return false;
	result = static_cast<int>(value);
	return true;
}
void Channel::changeMode(Client &client, Args args)
{
	if (!test_isOperator(client))
		return;
	bool adding = true;
	int limit = 0;
	size_t curParam = 2;
	std::string options = std::string();
	std::string values = std::string();
	for (size_t i = 0; args[1][i] != '\0'; i++)
	{
		if (args[1][i] == '+' || args[1][i] == '-')
		{
			adding = args[1][i] == '+';
			continue ;
		}
		std::string var = std::string();
		switch (args[1][i])
		{
			case 'i':
				_inviteOnly = adding;
				updateMode(args[1][i], adding);
				setModeRpl(args[1][i], adding, "", options, values);
				break;
			case 't':
				_topicLocked = adding;
				updateMode(args[1][i], adding);
				setModeRpl(args[1][i], adding, "", options, values);
				break;
			case 'k':
				if (adding)
				{
					if (!consumeArg(client, args, curParam, var))
						break ;
					if (var.find(' ', 0) != std::string::npos)
					{
						_server->err_INVALIDKEY(client, _name);
						break ;
					}
					else
						_passwordVal = var;
				} 
				else
					_passwordVal = "";
				updateMode(args[1][i], adding);
				setModeRpl(args[1][i], adding, _passwordVal, options, values);
				break;
			case 'o':
				if (consumeArg(client, args, curParam, var) && toggleOp(client, var, adding))
					setModeRpl(args[1][i], adding, var, options, values);
				break;
			case 'l':
				if (adding)
				{
					if (!consumeArg(client, args, curParam, var) || !extractNumber(var, limit))
						break ;
					_clientLimit = limit;
				}
				else
					_clientLimit = -1;
				var = (_clientLimit != -1 ? var : "");
				updateMode(args[1][i], adding);
				setModeRpl(args[1][i], adding, var, options, values);
				break;
			default:
				_server->err_UNKNOWNMODE(client, args[1][i]);
				break;
		}
	}
	if (!options.empty())
		broadcastMessage(client, "MODE", options + " " + values, true);
}

void Channel::broadcastMessage(Client &client, std::string cmd, std::string message, bool sendSender)
{
	if (!isOnChannel(client))
		return _server->err_CANNOTSENDTOCHAN(client, _name);
	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->second != Status_Invited && (sendSender || &client != it->first) )
			_server->clientToClient(*it->first, client, cmd + " " + _name + " " + message);
		it++;
	}
}

std::string Channel::getModes(Client &client)
{
	std::string ret = _mode;
	std::ostringstream stream;

	for (size_t i = 0; i < _mode.size(); i++)
	{
		switch (_mode[i])
		{
			case 'k':
				ret += " ";
				if (isOnChannel(client))
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
		if (it->second != Status_Invited)
			_server->rpl_WHOREPLY(client, this, *it->first);
		it++;
	}
}