#include "SocketServer.hpp"

#include <stdexcept>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>

SocketServer::SocketServer() {
  _fd = socket(AF_INET, SOCK_STREAM, 0);
  if (_fd < 0) {
    throw std::runtime_error(std::string("socket: ") + strerror(errno));
  }
  int opt = 1;
  if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    throw std::runtime_error(std::string("setsockopt: ") + strerror(errno));
  }
  _addr.sin_family = AF_INET;
  _addr.sin_port = htons(SERVER_PORT);
  _addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
  if (bind(_fd, reinterpret_cast<sockaddr *>(&_addr), sizeof(_addr)) < 0) {
    throw std::runtime_error(std::string("bind: ") + strerror(errno));
  }
  if (listen(_fd, SERVER_BACKLOG) < 0) {
    throw std::runtime_error(std::string("listen: ") + strerror(errno));
  }
}

SocketServer::SocketServer(const SocketServer & other) : Socket(other._fd) {
  _addr = other._addr;
}

SocketServer::~SocketServer() {
}

SocketServer & SocketServer::operator=(const SocketServer & other) {
  if (this == &other) {
    return *this;
  }
  if (_fd != -1) {
    close(_fd);
  }
  _fd = other._fd;
  _addr = other._addr;
  return *this;
}

int SocketServer::accept_client() const {
  int client_fd = accept(_fd, nullptr, nullptr);
  if (client_fd < 0) {
    throw std::runtime_error(std::string("bind: ") + strerror(errno));
  }
  return client_fd;
}
