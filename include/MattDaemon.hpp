#ifndef MATTDAEMON_HPP
#define MATTDAEMON_HPP

#include "SocketServer.hpp"

#include <poll.h>
#include <vector>

#define QUIT_CMD "quit\n"
#define MAX_CLIENT 3

class MattDaemon {
public:
   MattDaemon();
   MattDaemon(const MattDaemon &other);
   ~MattDaemon();

   MattDaemon & operator=(const MattDaemon &other);

   void loop();

private:
   void handle_poll_fds();
   void handle_client_fd(pollfd poll_fd);
   void handle_client_connection(pollfd poll_fd);
   void add_poll_fd(pollfd fd);
   void remove_poll_fd(int fd, bool close_fd);
   void set_sighandler();

   SocketServer _server;
   std::vector<pollfd> _poll_fds;
};


#endif //MATTDAEMON_HPP