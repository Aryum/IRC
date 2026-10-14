#include <poll.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <vector>
#include <string>
#include <map>
#include "Client.hpp"
#include "Channel.hpp"

class Server
{
	private:
		int _port;
		std::string _password;
		std::vector<pollfd> _sockets; //client + server
		std::map<int, Client*> _clients;
		std::vector<Channel> _channels;
};