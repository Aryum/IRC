#include "Server.hpp"

/*Server::Server()
{

}

Server::~Server()
{

}
*/
Server::Server(const Server &other)
{
    *this = other;
}

Server &Server::operator=(const Server &other)
{
    if(this != &other)
        *this = other;
    return(*this);
}

Server::Server(int port, const std::string& password): port(port), serverFd(-1), password(password)
{

}

Server::~Server()
{
    if (serverFd != -1)
        close(serverFd);
}

void Server::setupSocket() //socket 
{
    serverFd = socket(AF_INET, SOCK_STREAM, 0); //creates IPV4 socket

    if (serverFd == -1)
        throw std::runtime_error("socket failed");

    int opt = 1;

    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) //SO_REUSEADDR deixa usar o mesmo adress num retsart
        throw std::runtime_error("setsockopt failed");

    sockaddr_in address = {};
    address.sin_family = AF_INET; //IPv4
    address.sin_port = htons(port); //converts port to network byte order
    address.sin_addr.s_addr = htonl(INADDR_ANY);//accepts connection on any local network

    if (bind(serverFd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) //IP adress and port assigned to the socket
        throw std::runtime_error("bind failed");

    if (listen(serverFd, SOMAXCONN) == -1) //socket listening for connections
        throw std::runtime_error("listen failed");

    int flags = fcntl(serverFd, F_GETFL, 0);

    if (flags == -1 || fcntl(serverFd, F_SETFL, flags | O_NONBLOCK) == -1)
        throw std::runtime_error("fcntl failed");

    std::cout << "Server listening on port " << port << std::endl;
}

void Server::start()
{
    setupSocket();

    pollfd serverPoll;
    serverPoll.fd = serverFd;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;

    pollFds.push_back(serverPoll);

    run();
}


void Server::acceptClients()
{
    while (true)
    {
        sockaddr_in clientAddress;
        socklen_t clientLength = sizeof(clientAddress);

        int clientFd = accept(serverFd,reinterpret_cast<sockaddr*>(&clientAddress), &clientLength);

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

        Client* client = new Client(clientFd, ip, clientPort);
        clients[clientFd] = client;

        pollfd clientPoll;
        clientPoll.fd = clientFd;
        clientPoll.events = POLLIN;
        clientPoll.revents = 0;

        pollFds.push_back(clientPoll);

        sendToClient(clientFd, ":myserver NOTICE * :Welcome!\r\n");

        std::cout << "Client connected. fd=" << clientFd  << ", ip=" << ip << ", port=" << clientPort<< std::endl;
    }
}

void Server::run()
{
    while (true)
    {
        int result = poll(&pollFds[0], pollFds.size(), -1);

        if (result == -1)
        {
            if (errno == EINTR)
                continue;

            throw std::runtime_error("poll failed");
        }
        
        int i = static_cast<int>(pollFds.size()) - 1;

        while (i >= 1)
        {
            int clientFd = pollFds[i].fd;
            short events = pollFds[i].revents;

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
                if (static_cast<std::vector<pollfd>::size_type>(i) >= pollFds.size() || pollFds[i].fd != clientFd)
                {
                    --i;
                    continue;
                }
            }
            if (events & POLLOUT)
                sendData(i);

            --i;
        }   
        if (pollFds[0].revents & POLLIN)
            acceptClients();
    }

}


void Server::receiveData(int index)
{
    char buffer[512];
    int clientFd = pollFds[index].fd;

    while(true)
    {
        ssize_t bytesReceived = recv(pollFds[index].fd, buffer, sizeof(buffer), 0);
        std::cout << "recv returned: " << bytesReceived << std::endl;

        if(bytesReceived > 0)
        {
            std::map<int, Client*>::iterator it = clients.find(clientFd);

            if (it == clients.end())
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
    int clientFd = pollFds[index].fd;

    std::map<int, Client*>::iterator it = clients.find(clientFd);

    if (it != clients.end())
    {
        delete it->second;
        clients.erase(it);
    }
    close(clientFd);

    pollFds.erase(pollFds.begin() + index);

    std::cout << "Disconnected fd=" << clientFd << std::endl;

}

void Server::sendToClient(int clientFd, const std::string& message)
{
    std::map<int, Client*>::iterator it = clients.find(clientFd);

    if (it == clients.end())
        return;

    it->second->appendOutput(message);

    for (std::vector<pollfd>::size_type i = 0; i < pollFds.size(); ++i)
    {
        if (pollFds[i].fd == clientFd)
        {
            pollFds[i].events |= POLLOUT;
            break;
        }
    }
}

void Server::sendData(int index)
{
    int clientFd = pollFds[index].fd;

    std::map<int, Client*>::iterator it = clients.find(clientFd);

    if (it == clients.end())
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

    pollFds[index].events &= ~POLLOUT;
}