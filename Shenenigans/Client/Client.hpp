
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
		bool operator==(const Client &other) const;
		int getSocket();
		void setNames(NameType type, std::string value);
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
};

#endif