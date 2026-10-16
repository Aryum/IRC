
#ifndef CLIENT_HPP
# define CLIENT_HPP
#include "../Classes.hpp"
class Client
{
	public:
		Client();
		bool operator==(const Client &other) const;
		int getSocket();
		void setNames(std::string user, std::string host, std::string server, std::string real);
		std::string getNickName() const;
		void sendReply(std::string reply) const;

	private:
		std::string _userName;
		std::string _nickName;
		std::string _hostName;
		std::string _serverName;
		std::string _realName;
		std::string _password;
		int _socket;
};
#endif