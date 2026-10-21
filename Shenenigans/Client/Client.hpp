
#ifndef CLIENT_HPP
# define CLIENT_HPP
#include "../Classes.hpp"

enum NameType
{
	Name_User,
	Name_Nick, 
	Name_Host
};

class Client
{
	public:
		Client(int socket);
		bool operator==(Client &other) const;
		bool operator!=(Client &other) const;
		int getSocket();
		void changeNick(std::string nick);
		void setNames(std::string user, std::string host, std::string nick);
		void sendReply(std::string reply) const;
		void setPasswordVerified();
		bool isRegistred();
		std::string get(NameType type) const;
		
	private:
		std::string _userName;
		std::string _nickName;
		std::string _hostName;
		std::string _realName;
		int _socket;
		bool _correctPassword;
		Channel *_curChannel;
};

#endif