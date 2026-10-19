#include "Server.hpp"

Server::Server()
{
	create_cmdMap();
}

void Server::create_cmdMap()
{
	_cmd["PASS"] = &Server::cmd_PASS;
	_cmd["NICK"] = &Server::cmd_NICK;
	_cmd["USER"] = &Server::cmd_USER;
	_cmd["QUIT"] = &Server::cmd_QUIT;
	_cmd["JOIN"] = &Server::cmd_JOIN;
	_cmd["KICK"] = &Server::cmd_KICK;
	_cmd["INVITE"] = &Server::cmd_INVITE;
	_cmd["TOPIC"] = &Server::cmd_TOPIC;
	_cmd["MODE"] = &Server::cmd_MODE;
}

void Server::create_replyMap()
{
	_replies[ERR_ALREADYREGISTRED] = &Server::err_ALREADYREGISTRED;
	_replies[ERR_NEEDMOREPARAMS] = &Server::err_NEEDMOREPARAMS;
	_replies[ERR_NICKCOLLISION] = &Server::err_NICKCOLLISION;
	_replies[ERR_NONICKNAMEGIVEN] = &Server::err_NONICKNAMEGIVEN;
	_replies[ERR_NOSUCHCHANNEL] = &Server::err_NOSUCHCHANNEL;
	_replies[ERR_NOSUCHNICK] = &Server::err_NOSUCHNICK;
	_replies[RPL_NOTOPIC] = &Server::rpl_NOTOPIC;
	_replies[RPL_TOPIC] = &Server::rpl_TOPIC;
}


void Server::call_cmd(Client &client, std::string input)
{
	size_t spaceIdx = 0;
	std::string command = "";
	std::vector<std::string> params;
	int i = 0;
	bool prefix = false;
	while (spaceIdx != std::string::npos)
	{
		i++;
		std::string token = nextChar(input, spaceIdx, ' ', prefix);
		if (i == 1 && token[0] == '@')
		{
			std::cout << "ignoring tag\n";
			continue;
		}

		if (token[0] ==  ':')
		{
			if (!prefix)
			{
				std::cout << "ignoring source\n";
				prefix = true;
				continue;
			}
			else
				token.erase(0);
		}
			
		if (command.empty())
			command = token;
		else if (!token.empty())
			params.push_back(token);
	}
	std::cout << "Command " << command << "\n";
	std::cout << "Param " << "\n";
	for (size_t h = 0; h < params.size(); h++)
		std::cout << "	" << params[h] << "\n";
	return;
	CmdMap::iterator it = _cmd.find(command);
	if (it == _cmd.end())
		client.sendReply(command + "  not found"); // aint like this obv
	(this->*(it->second))(client, params);
}