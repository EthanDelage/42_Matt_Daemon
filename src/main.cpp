#include <iostream>
#include <ostream>

#include "Daemon.hpp"
#include "MattDaemon.hpp"
#include "TintinReporter.hpp"

int main() {
  try {
    TintinReporter::init(LOGFILE_PATH);
    MattDaemon matt_daemon;
    Daemon daemon;
    if (daemon.start(DAEMON_USER) == -1) {
      return EXIT_FAILURE;
    }
    matt_daemon.loop();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
