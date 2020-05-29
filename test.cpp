#include <iostream>
#include <string>
#include <functional>
#include <type_traits>
#include <utility>

template<typename CallbackFunction>
struct CallbackInfo {
  void* original;
  CallbackFunction* run;
};

template <class F, class... Args>
decltype(auto) Invoke(void *f, Args... args) {
  return std::invoke(*static_cast<typename std::decay<F>::type*>(f), args...);
}

template <class CallbackType, class F, class... Args>
CallbackInfo<CallbackType> CreateCallback(F&& f) {
  return {static_cast<void *>(std::addressof(f)), Invoke<F, Args...>};
}

#define RUN_CALLBACK(callback, args...) \
  callback.run(callback.original, args)

typedef void LogCallback(void*, const char* message);

void ApplyFunction(CallbackInfo<LogCallback> logger) {
  RUN_CALLBACK(logger, "Hello, world");
}

void MyLog(const char* message) {
  std::cout << "MyLog: " << message << std::endl;
}

class Logger {
public:
  void log(std::string message) { std::cout << "log: " << message << std::endl; }
  void clog(const char* message) { std::cout << "clog: " << message << std::endl; }
};

int main() {
  using namespace std::placeholders;

  Logger logger_;

  auto f = std::bind(&Logger::log, &logger_, _1);
  ApplyFunction(CreateCallback<LogCallback>(f));

  ApplyFunction(CreateCallback<LogCallback>(std::bind(&Logger::clog, &logger_, _1)));

  std::function<void(const char*)> ff = [](const char* message) { std::cout << "ff log: " << message << std::endl; };
  ApplyFunction(CreateCallback<LogCallback>([](const char *message) {
    std::cout << "ff log: " << message << std::endl;
  }));

  ApplyFunction(CreateCallback<LogCallback>(&MyLog));
}

