#include "Server.hpp"
void Server::cmd_PASS(Client &client, Args args)
{
	if (_clients.find(client.getSocket()) != _clients.end())
		client.sendReply("ERR_ALREADYREGISTRED");
	else if (args.size() == 0)
		client.sendReply("ERR_NEEDMOREPARAMS");
		return ; 
	if (args[0] == _password)
		return ; //NICK CMD check how  is it going to work
	
	/* 

	Command: PASS
	Parameters: <password>

	The PASS command is used to set a 'connection password'.  The
	password can and must be set before any attempt to register the
	connection is made.  Currently this requires that clients send a PASS
	command before sending the NICK/USER combination and servers *must*
	send a PASS command before any SERVER command.  The password supplied
	must match the one contained in the C/N lines (for servers) or I
	lines (for clients).  It is possible to send multiple PASS commands
	before registering but only the last one sent is used for
	verification and it may not be changed once registered.  Numeric
	Replies:

			ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED

	Example:

           PASS secretpasswordhere
	*/
}

void Server::cmd_NICK(Client &client, Args args)
{
	if (args.size() == 0)
	{
		client.sendReply("ERR_NONICKNAMEGIVEN");
		return ; 
	}
	
	ClientMap::iterator it = _clients.begin();
	bool newConnection = false;
	while (it != _clients.end())
	{
		if (it->first == client.getSocket())
			newConnection = false;
		if (it->second->getNickName() == args[0])
		{
			client.sendReply("ERR_NICKCOLLISION");
			return ;
		}
		it++;
	}

	if (!newConnection)
		return ;//replace name prob just change &client
	else
		return ;//USER CMD;
	/*
	Command: NICK
   Parameters: <nickname> [ <hopcount> ]

   NICK message is used to give user a nickname or change the previous
   one.  The <hopcount> parameter is only used by servers to indicate
   how far away a nick is from its home server.  A local connection has
   a hopcount of 0.  If supplied by a client, it must be ignored.

   If a NICK message arrives at a server which already knows about an
   identical nickname for another client, a nickname collision occurs.
   As a result of a nickname collision, all instances of the nickname
   are removed from the server's database, and a KILL command is issued
   to remove the nickname from all other server's database. If the NICK
   message causing the collision was a nickname change, then the
   original (old) nick must be removed as well.

   If the server recieves an identical NICK from a client which is
   directly connected, it may issue an ERR_NICKCOLLISION to the local
   client, drop the NICK command, and not generate any kills.
	Numeric Replies:

           ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
           ERR_NICKNAMEINUSE               ERR_NICKCOLLISION

   Example:

   NICK Wiz                        ; Introducing new nick "Wiz".

   :WiZ NICK Kilroy                ; WiZ changed his nickname to Kilroy.
	
	*/
}
void Server::cmd_USER(Client &client, Args args)
{
	if (args.size() < 4)
	{
		client.sendReply("ERR_NEEDMOREPARAMS");
		return ;
	}
	ClientMap::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->first == client.getSocket())
		{
			client.sendReply("ERR_ALREADYREGISTRED");
			return ;
		}
		it++;
	}
	client.setNames(args[0], args[1], args[2], args[3]);
	_clients.insert(std::make_pair(client.getSocket(), &client));
	//actually add to client list at the end of this cmd
	/*
	 Command: USER
   Parameters: <username> <hostname> <servername> <realname>

   The USER message is used at the beginning of connection to specify
   the username, hostname, servername and realname of s new user.  It is
   also used in communication between servers to indicate new user
   arriving on IRC, since only after both USER and NICK have been
   received from a client does a user become registered.

   Between servers USER must to be prefixed with client's NICKname.
   Note that hostname and servername are normally ignored by the IRC
   server when the USER command comes from a directly connected client
   (for security reasons), but they are used in server to server
   communication.  This means that a NICK must always be sent to a
   remote server when a new user is being introduced to the rest of the
   network before the accompanying USER is sent.

   It must be noted that realname parameter must be the last parameter,
   because it may contain space characters and must be prefixed with a
   colon (':') to make sure this is recognised as such.

   Since it is easy for a client to lie about its username by relying
   solely on the USER message, the use of an "Identity Server" is
   recommended.  If the host which a user connects from has such a
   server enabled the username is set to that as in the reply from the
   "Identity Server".

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED

   Examples:


   USER guest tolmoon tolsun :Ronnie Reagan
	; User registering themselves with a
                                   username of "guest" and real name
                                   "Ronnie Reagan".


   :testnick USER guest tolmoon tolsun :Ronnie Reagan
                                   ; message between servers with the
                                   nickname for which the USER command
                                   belongs to
	*/
}



void Server::cmd_QUIT(Client &client, Args args)
{
	_clients.erase(client.getSocket());
	std::string reason;
	if (args.size() != 0)
		reason = args[0];
	else
		reason = "Quit";
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i].getClientStatus(client) != Status_None)
		{
			//shit will need fixing
			//shit will need fixing
			//shit will need fixing
			//shit will need fixing
			//shit will need fixing
			//shit will need fixing
			//broadcast message
			_channels[i].broadcastMessage(reason); //will need formating
			_channels[i].partClient(client);
		}
	}
	//close connection

	/*
	 Command: QUIT
   Parameters: [<Quit message>]

   A client session is ended with a quit message.  The server must close
   the connection to a client which sends a QUIT message. If a "Quit
   Message" is given, this will be sent instead of the default message,
   the nickname.

   When netsplits (disconnecting of two servers) occur, the quit message
   is composed of the names of two servers involved, separated by a
   space.  The first name is that of the server which is still connected
   and the second name is that of the server that has become
   disconnected.

   If, for some other reason, a client connection is closed without  the
   client  issuing  a  QUIT  command  (e.g.  client  dies and EOF occurs
   on socket), the server is required to fill in the quit  message  with
   some sort  of  message  reflecting the nature of the event which
   caused it to happen.

   Numeric Replies:

           None.

   Examples:

   QUIT :Gone to have lunch        ; Preferred message format.
	*/
}

void Server::cmd_JOIN(Client &client, Args args)
{
	if (args.size() < 1)
	{
		client.sendReply("ERR_NEEDMOREPARAMS");
		return ;
	}
	size_t channelIdx = 0;
	size_t keyIdx = 0;

	while (channelIdx != std::string::npos)
	{
		std::string name = nextChar(args[0], channelIdx, ',', true);
		std::string password = nextChar(args[0], channelIdx, ',', true);

		Channel *channel = getChannel(name);
		std::string reply;
		if (channel == nullptr)
			_channels.push_back(Channel(client, name));
		else
			channel->joinClient(client, password);
		
	}
	/*
	   Command: JOIN
   Parameters: <channel>{,<channel>} [<key>{,<key>}]

   The JOIN command is used by client to start listening a specific
   channel. Whether or not a client is allowed to join a channel is
   checked only by the server the client is connected to; all other
   servers automatically add the user to the channel when it is received
   from other servers.  The conditions which affect this are as follows:

           1.  the user must be invited if the channel is invite-only;
           2.  the user's nick/username/hostname must not match any
               active bans;
           3.  the correct key (password) must be given if it is set.

   These are discussed in more detail under the MODE command (see
   section 4.2.3 for more details).

   Once a user has joined a channel, they receive notice about all
   commands their server receives which affect the channel.  This
   includes MODE, KICK, PART, QUIT and of course PRIVMSG/NOTICE.  The
   JOIN command needs to be broadcast to all servers so that each server
   knows where to find the users who are on the channel.  This allows
   optimal delivery of PRIVMSG/NOTICE messages to the channel.

   If a JOIN is successful, the user is then sent the channel's topic
   (using RPL_TOPIC) and the list of users who are on the channel (using
   RPL_NAMREPLY), which must include the user joining.

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_BANNEDFROMCHAN
           ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
           ERR_CHANNELISFULL               ERR_BADCHANMASK
           ERR_NOSUCHCHANNEL               ERR_TOOMANYCHANNELS
           RPL_TOPIC

   Examples:

   JOIN #foobar                    ; join channel #foobar.

   JOIN &foo fubar                 ; join channel &foo using key "fubar".

   JOIN #foo,&bar fubar            ; join channel #foo using key "fubar"
                                   and &bar using no key.

   JOIN #foo,#bar fubar,foobar     ; join channel #foo using key "fubar".
                                   and channel #bar using key "foobar".

   JOIN #foo,#bar                  ; join channels #foo and #bar.

   :WiZ JOIN #Twilight_zone        ; JOIN message from WiZ
	
	*/
}


void Server::cmd_PART(Client &client, Args args)
{
	if (args.size() < 1)
	{
		client.sendReply("ERR_NEEDMOREPARAMS");
		return ;
	}
	size_t channelIdx = 0;
	bool foundChannel = false;
	while (channelIdx != std::string::npos)
	{
		std::string name = nextChar(args[0], channelIdx, ',', true);
		Channel *channel = nullptr;
		if (getChannel(client, name, &channel))
			return ;
		else
			channel->partClient(client);
	}
	/*
	Command: PART
   Parameters: <channel>{,<channel>}
	The PART message causes the client sending the message to be removed
   from the list of active users for all given channels listed in the
   parameter string.

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
           ERR_NOTONCHANNEL

   Examples:

   PART #twilight_zone             ; leave channel "#twilight_zone"

   PART #oz-ops,&group5            ; leave both channels "&group5" and
                                   "#oz-ops".
	*/
}
void Server::cmd_KICK(Client &client, Args args)
{
	if (args.size() < 2)
	{
		client.sendReply("ERR_NEEDMOREPARAMS");
		return ;
	}

	size_t channelIdx = 0;
	size_t userIdx = 0;
	bool foundChannel = false;
	while (channelIdx != std::string::npos)
	{
		std::string channelName = nextChar(args[0], channelIdx, ',', true);
		std::string nickName = nextChar(args[1], userIdx, ',', true);
		if (userIdx == std::string::npos && nickName == "")
			break ;
		Channel *channel = nullptr;
		Client *user = nullptr;
		if (!getChannel(client, channelName, &channel) || !getClient(client, nickName, &user));
			continue ; 
		channel->kickClient(client, *user, args.size() >= 3 ? args[2] : "");
	}
	/*
NOTE:
     It is possible to extend the KICK command parameters to the
following:

<channel>{,<channel>} <user>{,<user>} [<comment>]
	*/

	/*
		Command: KICK
   Parameters: <channel> <user> [<comment>]

   The KICK command can be  used  to  forcibly  remove  a  user  from  a
   channel.   It  'kicks  them  out'  of the channel (forced PART).
	Only a channel operator may kick another user out of a  channel.
   Each  server that  receives  a KICK message checks that it is valid
   (ie the sender is actually a  channel  operator)  before  removing
   the  victim  from  the channel.

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
           ERR_BADCHANMASK                 ERR_CHANOPRIVSNEEDED
           ERR_NOTONCHANNEL

   Examples:

KICK &Melbourne Matthew         ; Kick Matthew from &Melbourne

KICK #Finnish John :Speaking English
                                ; Kick John from #Finnish using
                                "Speaking English" as the reason
                                (comment).

:WiZ KICK #Finnish John         ; KICK message from WiZ to remove John
                                from channel #Finnish

NOTE:
     It is possible to extend the KICK command parameters to the
following:

<channel>{,<channel>} <user>{,<user>} [<comment>]
	*/
}

void Server::cmd_INVITE(Client &client, Args args)
{
	if (args.size() < 2)
	{
		client.sendReply("ERR_NEEDMOREPARAMS");
		return ;
	}
	Client *user = nullptr;
	Channel *channel = nullptr;
	if (!getClient(client, args[1], &user) || !getChannel(client, args[2], &channel))
		return ; 
	channel->inviteClient(client, *user);
	/*
		Command: INVITE
   Parameters: <nickname> <channel>

   The INVITE message is used to invite users to a channel.  The
   parameter <nickname> is the nickname of the person to be invited to
   the target channel <channel>.  There is no requirement that the
   channel the target user is being invited to must exist or be a valid
   channel.  To invite a user to a channel which is invite only (MODE
   +i), the client sending the invite must be recognised as being a
   channel operator on the given channel.

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
           ERR_NOTONCHANNEL                ERR_USERONCHANNEL
           ERR_CHANOPRIVSNEEDED
           RPL_INVITING                    RPL_AWAY

   Examples:

   :Angel INVITE Wiz #Dust         ; User Angel inviting WiZ to channel
                                   #Dust

   INVITE Wiz #Twilight_Zone       ; Command to invite WiZ to
                                   #Twilight_zone
	
	*/
}
void Server::cmd_TOPIC(Client &client, Args args)
{
	if (args.size() < 1)
	{
		client.sendReply("ERR_NEEDMOREPARAMS");
		return ;
	}
	Channel *channel = nullptr;
	if (!getChannel(client, args[0], &channel))
		return ; //ERR_NOSUCHCHANNEL
	if (args.size() == 1)
		channel->getTopic(client);
	else
		channel->changeTopic(client, args[1]);
	
	/*
		Command: TOPIC
   Parameters: <channel> [<topic>]

   The TOPIC message is used to change or view the topic of a channel.
   The topic for channel <channel> is returned if there is no <topic>
   given.  If the <topic> parameter is present, the topic for that
   channel will be changed, if the channel modes permit this action.

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
           RPL_NOTOPIC                     RPL_TOPIC
           ERR_CHANOPRIVSNEEDED
		    Examples:

   :Wiz TOPIC #test :New topic     ;User Wiz setting the topic.

   TOPIC #test :another topic      ;set the topic on #test to "another
                                   topic".

   TOPIC #test                     ; check the topic for #test.
	*/
}
void Server::cmd_MODE(Client &client, Args args)
{
	if (args.size() < 2)
	{
		client.sendReply("ERR_NEEDMOREPARAMS");
		return ;
	}
	Channel *channel = nullptr;
	if (!getChannel(client, args[0], &channel))
		return ; //ERR_NOSUCHCHANNEL
	channel->changeMode(client, args);


	//for user check if client arg
	/*
		Command: MODE

   The MODE command is a dual-purpose command in IRC.  It allows both
   usernames and channels to have their mode changed.  The rationale for
   this choice is that one day nicknames will be obsolete and the
   equivalent property will be the channel.

   When parsing MODE messages, it is recommended that the entire message
   be parsed first and then the changes which resulted then passed on.
	4.2.3.1 Channel modes

   Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>]
               [<ban mask>]

   The MODE command is provided so that channel operators may change the
   characteristics of `their' channel.  It is also required that servers
   be able to change channel modes so that channel operators may be
   created.

   The various modes available for channels are as follows:
           i - invite-only channel flag;
           t - topic settable by channel operator only flag;
		   k - set a channel key (password).
           o - give/take channel operator privileges;
           l - set the user limit to channel;


   When using the 'o' and 'b' options, a restriction on a total of three
   per mode command has been imposed.  That is, any combination of 'o'
   and
*/



/*

   Replies:

           ERR_NEEDMOREPARAMS              RPL_CHANNELMODEIS
           ERR_CHANOPRIVSNEEDED            ERR_NOSUCHNICK
           ERR_NOTONCHANNEL                ERR_KEYSET
           RPL_BANLIST                     RPL_ENDOFBANLIST
           ERR_UNKNOWNMODE                 

           ERR_USERSDONTMATCH              RPL_UMODEIS
           ERR_UMODEUNKNOWNFLAG

   Examples:

           Use of Channel Modes:

MODE #Finnish +im               ; Makes #Finnish channel moderated and
                                'invite-only'.

MODE #Finnish +o Kilroy         ; Gives 'chanop' privileges to Kilroy on
 channel #Finnish.

MODE #Finnish +v Wiz            ; Allow WiZ to speak on #Finnish.

MODE #Fins -s                   ; Removes 'secret' flag from channel
                                #Fins.

MODE #42 +k oulu                ; Set the channel key to "oulu".

MODE #eu-opers +l 10            ; Set the limit for the number of users
                                on channel to 10.

MODE &oulu +b                   ; list ban masks set for channel.

MODE &oulu +b *!*@*             ; prevent all users from joining.

MODE &oulu +b *!*@*.edu         ; prevent any user from a hostname
                                matching *.edu from joining.

        Use of user Modes:

:MODE WiZ -w                    ; turns reception of WALLOPS messages
                                off for WiZ.

:Angel MODE Angel +i            ; Message from Angel to make themselves
                                invisible.

MODE WiZ -o                     ; WiZ 'deopping' (removing operator
                                status).  The plain reverse of this
                                command ("MODE WiZ +o") must not be
                                allowed from users since would bypass
                                the OPER command.
	*/

}

void Server::cmd_LIST(Client &client, Args args)
{
	/*
	Command: LIST
   Parameters: [<channel>{,<channel>} [<server>]]

   The list message is used to list channels and their topics.  If  the
   <channel>  parameter  is  used,  only  the  status  of  that  channel
   is displayed.  Private  channels  are  listed  (without  their
   topics)  as channel "Prv" unless the client generating the query is
   actually on that channel.  Likewise, secret channels are not listed
   at  all  unless  the client is a member of the channel in question.

   Numeric Replies:

           ERR_NOSUCHSERVER                RPL_LISTSTART
           RPL_LIST                        RPL_LISTEND

   Examples:

   LIST                            ; List all channels.

   LIST #twilight_zone,#42         ; List channels #twilight_zone and #42
	*/
}