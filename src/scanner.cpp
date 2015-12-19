#include "scanner.h"
#include <yara.h>
#include <sstream>
#include <fstream>
#include <QtCore/QCryptographicHash>

Scanner::~Scanner()
{
  m_io.stop();
  m_thread->join();
}

Scanner::Scanner(boost::asio::io_service& caller) : m_caller(caller), m_scanRunning(false), m_scanAborted(false)
{
  m_thread = boost::make_shared<boost::thread>(boost::bind(&Scanner::thread, this));
}

void Scanner::rulesHash(const std::string& file, RulesHashCallback callback)
{
  m_io.post(boost::bind(&Scanner::threadRulesHash, this, file, callback));
}

void Scanner::rulesCompile(const std::string& file, const std::string& ns, RulesCompileCallback callback)
{
  m_io.post(boost::bind(&Scanner::threadRulesCompile, this, file, ns, callback));
}

void Scanner::rulesSave(YR_RULES* rules, const std::string& file, RulesSaveCallback callback)
{
  m_io.post(boost::bind(&Scanner::threadRulesSave, this, rules, file, callback));
}

void Scanner::rulesLoad(const std::string& file, RulesLoadCallback callback)
{
  m_io.post(boost::bind(&Scanner::threadRulesLoad, this, file, callback));
}

void Scanner::rulesDestroy(YR_RULES* rules, RulesDestroyCallback callback)
{
  m_io.post(boost::bind(&Scanner::threadRulesDestroy, this, rules, callback));
}

void Scanner::scanStart(YR_RULES* rules, const std::string& file, int timeout, ScanResultCallback resultCallback, ScanCompleteCallback completeCallback)
{
  if (!m_scanRunning) {
    m_io.post(boost::bind(&Scanner::threadScanStart, this, rules, file, timeout, resultCallback, completeCallback));
  }
}

void Scanner::scanStop()
{
  m_scanAborted = true;
}

void Scanner::threadRulesHash(const std::string& file, RulesHashCallback callback)
{
  std::ifstream input(file.c_str(), std::ifstream::binary);
  if (!input.is_open()) {
    m_io.post(boost::bind(callback, std::string()));
    return;
  }

  QCryptographicHash hash(QCryptographicHash::Md5);
  std::vector<char> buffer(50 * 1024);
  while (!input.eof()) {
    input.read(&buffer[0], buffer.size());
    if (input.bad()) {
      m_io.post(boost::bind(callback, std::string()));
      return;
    }
    hash.addData(&buffer[0], input.gcount());
  }

  QByteArray hashBytes = hash.result().toHex();
  std::string hashString(hashBytes.constData(), hashBytes.length());
  m_io.post(boost::bind(callback, hashString));
}

void Scanner::threadRulesCompile(const std::string& file, const std::string& ns, RulesCompileCallback callback)
{
  CompileResult::Ref result = boost::make_shared<CompileResult>();
  result->rules = 0;
  result->ruleCount = 0;
  result->file = file;
  result->ns = ns;

  if (m_yaraInitStatus != ERROR_SUCCESS) {
    result->error = yaraErrorToString(m_yaraInitStatus);
    m_caller.post(boost::bind(callback, result));
    return;
  }

  FILE* fd = fopen(file.c_str(), "r");
  if (!fd) {
    result->error = "Failed to compile rules: Error loading file: \"" + file + "\"";
    m_caller.post(boost::bind(callback, result));
    return;
  }

  YR_COMPILER* compiler = 0;
  int createResult = yr_compiler_create(&compiler);
  if (createResult != ERROR_SUCCESS) {
    result->error = "Failed to compile rules: " + yaraErrorToString(createResult);
    fclose(fd);
    m_caller.post(boost::bind(callback, result));
    return;
  }

  yr_compiler_set_callback(compiler, yaraCompilerCallback, &result->compilerMessages);
  int errorCount = yr_compiler_add_file(compiler, fd, ns.c_str(), file.c_str());
  fclose(fd);

  if (errorCount) {
    result->error = "Failed to compile rules: Rules contain errors.";
    yr_compiler_destroy(compiler);
    m_caller.post(boost::bind(callback, result));
    return;
  }

  int rulesResult = yr_compiler_get_rules(compiler, &result->rules);
  yr_compiler_destroy(compiler);

  if (rulesResult != ERROR_SUCCESS) {
    result->error = "Failed to compile rules: " + yaraErrorToString(createResult);
    m_caller.post(boost::bind(callback, result));
    return;
  }

  YR_RULE* rule = 0;
  yr_rules_foreach(result->rules, rule) {
    result->ruleCount++;
  }

  m_caller.post(boost::bind(callback, result)); /* success */
}

void Scanner::threadRulesSave(YR_RULES* rules, const std::string& file, RulesSaveCallback callback)
{
  if (m_yaraInitStatus != ERROR_SUCCESS) {
    m_caller.post(boost::bind(callback, yaraErrorToString(m_yaraInitStatus)));
    return;
  }

  int saveResult = yr_rules_save(rules, file.c_str());
  if (saveResult != ERROR_SUCCESS) {
    m_caller.post(boost::bind(callback, yaraErrorToString(saveResult)));
    return;
  }

  m_caller.post(boost::bind(callback, std::string())); /* success */
}

void Scanner::threadRulesLoad(const std::string& file, RulesLoadCallback callback)
{
  LoadResult::Ref result = boost::make_shared<LoadResult>();
  result->rules = 0;

  if (m_yaraInitStatus != ERROR_SUCCESS) {
    result->error = yaraErrorToString(m_yaraInitStatus);
    m_caller.post(boost::bind(callback, result));
    return;
  }

  int loadResult = yr_rules_load(file.c_str(), &result->rules);
  if (loadResult != ERROR_SUCCESS) {
    result->rules = 0;
    result->error = yaraErrorToString(loadResult);
    m_caller.post(boost::bind(callback, result));
  }

  m_caller.post(boost::bind(callback, result)); /* success */
}

void Scanner::threadRulesDestroy(YR_RULES* rules, RulesDestroyCallback callback)
{
  yr_rules_destroy(rules);
  m_caller.post(callback);
}

void Scanner::threadScanStart(YR_RULES* rules, const std::string& file, int timeout, ScanResultCallback resultCallback, ScanCompleteCallback completeCallback)
{
  if (m_yaraInitStatus != ERROR_SUCCESS) {
    m_caller.post(boost::bind(completeCallback, yaraErrorToString(m_yaraInitStatus)));
    return;
  }

  m_scanResultCallback = resultCallback;
  m_scanRunning = true;
  m_scanAborted = false;

  int scanResult = yr_rules_scan_file(rules, file.c_str(), 0, yaraScanCallback, this, timeout);

  std::string error;
  if (scanResult != ERROR_SUCCESS) {
    error = yaraErrorToString(scanResult);
  }

  m_caller.post(boost::bind(completeCallback, error));

  m_scanRunning = false;
}

void Scanner::thread()
{
  /* initialize YARA for this thread */

  m_yaraInitStatus = yr_initialize();

  boost::asio::io_service::work keepAlive(m_io);
  m_io.run();

  if (m_yaraInitStatus == ERROR_SUCCESS) {
    yr_finalize();
  }
}

int Scanner::yaraScanCallback(int message, void* messageData, void* userData)
{
  Scanner* scanner = (Scanner*)userData;

  if (message == CALLBACK_MSG_RULE_NOT_MATCHING) {
    /* this is just to indicate scan progress */
    scanner->m_caller.post(boost::bind(scanner->m_scanResultCallback, ScannerRule::Ref()));
  }

  if (message == CALLBACK_MSG_RULE_MATCHING) {
    ScannerRule::Ref rule = boost::make_shared<ScannerRule>((YR_RULE*)messageData);
    scanner->m_caller.post(boost::bind(scanner->m_scanResultCallback, rule));
  }

  if (scanner->m_scanAborted) {
    return CALLBACK_ABORT;
  }

  return CALLBACK_CONTINUE;
}

void Scanner::yaraCompilerCallback(int errorLevel, const char* fileName, int lineNumber, const char* message, void* userData)
{
  std::string& result = *(std::string*)&userData;
  std::stringstream ss;
  ss << fileName << "(" << lineNumber << "): error: " << message << std::endl;
  result += ss.str();
}

std::string Scanner::yaraErrorToString(const int code)
{
  switch (code) {
  case ERROR_SUCCESS:
    return "Everything went fine.";
  case ERROR_INSUFICIENT_MEMORY:
    return "Insuficient memory to complete the operation.";
  case ERROR_COULD_NOT_OPEN_FILE:
    return "File could not be opened.";
  case ERROR_COULD_NOT_MAP_FILE:
    return "File could not be mapped into memory.";
  case ERROR_INVALID_FILE:
    return "File is not a valid rules file.";
  case ERROR_CORRUPT_FILE:
    return "Rules file is corrupt.";
  case ERROR_UNSUPPORTED_FILE_VERSION:
    return "File was generated by a different version and can’t be loaded.";
  case ERROR_TOO_MANY_SCAN_THREADS:
    return "Too many threads trying to scan simultaneously";
  case ERROR_SCAN_TIMEOUT:
    return "Scan timed out.";
  case ERROR_CALLBACK_ERROR:
    return "Callback returned an error.";
  case ERROR_TOO_MANY_MATCHES:
    return "Too many matches for some string in your rules. This usually happens when your rules contains very short or very common strings like 01 02 or FF FF FF FF.";
  default:
    break;
  }
  /* output error code for unknown errors */
  std::stringstream ss;
  ss << "Unknown error (" << code << ").";
  return ss.str();
}
