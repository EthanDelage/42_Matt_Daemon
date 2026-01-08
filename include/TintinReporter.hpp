#ifndef TINTIN_REPORTER_HPP
#define TINTIN_REPORTER_HPP

#include <memory>
#include <mutex>
#include <string>

#include "Socket.hpp"

#define LOGFILE_PATH "/var/log/matt_daemon/matt_daemon.log"
// #define LOGFILE_PATH "./matt_daemon.log"

class TintinReporter {
public:
  enum class Level { Debug, Info, Warning, Error };

  TintinReporter(const TintinReporter &) = delete;
  TintinReporter &operator=(const TintinReporter &) = delete;
  ~TintinReporter();

  static void init(const std::string &log_file_path_str);
  static TintinReporter &get_instance();

  void log(Level level, const std::string &message) const;
  void debug(const std::string &message) const;
  void info(const std::string &message) const;
  void warn(const std::string &message) const;
  void error(const std::string &message) const;

private:
  explicit TintinReporter(const std::string &log_file_path_str);

  static std::string log_level_to_color(Level level);
  static void create_log_directories(const std::string &log_file_path_str);

  static std::once_flag _init_flag;
  static std::unique_ptr<TintinReporter> _instance;
  int _log_fd;
};

std::ostream &operator<<(std::ostream &os, const TintinReporter::Level &level);

#endif // TINTIN_REPORTER_HPP
