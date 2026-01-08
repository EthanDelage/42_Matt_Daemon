#include "TintinReporter.hpp"

#include <cstring>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/fcntl.h>
#include <unistd.h>

#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"

#define DEBUG_COLOR COLOR_GREEN
#define INFO_COLOR COLOR_BLUE
#define WARN_COLOR COLOR_YELLOW
#define ERROR_COLOR COLOR_RED

std::unique_ptr<TintinReporter> TintinReporter::_instance;
std::once_flag TintinReporter::_init_flag;

TintinReporter::TintinReporter(const std::string &log_file_path_str) {
  create_log_directories(log_file_path_str);
  _log_fd =
      open(log_file_path_str.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (_log_fd == -1) {
    throw std::runtime_error(std::string("Logger(): ") + strerror(errno));
  }
}

TintinReporter::~TintinReporter() {
  info("Log file closed");
  close(_log_fd);
}

void TintinReporter::init(const std::string &log_file_path_str) {
  std::call_once(_init_flag, [&]() {
    _instance =
        std::unique_ptr<TintinReporter>(new TintinReporter(log_file_path_str));
  });
  get_instance().info("Log file `" + log_file_path_str + "` created");
}

TintinReporter &TintinReporter::get_instance() {
  if (!_instance) {
    throw std::runtime_error(
        "Logger not initialized. Call Logger::init() first.");
  }
  return *_instance;
}

void TintinReporter::log(Level level, const std::string &message) const {
  std::stringstream log_line_ss;

  const auto now = std::chrono::system_clock::now();
  const auto in_time_t = std::chrono::system_clock::to_time_t(now);
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) %
                  1000;

  log_line_ss << '['
              << std::put_time(std::localtime(&in_time_t), "%d/%m/%Y-%H:%M:%S")
              << '.' << std::setw(3) << std::setfill('0') << ms.count() << "] "
              << '[' << level << "] "
              << "[pid=" << getpid() << "] " << message;
  if (message[message.length() - 1] != '\n') {
    log_line_ss << std::endl;
  }

  if (level == Level::Error) {
    std::cerr << log_level_to_color(level) << log_line_ss.str() << COLOR_RESET
              << std::flush;
  } else {
    std::cout << log_level_to_color(level) << log_line_ss.str() << COLOR_RESET
              << std::flush;
  }
  if (Socket::write(_log_fd, log_line_ss.str()) == -1) {
    throw std::runtime_error("Logger::log(): Failed to write to file");
  }
}

void TintinReporter::debug(const std::string &message) const {
  log(Level::Debug, message);
}

void TintinReporter::info(const std::string &message) const {
  log(Level::Info, message);
}

void TintinReporter::warn(const std::string &message) const {
  log(Level::Warning, message);
}

void TintinReporter::error(const std::string &message) const {
  log(Level::Error, message);
}

std::string TintinReporter::log_level_to_color(Level level) {
  switch (level) {
  case TintinReporter::Level::Debug:
    return DEBUG_COLOR;
  case TintinReporter::Level::Info:
    return INFO_COLOR;
  case TintinReporter::Level::Warning:
    return WARN_COLOR;
  case TintinReporter::Level::Error:
    return ERROR_COLOR;
  default:
    return "";
  }
}

void TintinReporter::create_log_directories(
    const std::string &log_file_path_str) {
  std::filesystem::path log_file_path(log_file_path_str);

  try {
    std::filesystem::create_directories(log_file_path.parent_path());
  } catch (const std::filesystem::filesystem_error &e) {
    throw std::runtime_error(
        std::string(
            "TintinReporter::create_log_directories(): Filesystem error: ") +
        e.what());
  } catch (const std::exception &e) {
    throw std::runtime_error(
        std::string("TintinReporter::create_log_directories(): ") + e.what());
  }
}

std::ostream &operator<<(std::ostream &os, const TintinReporter::Level &level) {
  switch (level) {
  case TintinReporter::Level::Debug:
    os << "DEBUG";
    break;
  case TintinReporter::Level::Info:
    os << "INFO";
    break;
  case TintinReporter::Level::Warning:
    os << "WARNING";
    break;
  case TintinReporter::Level::Error:
    os << "ERROR";
    break;
  default:
    os << "UNKNOWN";
    break;
  }
  return os;
}
