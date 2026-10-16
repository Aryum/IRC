#include "Channel.hpp"

Channel::Channel(const Client &client,const std::string &name) :
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
	std::map<const Client*, ClientChannelStatus>::iterator it = _clients.begin();
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
	Check checks[] {isOnChannel, canInvite, fullChannel, checkKey, NULL};
	if (hasError(client, checks))
		return ;
	_clients.insert(std::make_pair(&client, Status_Connected));
}

void Channel::partClient(Client &client)
{
	if (!isOnChannel(client))
		return ;
	_clients.erase(&client);
}

void Channel::kickClient(Client &op, Client &user, std::string reason)
{
	if (!isOperator(op) || !isOnChannel(user))
		return;
	_clients.erase(&user);
}

void Channel::inviteClient(Client &op, Client &user)
{
	Check checks[] {canInvite, fullChannel, NULL};
	if (hasError(op, checks) || isOnChannel(user))
		return ;
	op.sendReply("RPL_INVITING");
}


void Channel::getTopic(Client &client)
{
	//probably wrong
	if (!isOperator(client))
		return ;
	if (_topicVal == "")
		client.sendReply("RPL_NOTOPIC");
	else
		client.sendReply("RPL_TOPIC");
}

void Channel::changeTopic(Client &client, std::string topic)
{
	if (!isOperator(client))
		return ;
	_topicVal = topic;
	//prob neefds smt else
}

bool Channel::consumeArg(Client &client, Args &args, size_t &cur, std::string &out)
{
	if (cur == args.size())
	{
		client.sendReply("ERR_NEEDMOREPARAMS");
		return false; 
	}
	out = args[cur];
	cur++;
	return true;
}

void Channel::toggleOp(Client &client, std::string name, bool on)
{
	
	std::map<const Client*, ClientChannelStatus>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->first->getNickName() == name)
		{
			it->second = on ? Status_Op: Status_Connected;
			return ;
		}
		it++;
	}
	client.sendReply("ERR_NOTONCHANNEL");
	return ;
}

/*
ERR_NEEDMOREPARAMS              RPL_CHANNELMODEIS
           ERR_CHANOPRIVSNEEDED            ERR_NOSUCHNICK
           ERR_NOTONCHANNEL                ERR_KEYSET
           RPL_BANLIST                     RPL_ENDOFBANLIST
           ERR_UNKNOWNMODE                 

           ERR_USERSDONTMATCH              RPL_UMODEIS
           ERR_UMODEUNKNOWNFLAG*/
void Channel::changeMode(Client &client, Args args)
{
	if (!isOperator(client))
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
				//this allows for op on op shenenignas
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
				client.sendReply("ERR_UNKNOWNMODE");
				break;

		}
	}


	//args are checked here, it passes MODE as well, remember to ignore 0
	/*
			i - invite-only channel flag;
           t - topic settable by channel operator only flag;
		   k - set a channel key (password).
           o - give/take channel operator privileges;
        	   l - set the user limit to channel;
   Yes. A single MODE command can carry several mode changes in one modestring, and the parameters are consumed left to right by whichever modes need them.

MODE #chan +itk secret          ; +i, +t, and key "secret"
MODE #chan +kl secret 10        ; key, then limit (params in order)
MODE #chan +i-t+o alice         ; mixed signs: +i, -t, +o alice
MODE #chan -k+l 20              ; -k takes no param here, +l takes 20


Parsing rules

Start with a “current sign” (+ by default). Each + or - in the modestring switches it, and it applies to all following letters until the next sign.
Walk the letters one at a time. For each letter, decide whether it needs a parameter, and if so take the next unused argument from the list.
Which modes need a parameter depends on the sign:
Mode	+	-
i, t	none	none
k	key	none (some servers accept and ignore a key)
l	limit	none
o	nick	nick
*/
}


