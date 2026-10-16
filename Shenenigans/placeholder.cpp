#include "placeholder.hpp"

placeholder::placeholder()
{
	createMap();
}

void placeholder::createMap()
{
	_cmd["PASS"] = &placeholder::cmd_PASS;
	_cmd["NICK"] = &placeholder::cmd_NICK;
	_cmd["USER"] = &placeholder::cmd_USER;
	_cmd["QUIT"] = &placeholder::cmd_QUIT;
	_cmd["JOIN"] = &placeholder::cmd_JOIN;
	_cmd["KICK"] = &placeholder::cmd_KICK;
	_cmd["INVITE"] = &placeholder::cmd_INVITE;
	_cmd["TOPIC"] = &placeholder::cmd_TOPIC;
	_cmd["MODE"] = &placeholder::cmd_MODE;
}


void placeholder::call_cmd(Client &client, std::string input)
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
		throw Reply(command + "  not found"); 
	(this->*(it->second))(client, params);
}