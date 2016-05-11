// Copyright 2016 Chirstopher Torres (Raven), L3nn0x
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// #undef UNICODE
//
#include <sstream>
#include "logconsole.h"
#ifdef _WIN32
#include <spdlog/sinks/wincolor_sink.h>
#endif

std::ostream& Core::Color::operator<<(std::ostream& os,
                                      Core::Color::CodeFG code) {
#ifndef _WIN32
  return os << "\033[1;" << static_cast<int>(code) << "m";
#else
  return os;
#endif
}

std::ostream& Core::Color::operator<<(std::ostream& os,
                                      Core::Color::CodeBG code) {
#ifndef _WIN32
  return os << "\033[" << static_cast<int>(code) << "m";
#else
  return os;
#endif
}

namespace Core {

spdlog::level::level_enum CLog::level_ = spdlog::level::notice;

void CLog::SetLevel(spdlog::level::level_enum _level) {
  level_ = _level;

  std::ostringstream format;
  format << "[%H:%M:%S.%e %z] [%L]";

  if (level_ <= spdlog::level::debug) format << " [thread %t]";
  format << " [%n]" << " %v ";
  spdlog::set_pattern(format.str());
}

std::weak_ptr<spdlog::logger> CLog::GetLogger(
    log_type _type) {
  std::weak_ptr<spdlog::logger> logger;
  try
  {
    switch (_type) {
      case log_type::NETWORK:
        logger = spdlog::get("net");
        break;

      case log_type::DATABASE:
        logger = spdlog::get("db");
        break;

      case log_type::GENERAL:
      default:
        logger = spdlog::get("server");
        break;
    }

    if (logger.expired()) {
      std::ostringstream format;
      format << "[%H:%M:%S.%e.%f %z] [%L]";

      if (level_ <= spdlog::level::debug) format << " [thread %t]";
      format << " [%n]" << " %v ";

      size_t q_size = 1048576;
      spdlog::set_async_mode(q_size, spdlog::async_overflow_policy::discard_log_msg,
                             nullptr, std::chrono::seconds(30));

      std::string path, name;

      switch (_type) {
        case log_type::NETWORK: {
          path = "logs/network";
          name = "net";
          break;
        }
        case log_type::DATABASE: {
          path = "logs/database";
          name = "db";
          break;
        }
        case log_type::GENERAL:
        default: {
          path = "logs/server";
          name = "server";
          break;
        }
      }

      std::vector<spdlog::sink_ptr> net_sink;
//      auto console_out = spdlog::sinks::stdout_sink_mt::instance();
#ifdef _WIN32
      auto console_sink = std::make_shared<spdlog::sinks::wincolor_sink_mt>(std::cout, true);
#else
      auto console_out = spdlog::sinks::stdout_sink_mt::instance();
      auto console_sink = std::make_shared<spdlog::sinks::ansicolor_sink>(console_out);
#endif
//       auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
//           path.c_str(), "txt", 23, 59);
      net_sink.push_back(console_sink);
//      net_sink.push_back(daily_sink);

      auto net_logger = std::make_shared<spdlog::logger>(
          name.c_str(), begin(net_sink), end(net_sink));
      net_logger->set_level(level_);
      net_logger->set_pattern(format.str());

      spdlog::register_logger(net_logger);

      return net_logger;
    }
  } catch (const spdlog::spdlog_ex& ex) {
    std::cout << "Log failed: " << ex.what() << std::endl;
  }
  return logger;
}
}
