#ifndef SERVER_HPP
# define SERVER_HPP
#include "../Classes.hpp"

#include <poll.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <vector>
#include <string>
#include <map>
class Server;
typedef const std::vector<std::string>& Args;
typedef std::map<std::string, void (Server::*)(Client &, Args)> CmdMap;
typedef std::map<int, Client*> ClientMap;

class Server
{
	public:
		Server();
		void call_cmd(Client &client, std::string input);

	private:
		int _port;
		std::string _password;
		std::vector<pollfd> _sockets; //client + server
		ClientMap _clients;
		std::vector<Channel> _channels;
		CmdMap _cmd;
		
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
		Channel* getChannel(std::string name);
		Client *getClient(std::string name);

};

//move to its own helper file, it contains all the system headers as well
std::string nextChar(const std::string &input, size_t &idx, char c, bool prefix);

#endif

