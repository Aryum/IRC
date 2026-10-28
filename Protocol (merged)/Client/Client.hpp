
#ifndef CLIENT_HPP
# define CLIENT_HPP
#include "../Classes.hpp"
#include "../Libs.hpp"

enum NameType
{
	Name_User,
	Name_Nick, 
	Name_Host,
	Name_Full,
	Name_Real
};

class Client 
{
    public:
		Client(int socket);
        ~Client();

		bool operator==(const Client &other) const;
		bool operator!=(const Client &other) const;

		int getSocket();
		void changeNick(std::string nick);
		void setNames(std::string user, std::string host, std::string nick);
		void setPasswordVerified();
		void setWelcome();
		bool isRegistred();
		bool isWelcome();

		std::string get(NameType type) const;


        void appendInput(const std::string& data);
        bool hasCompleteLine() const;
        std::string extractLine();

        
        void appendOutput(const std::string& data);
        bool hasPendingOutput() const;
        const std::string& getOutputBuffer() const;
        void removeSentOutput(std::string::size_type bytes);



    private:
		std::string _userName;
		std::string _nickName;
		std::string _hostName;
		std::string _fullName;
		std::string _realName;
		int _socket;
		bool _correctPassword;
		bool _namesSet;
		bool _isWelcome;
		Channel *_curChannel;
        std::string inputBuffer;
        std::string outputBuffer;

};


#endif