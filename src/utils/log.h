//
// Created by arrayJY on 2023/01/16.
//

#ifndef SPARROWENGINE_LOG_H
#define SPARROWENGINE_LOG_H

#include <format>

namespace Sparrow {

#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define WHITE "\x1b[37m"

#define LOG_BASE(outputStream, color, level, str)                    \
  {                                                                  \
    (outputStream) << (color) << (level) << ": " << __FILE__ << ": " \
                   << "line " << __LINE__ << ": " << __FUNCTION__    \
                   << "(): " << RESET << (str) << "\n";              \
  }

#define FORMAT_STR(fmt, ...) (std::format((fmt), __VA_ARGS__))

#define LOG(str) LOG_BASE(std::cout, WHITE, "LOG", str)

#define LOG_WARN(str) LOG_BASE(std::cout, YELLOW, "WARNING", str)

#define LOG_ERROR(str) LOG_BASE(std::cerr, RED, "ERROR", str)

#define LOG_FMT(fmt, ...) LOG(FORMAT_STR(fmt, __VA_ARGS__))

#define LOG_WARN_FMT(fmt, ...) LOG_WARN(FORMAT_STR(fmt, __VA_ARGS__))

#define LOG_ERROR_FMT(fmt, ...) LOG_ERROR(FORMAT_STR(fmt, __VA_ARGS__))
};  // namespace Sparrow

#endif