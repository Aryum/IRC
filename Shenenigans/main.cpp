#include "placeholder.hpp"

int main(int argc, char **argv)
{
	std::string cmd;
	for (int i = 1; i < argc; i++)
	{
		cmd.append(argv[i]);
		if (i + 1 != argc)
			cmd.append(" ");
		
	}
	placeholder h;
	try
	{
		h.call_cmd(cmd);
	}
	catch(const std::exception& e)
	{
		std::cerr << "ERROR " << e.what() << '\n';
	}
	
}