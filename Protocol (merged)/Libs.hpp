#ifndef LIBS_HPP
# define LIBS_HPP

#include <poll.h>
#include <sys/socket.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cmath>
#include <string>
#include <exception>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <cerrno>
#include <arpa/inet.h>
#include <stdexcept>



typedef const std::vector<std::string>& Args;
typedef std::vector<std::string> EmptyArgs;
typedef std::map<std::string, void (Server::*)(Client &, Args)> CmdMap;
typedef std::map<int, Client*> ClientMap;

#endif