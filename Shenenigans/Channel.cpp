#include "Channel.hpp"

ClientChannelStatus Channel::getClientStatus(Client &client)
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
std::string Channel::joinClient(Client &client, std::string password)
{
	ClientChannelStatus status = getClientStatus(client);
	if (status != Status_None)
		return; //ERR_USERONCHANNEL
	if (_inviteOnly)
	{
		return ;//ERR_INVITEONLYCHAN
	}
	if (_clients.size() == _clientLimit)
		return ; //ERR_CHANNELISFULL
	else if (_passwordVal != "" && password != _passwordVal)
		return ;//ERR_BADCHANNELKEY
	_clients.insert(std::make_pair(&client, Status_Connected));
}

std::string Channel::partClient(Client &client)
{
	ClientChannelStatus status = getClientStatus(client);
	if (status == Status_None)
		return; //ERR_NOTONCHANNEL
	_clients.erase(&client);
}

std::string Channel::kickClient(Client &op, Client &user, std::string reason)
{
	ClientChannelStatus callerStatus = getClientStatus(op);
	ClientChannelStatus targetStatus = getClientStatus(user);
	//maybe needs to check if op is in the server as well
	if (callerStatus != Status_Op)
		return; //ERR_CHANOPRIVSNEEDED
	if (targetStatus == Status_None)
		return ; //ERR_NOTONCHANNEL
	if (targetStatus == Status_Op)
		return ; //op on op violence?
	_clients.erase(&user);
	//maybe needs to check if user is already banned
}

std::string Channel::inviteClient(Client &op, Client &user)
{
	ClientChannelStatus callerStatus = getClientStatus(op);
	ClientChannelStatus userStatus = getClientStatus(user);
	switch (callerStatus)
	{
		case Status_Op:
			break;
		case Status_Connected:
			if (_inviteOnly)
				return ; //ERR_CHANOPRIVSNEEDED
			break;
		default: //for none status
			return ; //ERR_NOTONCHANNEL

	}
	if (userStatus != Status_None)
		return ; //ERR_USERONCHANNEL
	if (_clients.size() == _clientLimit)
		return ; //ERR_CHANNELISFULL
	return ; //RPL_INVITING
	
	//RPL_AWAY is sent check when itssent prob doesnt matter
}

std::string Channel::getTopic(Client &client)
{
	ClientChannelStatus callerStatus = getClientStatus(client);
	switch (callerStatus)
	{
		case Status_Op:
			break;
		case Status_Connected: //probably wrong
			return; //ERR_CHANOPRIVSNEEDED
		default:
			return;//ERR_NOTONCHANNEL
	}
	if (_topicVal == "")
		return ; //RPL_NOTOPIC
	return; //RPL_TOPIC_ topicSet

}

std::string Channel::changeTopic(Client &client, std::string topic)
{
	ClientChannelStatus callerStatus = getClientStatus(client);
	switch (callerStatus)
	{
		case Status_None:
			return;//ERR_NOTONCHANNEL
		case Status_Connected:
			return; //ERR_CHANOPRIVSNEEDED
		default:
			break;
	}
	_topicVal = topic;
	//prob neefds smt else
}
bool Channel::consumeArg(Args &args, size_t &cur, std::string &out)
{
	if (cur == args.size())
	{
		return false; //ERR_NEEDMOREPARAMS
	}
	out = args[cur];
	cur++;
	return true;
}

void Channel::toggleOp(std::string name, bool on)
{
	std::map<Client*, ClientChannelStatus>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->first->getNickName() == name)
		{
			it->second = on ? Status_Op: Status_Connected;
			return ;
		}
		it++; //ERR_NOTONCHANNEL
	}
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
std::string Channel::changeMode(Client &client, Args args)
{
	switch(getClientStatus(client))
	{
		case Status_None:
			return ; //ERR_NOTONCHANNEL
		case Status_Connected:
			return ; //ERR_CHANOPRIVSNEEDED
		default:
			break;
	}
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
					consumeArg(args, curParam, _topicVal);
				else
					_topicVal = "";
				break;
			case 'k':
				if (adding)
					consumeArg(args, curParam,_passwordVal);
				else
					_passwordVal = "";
				break;
			case 'o':
				//this allows for op on op shenenignas
				if (consumeArg(args, curParam, var))
					toggleOp(var, adding);
				break;
			case 'l':
				if (adding && consumeArg(args, curParam, var))
					_clientLimit = atoi(var.c_str());
				else
					_clientLimit = -1;
				break;
			default:
				//ERR_UNKNOWNMODE
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


