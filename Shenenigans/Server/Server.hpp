#ifndef SERVER_HPP
# define SERVER_HPP
#include "../Classes.hpp"

#include <poll.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>

class Server;
typedef const std::vector<std::string>& Args;
typedef std::vector<std::string> EmptyArgs;
typedef std::map<std::string, void (Server::*)(Client &, Args)> CmdMap;
typedef std::map<int, void (Server::*)(Client &, Args)> ReplyMap;
typedef std::map<int, Client*> ClientMap;

enum ReplyCode
{
	ERR_ALREADYREGISTRED = 462,
	ERR_NEEDMOREPARAMS = 461,
	ERR_NICKCOLLISION = 436,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_NOSUCHCHANNEL = 403,
	ERR_NOSUCHNICK = 401,
	RPL_NOTOPIC = 331,
	RPL_TOPIC = 332
};

class Server
{
	public:
		Server();
		void call_cmd(Client &client, std::string input);

	private:
		int _port;
		std::string _password;
		std::vector<pollfd> _sockets; //client + server
		std::vector<Channel> _channels;
		ClientMap _clients;
		CmdMap _cmd;
		ReplyMap _replies;

		void create_cmdMap();
		void create_replyMap();

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

		void err_ALREADYREGISTRED(Client &client, Args args);
		void err_NEEDMOREPARAMS(Client &client, Args args);
		void err_NICKCOLLISION(Client &client, Args args);
		void err_NONICKNAMEGIVEN(Client &client, Args args);
		void err_NOSUCHCHANNEL(Client &client, Args args);
		void err_NOSUCHNICK(Client &client, Args args);
		void rpl_NOTOPIC(Client &client, Args args);
		void rpl_TOPIC(Client &client, Args args);
		
		//move to its own helper file, it contains all the system headers as well
		bool getClient(Client &client, std::string name, Client **out);
		bool getChannel(Client &client, std::string name, Channel **out);
		Channel *getChannel(std::string name);
};

//move to its own helper file, it contains all the system headers as well
std::string nextChar(const std::string &input, size_t &idx, char c, bool prefix);

#endif






