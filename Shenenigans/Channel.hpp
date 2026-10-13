#include <string>
#include <vector>
#include "Client.hpp"
class Channel
{
	public:
		Channel();

	private:
		std::string _topic;
		std::vector<Client> _connectedClients;

}