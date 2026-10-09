#include <map>
#include <string>
class placeholder
{
	public:
		placeholder();

	private:
		std::map<std::string, void (placeholder::*)(void)> _func;

		void createMap();
		void cmd_PASS();
		void cmd_NICK();
		void cmd_USER();
		void cmd_PART();
		void cmd_LIST();
		void cmd_QUIT();
		void cmd_JOIN();
		void cmd_KICK();
		void cmd_INVITE();
		void cmd_TOPIC();
		void cmd_MODE();
		void cmd_NAMES();
};


