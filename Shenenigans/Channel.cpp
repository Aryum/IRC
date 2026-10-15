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
	switch (status)
	{
		case Status_None:
			break;
		default:
			return; //ERR_USERONCHANNEL
	}
	if (_inviteOnly)
	{
		//ERR_INVITEONLYCHAN
	}
	else if (!_passwordSet || password == _passwordVal)
		_clients.insert(std::make_pair(&client, Status_Connected));
	else
	{
		//ERR_BADCHANNELKEY
	}
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
	switch (userStatus)
	{
		case Status_None:
			return ; //RPL_INVITING
		default:
			return ; //ERR_USERONCHANNEL
	}
	//RPL_AWAY is sent check when
}

std::string Channel::getTopic(Client &client)
{
	ClientChannelStatus callerStatus = getClientStatus(client);
	switch (callerStatus)
	{
		case Status_Op:
			break;
		case Status_Connected:
			return; //ERR_CHANOPRIVSNEEDED
		default:
			return;//ERR_NOTONCHANNEL
	}
	if (_topicSet)
		return ; //RPL_TOPIC
	return;  //RPL_NOTOPIC

}

std::string Channel::changeTopic(Client &client, std::string topic)
{
	ClientChannelStatus callerStatus = getClientStatus(client);
	switch (callerStatus)
	{
		case Status_Op:
			break;
		case Status_Connected:
			return; //ERR_CHANOPRIVSNEEDED
		default:
			return;//ERR_NOTONCHANNEL
	}
	_topicSet = true;
	_topicVal = topic;
	//prob neefds smt else

}
std::string Channel::changeMode(Client &client, Args args)
{
	//args are checked here, it passes MODE as well, remember to ignore 0
	/*
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


