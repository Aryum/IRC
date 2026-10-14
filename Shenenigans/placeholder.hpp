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
typedef std::vector<std::string> Args;
typedef std::map<std::string, void (placeholder::*)(Client &client, Args)> CmdMap;
typedef std::map<int, void (placeholder::*)()> ReplyMap;
typedef std::map<int, Client*> clientMap;

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
		CmdMap _cmd;
		ReplyMap _rpl;
		
		void createMap();
		void cmd_PASS(Client &client, Args args);
		void cmd_NICK(Client &client, Args args);
		void cmd_USER(Client &client, Args args);
		void cmd_PART(Client &client, Args args);
		void cmd_LIST(Client &client, Args args);
		void cmd_QUIT(Client &client, Args args);
		void cmd_JOIN(Client &client, Args args);
		void cmd_KICK(Client &client, Args args);
		void cmd_INVITE(Client &client, Args args);
		void cmd_TOPIC(Client &client, Args args);
		void cmd_MODE(Client &client, Args args);


		//move to its own helper file, it contains all the system headers as well
		std::vector<Channel>::iterator getChannel(std::string name);
};

//move to its own helper file, it contains all the system headers as well
std::string nextChar(std::string &input, size_t &idx, char c, bool prefix);

#endif

