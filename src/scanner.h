#ifndef __SCANNER_H__
#define __SCANNER_H__

/* this is the interface to the YARA engine */
/* all access to the YARA API happens in a dedicated thread */
/* the calling thread requests a work operation on the YARA thread using boost::asio */

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
    std::string file;
    std::string ns;
    std::string error;
    std::string compilerMessages;
    YR_RULES* rules; /* do not access this from anywhere but the Scanner thread */
    int ruleCount;
  };

  struct LoadResult
  {
    typedef boost::shared_ptr<LoadResult> Ref;
    YR_RULES* rules;
    std::string error;
  };

  typedef boost::function<void (const std::string& hash)> RulesHashCallback;
  typedef boost::function<void (CompileResult::Ref result)> RulesCompileCallback;
  typedef boost::function<void (const std::string& error)> RulesSaveCallback;
  typedef boost::function<void (LoadResult::Ref result)> RulesLoadCallback;
  typedef boost::function<void ()> RulesDestroyCallback;
  typedef boost::function<void (const ScannerRule::Ref rule)> ScanResultCallback;
  typedef boost::function<void (const std::string& error)> ScanCompleteCallback;

  void rulesHash(const std::string& file, RulesHashCallback callback);
  void rulesCompile(const std::string& file, const std::string& ns, RulesCompileCallback callback);
  void rulesSave(YR_RULES* rules, const std::string& file, RulesSaveCallback callback);
  void rulesLoad(const std::string& file, RulesLoadCallback callback);
  void rulesDestroy(YR_RULES* rules, RulesDestroyCallback callback);
  void scanStart(YR_RULES* rules, const std::string& file, int timeout, ScanResultCallback resultCallback, ScanCompleteCallback completeCallback);
  void scanStop();

private:

  void threadRulesHash(const std::string& file, RulesHashCallback callback);
  void threadRulesCompile(const std::string& file, const std::string& ns, RulesCompileCallback callback);
  void threadRulesSave(YR_RULES* rules, const std::string& file, RulesSaveCallback callback);
  void threadRulesLoad(const std::string& file, RulesLoadCallback callback);
  void threadRulesDestroy(YR_RULES* rules, RulesDestroyCallback callback);
  void threadScanStart(YR_RULES* rules, const std::string& file, int timeout, ScanResultCallback resultCallback, ScanCompleteCallback completeCallback);
  void threadScanStop();
  void thread();

  static int yaraScanCallback(int message, void* messageData, void* userData);
  static void yaraCompilerCallback(int errorLevel, const char* fileName, int lineNumber, const char* message, void* userData);
  static std::string yaraErrorToString(const int code);

  boost::asio::io_service& m_caller; /* to post results back to the main thread */

  boost::asio::io_service m_io;
  boost::shared_ptr<boost::thread> m_thread;

  int m_yaraInitStatus;

  ScanResultCallback m_scanResultCallback;
  boost::atomic<bool> m_scanRunning;
  boost::atomic<bool> m_scanAborted;

};

#endif // __SCANNER_H__
