#include <iostream>
#include <ostream>
#include <unistd.h>

#include "Daemon.hpp"
#include "MattDaemon.hpp"
#include "TintinReporter.hpp"

int main() {
  if (geteuid() != 0) {
    std::cerr << "Daemon must start as root !" << std::endl;
    return EXIT_FAILURE;
  }
  try {
    TintinReporter::init(LOGFILE_PATH);
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  try {
    Daemon daemon;
    if (daemon.start(DAEMON_USER) == -1) {
      return EXIT_FAILURE;
    }
    MattDaemon matt_daemon;
    matt_daemon.loop();
  } catch (std::exception &e) {
    TintinReporter::get_instance().error(e.what());
    TintinReporter::get_instance().info("Exiting MattDaemon unexpectedly");
    return EXIT_FAILURE;
  }
  TintinReporter::get_instance().info("Exiting MattDaemon");
  return EXIT_SUCCESS;
}
