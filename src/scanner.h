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

  typedef boost::function<void (const ScannerRule::Ref rule)> ScanResultCallback;
  typedef boost::function<void (std::string error)> ScanCompleteCallback;

  void compile(const std::string& file, const std::string& ns, boost::function<void (CompileResult::Ref result)> callback);
  bool scanStart(YR_RULES* rules, const std::string& file, int timeout, ScanResultCallback resultCallback, ScanCompleteCallback completeCallback);
  void scanStop();

private:

  void threadCompile(const std::string& file, const std::string& ns, boost::function<void (CompileResult::Ref result)> callback);
  void threadScanStart(YR_RULES* rules, const std::string& file, int timeout, ScanResultCallback resultCallback, ScanCompleteCallback completeCallback);
  void threadScanStop();
  void thread();

  static int yaraScanCallback(int message, void* messageData, void* userData);
  static void yaraCompilerCallback(int errorLevel, const char* fileName, int lineNumber, const char* message, void* userData);
  std::string yaraErrorToString(const int code) const;

  boost::asio::io_service& m_caller; /* to post results back to the main thread */

  boost::asio::io_service m_io;
  boost::shared_ptr<boost::thread> m_thread;

  int m_yaraInitStatus;

  ScanResultCallback m_scanResultCallback;
  boost::atomic<bool> m_scanRunning;
  boost::atomic<bool> m_scanAborted;

};

#endif // __SCANNER_H__
