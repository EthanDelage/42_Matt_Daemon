#include "MattDaemon.hpp"

#include <unistd.h>
#include <signal.h>

#include "TintinReporter.hpp"

volatile sig_atomic_t running = 1;

static void signal_handler(int sig);

MattDaemon::MattDaemon() {
    add_poll_fd({_server.get_fd(), POLLIN, 0});
};

MattDaemon::MattDaemon(const MattDaemon &other) {
    *this = other;
}

MattDaemon::~MattDaemon(){
    for (size_t i = 0; i < _poll_fds.size(); i++) {
        if (i == 0) {
            continue;
        }
        close(_poll_fds[i].fd);
    }
}

MattDaemon & MattDaemon::operator=(const MattDaemon &other) {
    if (this != &other) {
        _server = other._server;
        _poll_fds = other._poll_fds;
    }
    return *this;
}

void MattDaemon::loop() {
    set_sighandler();
    while (running) {
        int result = poll(_poll_fds.data(), _poll_fds.size(), -1);
        if (result == -1) {
            if (errno != EINTR) {
                throw std::runtime_error(std::string("poll()") + strerror(errno));
            }
            continue;
        }
        handle_poll_fds();
    }
    TintinReporter::get_instance().info("Quitting MattDaemon...");
}

void MattDaemon::handle_poll_fds() {
    handle_client_connection(_poll_fds[0]);
    for (size_t i = 1; i < _poll_fds.size(); i++) {
        if (_poll_fds[i].revents) {
            handle_client_fd(_poll_fds[i]);
        }
    }
}

void MattDaemon::handle_client_fd(pollfd poll_fd) {
    char buffer[SOCKET_BUFFER_SIZE];

    if (poll_fd.revents & POLLIN) {
        ssize_t ret = Socket::read(poll_fd.fd, buffer, SOCKET_BUFFER_SIZE);
        if (ret <= 0) {
            remove_poll_fd(poll_fd.fd, true);
            return;
        }
        std::string msg(buffer, ret);
        if (msg == QUIT_CMD) {
            running = false;
        }
        TintinReporter::get_instance().info("fd=" + std::to_string(poll_fd.fd) + ": " + msg);
    } else {
        remove_poll_fd(poll_fd.fd, true);
    }
}

void MattDaemon::handle_client_connection(pollfd pollfd) {
    int client_fd = accept(pollfd.fd, NULL, NULL);
    if (client_fd < 0) {
        throw std::invalid_argument(std::string("accept: ") + strerror(errno));
    }
    if (_poll_fds.size() >= MAX_CLIENT + 1) {
        close(client_fd);
    }
    add_poll_fd({client_fd, POLLIN, 0});
}

void MattDaemon::add_poll_fd(pollfd fd) {
    const auto it =
        std::find_if(_poll_fds.begin(), _poll_fds.end(),
                     [fd](pollfd poll_fd) { return fd.fd == poll_fd.fd; });
    if (it != _poll_fds.end()) {
        TintinReporter::get_instance().warn("add_poll_fd: fd=" + std::to_string(fd.fd) +
                                    " already in _poll_fds");
    }
    TintinReporter::get_instance().info("Add fd=" + std::to_string(fd.fd) +
                                " to poll_fds");
    _poll_fds.emplace_back(fd);
}

void MattDaemon::remove_poll_fd(const int fd, bool close_fd) {
    const auto it =
        std::find_if(_poll_fds.begin(), _poll_fds.end(),
                     [fd](pollfd poll_fd) { return fd == poll_fd.fd; });
    if (it == _poll_fds.end()) {
        throw std::invalid_argument("remove_poll_fd(): invalid fd=" +
                                    std::to_string(fd));
    }
    TintinReporter::get_instance().info("Remove fd=" + std::to_string(fd) +
                                " from poll_fds");
    _poll_fds.erase(it);
    if (close_fd == true) {
        close(fd);
    }
}

void MattDaemon::set_sighandler() {
    struct sigaction sa = {};
    static const std::vector<int> signal_vec = {
        SIGHUP,   SIGINT,   SIGQUIT,
        SIGILL,   SIGABRT, SIGFPE,
        SIGKILL, SIGSEGV, SIGPIPE,
        SIGALRM, SIGTERM, SIGUSR1,
        SIGUSR2, SIGCHLD, SIGCONT,
        SIGSTOP, SIGTSTP, SIGTTIN,
        SIGTTOU
    };

    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    for (int sig : signal_vec) {
        if (sigaction(sig, &sa, nullptr) == -1) {
            throw std::invalid_argument(std::string("sigaction: ") + strerror(errno));
        }
    }
}

static void signal_handler(int sig) {
    TintinReporter::get_instance().info("Received signal " + std::to_string(sig));
}