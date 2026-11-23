#include "Server.hpp"

bool Server::_shutDownSignal = false;

Server::Server(int port, const std::string& password) :
	_port(port), 
	_serverFd(-1), 
	_password(password),
	_pollFds(),
	_channels(),
	_date(),
	_quitingClient(),
	_removeChannel(),
	_clients(),
	_cmd()
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
	_cmd["PART"] = &Server::cmd_PART;
	_cmd["PRIVMSG"] = &Server::cmd_PRIVMSG;
	_cmd["WHO"] = &Server::cmd_WHO;
	

	time_t t = time(NULL);
	_date = ctime(&t);
	_date.erase(_date.size() - 1);
	std::cout << "date ("<< _date << ")"<< "\n";
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


bool Server::allowed_command(Client &client, std::string cmd)
{
	if (client.isRegistred())
		return true;
	std::string options[] = { "PASS", "USER", "NICK", "QUIT", "" };
	for (int i = 0; options[i] != ""; i++)
	{
		if (cmd == options[i])
			return true;
	}
	return false;
}


void Server::call_cmd(Client &client, std::string input)
{
	if (input.size() == 0)
		return ;
	size_t spaceIdx = 0;
	std::string command = "";
	std::vector<std::string> params = std::vector<std::string>();
	int i = 0;
	bool prefix = false;
	while (spaceIdx != std::string::npos)
	{
		i++;
		std::string token = nextChar(input, spaceIdx, ' ', prefix);
		if (i == 1 && token[0] == '@')
			continue;

		if (token[0] == ':' && spaceIdx != std::string::npos)
		{
			if (!prefix)
			{
				prefix = true;
				continue;
			}
		}
			
		if (command.empty())
			command = token;
		else if (!token.empty())
			params.push_back(token);
	}
	command = strToUpper(command);
	CmdMap::iterator it = _cmd.find(command);
	if (it == _cmd.end())
		return err_UNKNOWNCOMMAND(client, command);
	if (!params.empty())
	{
		if (params[params.size() - 1][0] == ':')
			params[params.size() - 1].erase(0, 1);
	}
	std::cout << GREEN << "	Command:" << WHITE << std::endl;
	std::cout << "		" << command << std::endl;
	std::cout << MAGENTA << "	Params: " << WHITE << std::endl;
	for (size_t i = 0; i < params.size(); i++)
		std::cout << "		" << params[i] << std::endl;
	std::cout << std::endl;
	if (allowed_command(client, command))
		(this->*(it->second))(client, params);
	else
		err_NOTREGISTERED(client);
}

void Server::set_shutDown(int signal)
{
	(void)signal;
	_shutDownSignal = true;
}
