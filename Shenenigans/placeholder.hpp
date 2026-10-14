#ifndef PLACEHOLDER_HPP
# define PLACEHOLDER_HPP
#include <poll.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <vector>
#include <string>
#include <map>
#include <string>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <vector>
#include "Client.hpp"
#include "Channel.hpp"


class placeholder;
typedef std::map<std::string, void (placeholder::*)(Client &client, std::vector<std::string>)> cmdMap;
typedef std::map<int, void (placeholder::*)()> rplMap;
typedef std::map<int, Client*> clientMap;
typedef cmdMap::iterator funcIt;

class placeholder
{
	public:
		placeholder();
		void call_cmd(Client &client, std::string input);

	private:
		int _port;
		std::string _password;
		std::vector<pollfd> _sockets; //client + server
		clientMap _clients;
		std::vector<Channel> _channels;
		cmdMap _cmd;
		rplMap _rpl;
		
		void createMap();
		void cmd_PASS(Client &client, std::vector<std::string> args);
		void cmd_NICK(Client &client, std::vector<std::string> args);
		void cmd_USER(Client &client, std::vector<std::string> args);
		void cmd_PART(Client &client, std::vector<std::string> args);
		void cmd_LIST(Client &client, std::vector<std::string> args);
		void cmd_QUIT(Client &client, std::vector<std::string> args);
		void cmd_JOIN(Client &client, std::vector<std::string> args);
		void cmd_KICK(Client &client, std::vector<std::string> args);
		void cmd_INVITE(Client &client, std::vector<std::string> args);
		void cmd_TOPIC(Client &client, std::vector<std::string> args);
		void cmd_MODE(Client &client, std::vector<std::string> args);
		void cmd_NAMES(Client &client, std::vector<std::string> args);
};


#endif

