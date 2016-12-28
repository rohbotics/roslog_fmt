#ifndef FMT_LOGGER_H
#define FMT_LOGGER_H

#include <fmt/format.h>
#include <ros2log/logger.hpp>

class FmtLogger : public Logger {
 public:
  using Logger::Logger;

  template <typename... Args>
  void log(Log_Levels level, const char *file, const char *function, int line, const char *fmt,
             Args &&... args) const {
    auto md = MetaData{std::chrono::system_clock::now(), file, function, line};
    auto data = fmt::format(fmt, std::forward<Args>(args)...);
    output(level, md, data.c_str());
  }
};

#endif