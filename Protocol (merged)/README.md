*This project has been created as part
of the 42 curriculum by jlopes-c, ricsanto*

# FT_IRC

## Description

FT_IRC is a networking project from the 42 curriculum. Through this project, you will learn how IRC (Internet Relay Chat) servers work by implementing your own IRC server in C++.

The goal of the project is to build an IRC server capable of handling multiple clients simultaneously, allowing users to connect, authenticate, join channels, exchange messages, and communicate using the IRC protocol.

The project focuses on network programming, socket management, TCP connections, and the implementation of a communication protocol.

---

## Instructions

### Running the Server

To compile and start the IRC server locally:

1. Clone the repository.

2. Navigate to the project directory.

3. Compile the project using the Makefile.

```bash
make
```

4. Start the server by providing a port and a password.

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 8080 mypassword
```

This will start the IRC server, listening for incoming client connections on the specified port.

### Connecting to the Server

You can connect to your IRC server using an IRC client. We used **Hexchat** and **Netcat** to test. Results may vary with others clients, since they can expect some other responses.

Example using netcat:

```bash
nc -C 127.0.0.1 8080
```

Once connected, you can register using the following IRC commands:

- `PASS` – Authenticate with the server password.
- `NICK` – Set your nickname.
- `USER` – Register your username.
- `JOIN` – Join an existing channel or create a new one.
- `PRIVMSG` – Send private messages or channel messages.
- `KICK` – Remove a user from a channel.
- `INVITE` – Invite a user to a channel.
- `TOPIC` – View or change a channel's topic.
- `MODE` – Change channel or user modes.

### Using the Server

Once connected, multiple clients can communicate through the same server.

The server supports channel management, private messaging, user authentication, and operator privileges.

The objective is to ensure that all clients can interact correctly while respecting the IRC protocol and the permissions associated with each channel.

---

## Resources

### Concepts Studied

- TCP/IP networking.
- Socket programming.
- Client-server architecture.
- Non-blocking I/O.
- Polling and file descriptor management.
- IRC protocol and command parsing.
- TCP connections and data transmission.
- Channel and user management.
- Event-driven programming.
- C++ and object-oriented programming.

### References

- [RFC 1459 – Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc1459) – Original specification of the IRC protocol.
- [RFC 2812 – Internet Relay Chat: Client Protocol](https://www.rfc-editor.org/rfc/rfc2812) – IRC client protocol specification.
- [Internet Relay Chat by Ridwane EL FILALI](https://dev.to/ridwaneelfilali/internet-relay-chat-37ll) – A useful guide to socket programming and network communication.

### AI tools were used during this project for:

- Helping draft this README.
- Understand when and the format of some server replies.