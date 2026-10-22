#include "Server.hpp"

Server::Server(int port, const std::string& password) :
	_port(port), 
	_serverFd(-1), 
	_password(password)
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
	_cmd["PRIVMSG"] = &Server::cmd_PRIVMSG;
	time_t t = time(NULL);
	_date = ctime(&t);
}
Server::~Server()
{
    if (_serverFd != -1)
        close(_serverFd);
}

std::string Server::getDate() const
{
	return _date;
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
		return err_UNKNOWNCOMMAND(client, command);
	if (command != "PRIVMSG" && params[params.size() - 1][0] == ':')
		params[params.size() - 1].erase(0);
	(this->*(it->second))(client, params);
}