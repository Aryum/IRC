#ifndef CLIENT_HPP
#define CLIENT_HPP


#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cmath>
#include <string>
#include <exception>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <cerrno>



class Client 
{
    public:
		Client(int socket);
		bool operator==(Client &other) const;
		bool operator!=(Client &other) const;

		int getSocket();
		void cmd_Nick(std::string nick);
		void cmd_USER(std::string user, std::string host, std::string nick);
		void sendReply(std::string reply) const;
		void setPasswordVerified();
		bool isRegistred();
		std::string get(NameType type) const;
        ~Client();

        int getFd() const;
        const std::string& getHostname() const;
        int getPort() const;

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
		std::string _realName;
		int _socket;
		bool _correctPassword;
		Channel *_curChannel;
        std::string inputBuffer;
        std::string outputBuffer;

		

};


#endif