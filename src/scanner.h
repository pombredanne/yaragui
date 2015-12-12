#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "scanner_rule.h"
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <yara/types.h>
#include <yara/compiler.h>

class Scanner
{
public:

  ~Scanner();
  Scanner(boost::asio::io_service& caller);

  struct CompileResult
  {
    typedef boost::shared_ptr<CompileResult> Ref;
    YR_RULES* rules; /* do not access this from anywhere but the Scanner thread */
    std::string file;
    std::string ns;
    std::string error;
    std::string compilerMessages;
  };

  void compile(const std::string& file, const std::string& ns, boost::function<void (CompileResult::Ref result)> callback);

private:

  void threadCompile(const std::string& file, const std::string& ns, boost::function<void (CompileResult::Ref result)> callback);

  void thread();

  static void yaraCompilerCallback(int errorLevel, const char* fileName, int lineNumber, const char* message, void* userData);

  std::string yaraErrorToString(const int code) const;

  boost::asio::io_service& m_caller; /* to post results back to the main thread */

  boost::asio::io_service m_io;
  boost::shared_ptr<boost::thread> m_thread;

  int m_yaraInitStatus;

};

#endif // __SCANNER_H__
