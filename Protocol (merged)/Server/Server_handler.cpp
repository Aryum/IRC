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

    int flags = fcntl(_serverFd, F_GETFL, 0);

    if (flags == -1 || fcntl(_serverFd, F_SETFL, flags | O_NONBLOCK) == -1)
        throw std::runtime_error("fcntl failed");

    std::cout << "Server listening on port " << _port << std::endl;
}

void Server::start()
{
    setupSocket();

    pollfd serverPoll;
    serverPoll.fd = _serverFd;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;

    _pollFds.push_back(serverPoll);

    run();
}


void Server::acceptClients()
{
    while (true)
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

        int flags = fcntl(clientFd, F_GETFL, 0);

        if (flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            close(clientFd);
            throw std::runtime_error("fcntl client failed");
        }

        char ipBuffer[INET_ADDRSTRLEN];
        const char* ip = inet_ntop(AF_INET, &clientAddress.sin_addr, ipBuffer, sizeof(ipBuffer));

        if (ip == NULL)
        {
            close(clientFd);
            throw std::runtime_error("inet_ntop failed");
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
    while (true)
    {
        int result = poll(&_pollFds[0], _pollFds.size(), -1);

        if (result == -1)
        {
            if (errno == EINTR)
                continue;

            throw std::runtime_error("poll failed");
        }
        
        int i = static_cast<int>(_pollFds.size()) - 1;

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

        // receiveData may have removed this client.
                if (static_cast<std::vector<pollfd>::size_type>(i) >= _pollFds.size() || _pollFds[i].fd != clientFd)
                {
                    --i;
                    continue;
                }
            }
            if (events & POLLOUT)
                sendData(i);

            --i;
        }   
        if (_pollFds[0].revents & POLLIN)
            acceptClients();
    }

}


void Server::receiveData(int index)
{
    char buffer[512];
    int clientFd = _pollFds[index].fd;

    while(true)
    {
        ssize_t bytesReceived = recv(_pollFds[index].fd, buffer, sizeof(buffer), 0);
        std::cout << "recv returned: " << bytesReceived << std::endl;

        if(bytesReceived > 0)
        {
            std::map<int, Client*>::iterator it = _clients.find(clientFd);

            if (it == _clients.end())
            {
                removeData(index);
                return;
            }

            std::string data(buffer, bytesReceived);
            it->second->appendInput(data);

            std::cout << "Buffer contents: [";

            for (std::string::size_type i = 0; i < data.size(); ++i)
            {
				
                if (data[i] == '\r')
                    std::cout << "\\r";
                else if (data[i] == '\n')
                    std::cout << "\\n";
                else
                    std::cout << data[i];
            }

            std::cout << "]" << std::endl;

            while (it->second->hasCompleteLine())
            {
                std::string line = it->second->extractLine();
                std::cout << "IRC message from fd=" << clientFd << ": " << line << std::endl;
				call_cmd(*it->second,line );
			}

            continue;
        }

        if(bytesReceived == 0)
        {
            removeData(index);
            continue; //remove later maybe
        }

        if(errno == EINTR)
            continue;

        if(errno == EAGAIN)
            return;

        removeData(index);
        return;

    }
}

void Server::removeData(int index)
{
    int clientFd = _pollFds[index].fd;

    std::map<int, Client*>::iterator it = _clients.find(clientFd);

    if (it != _clients.end())
    {
        delete it->second;
        _clients.erase(it);
    }
    close(clientFd);

    _pollFds.erase(_pollFds.begin() + index);

    std::cout << "Disconnected fd=" << clientFd << std::endl;

}

void Server::serverToClient(int clientFd, const std::string& message)
{
	sendToClient(clientFd, ":IRC", message);
}
void Server::clientToClient(int clientFd, Client& sender, const std::string& message)
{
	sendToClient(clientFd, ":" + sender.get(Name_Full), message);
}

void Server::sendToClient(int clientFd, const std::string& sender, const std::string& message)
{
	std::cout << YELLOW << "	Sending to client: " << message << WHITE << std::endl;
    std::map<int, Client*>::iterator it = _clients.find(clientFd);

    if (it == _clients.end())
        return;

    it->second->appendOutput(sender + " " + message + "\r\n");

    for (std::vector<pollfd>::size_type i = 0; i < _pollFds.size(); ++i)
    {
        if (_pollFds[i].fd == clientFd)
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

    while (client->hasPendingOutput())
    {
        const std::string& data = client->getOutputBuffer();

        ssize_t bytesSent = send(clientFd, data.data(), data.size(), 0);

        if (bytesSent > 0)
        {
            client->removeSentOutput(bytesSent);
            continue;
        }

        if (bytesSent == -1 && errno == EINTR)
            continue;

        if (bytesSent == -1 &&
            (errno == EAGAIN || errno == EWOULDBLOCK))
            return;

        removeData(index);
        return;
    }

    _pollFds[index].events &= ~POLLOUT;
}