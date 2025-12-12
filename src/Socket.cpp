#include "Socket.hpp"

#include <iostream>
#include <unistd.h>

Socket::Socket() : _fd(-1) {}

Socket::Socket(const int fd) : _fd(fd) {}

Socket::Socket(const Socket &other) : _fd(other._fd) {}

Socket::~Socket() {
  if (_fd != -1) {
    close(_fd);
  }
  _fd = -1;
}

Socket &Socket::operator=(const Socket &other) {
  if (this == &other) {
    return *this;
  }
  if (_fd != -1) {
    close(_fd);
  }
  _fd = other._fd;
  return *this;
}

ssize_t Socket::read(char *buffer, size_t size) const {
  return ::read(_fd, buffer, size);
}

ssize_t Socket::write(const std::string &buffer) const {
  return write(_fd, buffer);
}

ssize_t Socket::write(const char *buffer, size_t size) const {
  return write(_fd, buffer, size);
}

ssize_t Socket::read(int fd, char *buffer, size_t size) {
  return ::read(fd, buffer, size);
}

ssize_t Socket::write(int fd, const std::string &buffer) {
  return write(fd, buffer.c_str(), buffer.size());
}

ssize_t Socket::write(int fd, const char *buffer, size_t size) {
  return ::write(fd, buffer, size);
}

int Socket::get_fd() const { return _fd; }

void Socket::set_fd(int fd) { _fd = fd; }
