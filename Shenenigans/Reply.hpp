#ifndef REPLY_HPP
# define REPLY_HPP
#include <string>

class Reply
{
	public:
		Reply(std::string msg);
	
		std::string getMessage();
	private:
		std::string _message;
};

#endif