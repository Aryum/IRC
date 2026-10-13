#include <string>
class Client
{
	public:
		Client();

	private:
		std::string _userName;
		std::string _nickName;
		std::string _password;
		int _socket;
		bool _isOperator;
};