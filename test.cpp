#include <iostream>
#include <string>
#include <functional>

class Logger {
public:
  void log(std::string message) { std::cout << "log: " << message << std::endl; }
  void clog(const char* message) { std::cout << "log: " << message << std::endl; }
};

template<typename CallbackFunction>
struct CallbackInfo {
  void* instance;
  CallbackFunction* DoAction;
};

template <class Callable, class... Args>
static auto CallFunction(void* function, Args... args) -> decltype((*reinterpret_cast<Callable*>(function))(args...))
{
  return (*reinterpret_cast<Callable*>(function))(args...);
}

template <class R, class... Args>
static R CallStdFunction(void* function, Args... args)
{
  return (*reinterpret_cast<std::function<R(Args...)>*>(function))(args...);
}

template <class R, class... Args>
CallbackInfo<R(void*, Args...)> CreateCallback(std::function<R(Args...)> f) {
  return CallbackInfo<R(void*, Args...)>{reinterpret_cast<void*>(&f), CallStdFunction<R, Args...>};
}

#define TO_CALLBACK(f, CallbackType) \
  CallbackInfo<CallbackType>{reinterpret_cast<void*>(&f), CallFunction<decltype(f)>}

#define RUN_CALLBACK(callback, args...) \
  callback.DoAction(callback.instance, args)

typedef void LogCallback(void* log_function, const char* message);

void ApplyFunction(CallbackInfo<LogCallback> logger) {
  RUN_CALLBACK(logger, "Hello, world");
}

void MyLog(const char* message) {
  std::cout << "MyLog: " << message << std::endl;
}

int main() {
  using namespace std::placeholders;

  Logger logger_;
  auto f = std::bind(&Logger::clog, &logger_, _1);

  ApplyFunction(TO_CALLBACK(f, LogCallback));
  ApplyFunction(TO_CALLBACK(MyLog, LogCallback));

  std::function<void(const char*)> clog_binded = f;
  ApplyFunction(CreateCallback(clog_binded));

  std::function<void(const char*)> ff = [](const char* message) { std::cout << "ff log: " << message << std::endl; };
  ApplyFunction(CreateCallback(ff));

  std::function<void(const char*)> my_log = MyLog;
  ApplyFunction(CreateCallback(my_log));
}

