#pragma once

#ifdef ENABLE_CRASH_REPORTS
  //#include <client/windows/handler/exception_handler.h>
#endif

namespace Core {
  class CrashReport : ICrashReport {
  public:
    CrashReport(std::string path, std::string pipe = "") {};
    
  private:
#ifdef ENABLE_CRASH_REPORTS
    //google_breakpad::ExceptionHandler _exception_handler;
#endif
  };
}