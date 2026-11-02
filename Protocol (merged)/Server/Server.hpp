#ifndef SERVER_HPP
# define SERVER_HPP

#include "../Classes.hpp"
#include "../Libs.hpp"

class Server
{
	public:
        Server(int port, const std::string& password);
		~Server();
		void call_cmd(Client &client, std::string input);

		void start();
        void handleMessage(int fd);
        std::string readMessage(int fd); 
        void setupSocket();
        void run();
        void acceptClients();
        void receiveData(int index);
        void removeData(int index);
        void removeChannel(Channel &channel);
		void removeClientChannels(Client &client);


		void serverToClient(Client& target, const std::string& message);
		void clientToClient(Client& target, Client& sender, const std::string& message);

        void sendData(int index);

		void err_USERNOTINCHANNEL(Client &client, std::string user, std::string channel);
		void err_PASSWDMISMATCH(Client &client);
		void err_UNKNOWNCOMMAND(Client &client, std::string cmd);
		void err_ALREADYREGISTRED(Client &client);
		void err_NEEDMOREPARAMS(Client &client, std::string cmd);
		void err_NICKNAMEINUSE(Client &client, std::string nick);
		void err_NONICKNAMEGIVEN(Client &client);
		void err_NOSUCHCHANNEL(Client &client, std::string channel);
		void err_NOSUCHNICK(Client &client, std::string nick);
		void err_UNKNOWNMODE(Client &client, char c);
		void err_CHANOPRIVSNEEDED(Client &client, std::string channel);
		void err_CHANNELISFULL(Client &client, std::string channel);
		void err_NOTONCHANNEL(Client &client, std::string channel);
		void err_INVITEONLYCHAN(Client &client, std::string channel);
		void err_BADCHANNELKEY(Client &client, std::string channel);
		void err_CANNOTSENDTOCHAN(Client &client, std::string channel);
		void err_NORECIPIENT(Client &client);
		void err_NOTEXTTOSEND(Client &client);
		void err_ERRONEUSNICKNAME(Client &client, std::string channel);
		void err_USERONCHANNEL(Client &client, Client &tar, std::string channel);
		void rpl_WELCOME(Client &client, std::string date);
		void rpl_CHANNELMODEIS(Client &client, Channel &channel);
		void rpl_INVITING(Client &client, std::string invited,std::string channel);
		void rpl_NOTOPIC(Client &client, std::string channel);
		void rpl_TOPIC(Client &client, std::string channel,std::string topic);
		void rpl_WHOREPLY(Client &client, Channel *channel, Client &user);
		void rpl_ENDOFWHO(Client &client, std::string query);

	private:
		int _port;
        int _serverFd;
        std::string _password;
        std::vector<pollfd> _pollFds;
		std::vector<Channel> _channels;
		std::string _date;
		ClientMap _clients;
		CmdMap _cmd;

		std::string getDate() const;
		void cmd_PASS(Client &client, Args args);
		void cmd_NICK(Client &client, Args args);
		void cmd_USER(Client &client, Args args);
		void cmd_PART(Client &client, Args args);
		void cmd_QUIT(Client &client, Args args);
		void cmd_JOIN(Client &client, Args args);
		void cmd_KICK(Client &client, Args args);
		void cmd_INVITE(Client &client, Args args);
		void cmd_TOPIC(Client &client, Args args);
		void cmd_MODE(Client &client, Args args);
		void cmd_PRIVMSG(Client &client, Args args);
		void cmd_WHO(Client &client, Args args);

		
		void sendToClient(Client &client, const std::string& sender, const std::string& message);
	

		
		//move to its own helper file, it contains all the system headers as well
		bool getClient(Client &client, std::string name, Client **out);
		bool getChannel(Client &client, std::string name, Channel **out);
		Channel *getChannel(std::string name);
		Client *getClient(std::string name);

};

//move to its own helper file, it contains all the system headers as well
std::string nextChar(const std::string &input, size_t &idx, char c, bool prefix);
std::string nextChar(const std::string &input, size_t &idx, char c);



#endif






