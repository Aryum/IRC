#include "Server.hpp"


void Server::setupSocket() //socket 
{
    _serverFd = socket(AF_INET, SOCK_STREAM, 0); //creates IPV4 socket

    if (_serverFd == -1)
        throw std::runtime_error("socket failed");

    int opt = 1;

    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) //SO_REUSEADDR deixa usar o mesmo adress num retsart
        throw std::runtime_error("setsockopt failed");

    sockaddr_in address = {};
    address.sin_family = AF_INET; //IPv4
    address.sin_port = htons(_port); //converts port to network byte order
    address.sin_addr.s_addr = htonl(INADDR_ANY);//accepts connection on any local network

    if (bind(_serverFd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) //IP adress and port assigned to the socket
        throw std::runtime_error("bind failed");

    if (listen(_serverFd, SOMAXCONN) == -1) //socket listening for connections
		throw std::runtime_error("listen failed");

	if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) == -1)
        throw std::runtime_error("fcntl failed");

    std::cout << "Server listening on port " << _port << std::endl;
}

void Server::start()
{
    std::signal(SIGINT, set_shutDown);
    std::signal(SIGTERM, set_shutDown);
    setupSocket();

    pollfd serverPoll;
    serverPoll.fd = _serverFd;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;

    _pollFds.push_back(serverPoll);

    run();
    shutdownServer();
}


void Server::acceptClients()
{
    while (!_shutDownSignal)
    {
        sockaddr_in clientAddress;
        socklen_t clientLength = sizeof(clientAddress);

        int clientFd = accept(_serverFd,reinterpret_cast<sockaddr*>(&clientAddress), &clientLength);

        if (clientFd == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;

            if (errno == EINTR)
                continue;

            throw std::runtime_error("accept failed");
        }

		if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
        {
            close(clientFd);
	        std::cerr << "Error: fcntl client failed" << std::endl;;
			return;
		}

        char ipBuffer[INET_ADDRSTRLEN];
        const char* ip = inet_ntop(AF_INET, &clientAddress.sin_addr, ipBuffer, sizeof(ipBuffer));

        if (ip == NULL)
        {
            close(clientFd);
			std::cerr << "Error: inet_ntop failed" << std::endl;;
			return;
        }

        int clientPort = ntohs(clientAddress.sin_port);

        Client* client = new Client(clientFd);
        _clients[clientFd] = client;

        pollfd clientPoll;
        clientPoll.fd = clientFd;
        clientPoll.events = POLLIN;
        clientPoll.revents = 0;

        _pollFds.push_back(clientPoll);

        std::cout << "Client connected. fd=" << clientFd  << ", ip=" << ip << ", port=" << clientPort<< std::endl;
    }
}

void Server::run()
{
    while (!_shutDownSignal)
    {
        int result = poll(&_pollFds[0], _pollFds.size(), -1);

        if (result == -1)
        {
            if (errno == EINTR)
                continue;

            throw std::runtime_error("poll failed");
        }
        
        size_t i = _pollFds.size() - 1;

        while (i >= 1)
        {
            int clientFd = _pollFds[i].fd;
            short events = _pollFds[i].revents;

            if (events & (POLLERR | POLLHUP | POLLNVAL))
            {
                removeData(i);
                --i;
                continue;
            }

            if (events & POLLIN)
            {
                receiveData(i);
				//static_cast<std::vector<pollfd>::size_type>()
        // receiveData may have removed this client.
                if (i >= _pollFds.size() || _pollFds[i].fd != clientFd)
                {
                    --i;
                    continue;
                }
            }
            if (events & POLLOUT)
                sendData(i);

            --i;
        }
		for (size_t h = 0; h < _quitingClient.size(); h++)
			removeData(_quitingClient[h]);
		_quitingClient.clear();
        if (_pollFds[0].revents & POLLIN)
            acceptClients();
    }

}


void Server::receiveData(int index)
{
    char buffer[512];
    int clientFd = _pollFds[index].fd;
	Client *client = _clients[clientFd];
	ssize_t bytesReceived = recv(_pollFds[index].fd, buffer, sizeof(buffer), 0);

	if (bytesReceived >= 512)
		return (client->extractLine(), err_INPUTTOOLONG(*client));
	if(bytesReceived <= 0)
	{
		removeData(index);
		return;
	}
	std::string data(buffer, bytesReceived);
	client->appendInput(data);

	while (client->hasCompleteLine())
	{
		std::string line = client->extractLine();
		std::cout << BLUE << "Message from " << client->get(Name_Nick) << ": " << WHITE << line  << std::endl;
		call_cmd(*client, line);
	}
	if (client->isQuiting())
		_quitingClient.push_back(index);
	
}
void Server::removeChannel(Channel &channel)
{
	std::vector<Channel>::iterator it = std::find(_channels.begin(), _channels.end(), channel);
	if (it != _channels.end())
		_channels.erase(it);
}

void Server::removeData(int index)
{
    int clientFd = _pollFds[index].fd;

    std::map<int, Client*>::iterator it = _clients.find(clientFd);

    if (it != _clients.end())
    {
		removeClientChannels(*it->second);
        delete it->second;
        _clients.erase(it);
    }
    close(clientFd);

    _pollFds.erase(_pollFds.begin() + index);

    std::cout << "Disconnected fd=" << clientFd << std::endl;

}

void Server::serverToClient(Client& target, const std::string& message)
{
	std::cout << YELLOW;
	sendToClient(target, ":IRC", message);
}
void Server::clientToClient(Client& target, Client& sender, const std::string& message)
{
	std::cout << GREEN; 
	sendToClient(target, ":" + sender.get(Name_Full), message);
}

void Server::sendToClient(Client &client, const std::string& sender, const std::string& message)
{
    client.appendOutput(sender + " " + message + "\r\n");

	std::cout << sender << " message to " << client.get(Name_Nick) << ": " << WHITE << message  << std::endl;

    for (std::vector<pollfd>::size_type i = 0; i < _pollFds.size(); ++i)
    {
        if (_pollFds[i].fd == client.getSocket())
        {
            _pollFds[i].events |= POLLOUT;
            break;
        }
    }
}

void Server::sendData(int index)
{
    int clientFd = _pollFds[index].fd;

    std::map<int, Client*>::iterator it = _clients.find(clientFd);

    if (it == _clients.end())
    {
        removeData(index);
        return;
    }

    Client* client = it->second;

    if (client->hasPendingOutput())
    {
        const std::string& data = client->getOutputBuffer();

        ssize_t bytesSent = send(clientFd, data.data(), data.size(), 0);

		if (bytesSent <= 0)
		{
			removeData(index);
        	return;
		}
		client->removeSentOutput(bytesSent);
    }

	if (!client->hasPendingOutput())
    	_pollFds[index].events &= ~POLLOUT;
}

void Server::shutdownServer()
{
    std::cout << "Shutting down server..." << std::endl;
   
    while (_pollFds.size() > 1)
        removeData(_pollFds.size() - 1);

    // Close listening socket
    if (_serverFd != -1)
    {
        close(_serverFd);
        _serverFd = -1;
    }

    _pollFds.clear();
}