
#ifndef CLIENT_HPP
# define CLIENT_HPP
#include <string>
#include "Channel.hpp"
#include "placeholder.hpp"

class Client
{
	public:
		Client();
		bool operator==(const Client &other) const;
		int getSocket();
		void setNames(std::string user, std::string host, std::string server, std::string real);
		std::string getUserName();

	private:
		std::string _userName;
		std::string _hostName;
		std::string _serverName;
		std::string _realName;
		std::string _password;
		int _socket;
		bool _isOperator;
};
#endif