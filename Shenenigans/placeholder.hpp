#include <map>
#include <string>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <vector>

class placeholder;

typedef std::map<std::string, void (placeholder::*)(std::vector<std::string>)> cmdMap;
typedef std::map<int, void (placeholder::*)()> rplMap;

typedef cmdMap::iterator funcIt;

class placeholder
{
	public:
		placeholder();
		void call_cmd(std::string input);

	private:
		cmdMap _cmd;
		rplMap _rpl;
		
		void createMap();
		void cmd_PASS(std::vector<std::string> args);
		void cmd_NICK(std::vector<std::string> args);
		void cmd_USER(std::vector<std::string> args);
		void cmd_PART(std::vector<std::string> args);
		void cmd_LIST(std::vector<std::string> args);
		void cmd_QUIT(std::vector<std::string> args);
		void cmd_JOIN(std::vector<std::string> args);
		void cmd_KICK(std::vector<std::string> args);
		void cmd_INVITE(std::vector<std::string> args);
		void cmd_TOPIC(std::vector<std::string> args);
		void cmd_MODE(std::vector<std::string> args);
		void cmd_NAMES(std::vector<std::string> args);
};




