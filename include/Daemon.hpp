#ifndef DAEMON_HPP
#define DAEMON_HPP

#include <sys/types.h>

#define DAEMON_LOCKFILE "/var/lock/matt_daemon.lock"
#define DAEMON_USER

class Daemon {
public:
    Daemon();
    Daemon(const Daemon &);
    ~Daemon();

    Daemon &operator=(const Daemon &other);

    int start(const char *daemon_user);
private:
    int daemon();
    int create_lockfile(uid_t uid, gid_t gid);

    int _fd;
};


#endif //DAEMON_HPP