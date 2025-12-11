#include "Daemon.hpp"

#include "TintinReporter.hpp"

#include <fcntl.h>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>

Daemon::Daemon(): _fd(-1) {};

Daemon::Daemon(const Daemon &other) {
  *this = other;
}

Daemon::~Daemon() {
  close(_fd);
  unlink(DAEMON_LOCKFILE);
}

Daemon &Daemon::operator=(const Daemon &other) {
  if (this != &other) {
    _fd = other._fd;
  }
  return *this;
}

int Daemon::start(const char *daemon_user) {
  uid_t uid;
  gid_t gid;

  if (geteuid() != 0) {
    TintinReporter::get_instance().error("start: Daemon must start as root");
    return -1;
  }

  struct passwd *pw = getpwnam(daemon_user);
  if (!pw) {
    TintinReporter::get_instance().error(std::string(
        std::string("start: User '") + daemon_user + "' not found"));
    return -1;
  }

  uid = pw->pw_uid;
  gid = pw->pw_gid;

  int pidfd = create_lockfile(uid, gid);
  if (pidfd < 0) {
    return -1;
  }

  if (daemon() < 0) {
    TintinReporter::get_instance().error(
        std::string("start: failed to daemon: ") + strerror(errno));
    return -1;
  }

  if (setgid(gid) < 0) {
    TintinReporter::get_instance().error(std::string("start: setgid: ") +
                                 strerror(errno));
    return -1;
  }
  if (setuid(uid) < 0) {
    TintinReporter::get_instance().error(std::string("start: setgid: ") +
                                 strerror(errno));
    return -1;
  }

  return pidfd;
}

int Daemon::daemon() {
  pid_t pid = fork();
  if (pid < 0) {
    TintinReporter::get_instance().error(std::string("Taskmaster::daemon: fork(): ") +
                                 strerror(errno));
    return -1;
  }
  if (pid > 0) {
    TintinReporter::get_instance().info("daemon: exiting parent process");
    exit(0);
  }

  if (setsid() < 0) {
    TintinReporter::get_instance().error(std::string("daemon: setsid(): ") +
                                 strerror(errno));
    return -1;
  }

  umask(0);

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  if (open("/dev/null", O_RDWR) != STDIN_FILENO) { /* 'fd' should be 0 */
    TintinReporter::get_instance().error("daemon: STDIN fd not equal to 0");
    return -1;
  }
  if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO) {
    TintinReporter::get_instance().error("daemon: STDOUT fd not equal to 1");
    return -1;
  }
  if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO) {
    TintinReporter::get_instance().error("daemon: STDERR fd not equal to 1");
    return -1;
  }
  return 0;
}

int Daemon::create_lockfile(uid_t uid, gid_t gid) {
  int fd = open(DAEMON_LOCKFILE, O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    TintinReporter::get_instance().error(std::string("create_lockfile: open: ") +
                                 strerror(errno));
    return -1;
  }

  if (flock(fd, LOCK_EX | LOCK_NB) < 0) {
    if (errno == EWOULDBLOCK)
      TintinReporter::get_instance().error("create_lockfile: Daemon already running !");
    else
      TintinReporter::get_instance().error(std::string("create_lockfile: flock: ") +
                                   strerror(errno));
    close(fd);
    return -1;
  }

  if (ftruncate(fd, 0) < 0) {
    TintinReporter::get_instance().error(std::string("create_lockfile: ftruncate: ") +
                                 strerror(errno));
    close(fd);
    return -1;
  }

  dprintf(fd, "%d\n", getpid());

  if (fchown(fd, uid, gid) < 0) {
    TintinReporter::get_instance().error(std::string("create_lockfile: fchown: ") +
                                 strerror(errno));
    close(fd);
    return -1;
  }

  return fd;
}
