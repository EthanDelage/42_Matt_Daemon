#ifndef SOCKETSERVER_HPP
#define SOCKETSERVER_HPP

#include "Socket.hpp"
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8080
#define SERVER_BACKLOG 3

class SocketServer : public Socket {
public:
  SocketServer();
  SocketServer(const SocketServer &other);
  ~SocketServer();

  SocketServer & operator=(const SocketServer & other);

  int accept_client() const;

private:
  sockaddr_in _addr;
};

#endif // SOCKETSERVER_HPP
