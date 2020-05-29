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

#define RUN_CALLBACK(callback) callback.run(callback.original)
#define RUN_CALLBACK_WITH_PARAMETERS(callback, ...) callback.run(callback.original, __VA_ARGS__)

typedef void LogCallback(void*, const char* message);

void ApplyFunction(CallbackInfo<LogCallback> log) {
  RUN_CALLBACK_WITH_PARAMETERS(log, "Hello, world");
}

void MyLog(const char* message) {
  std::cout << "MyLog: " << message << std::endl;
}

class Logger {
public:
  void log(std::string message) { std::cout << "log: " << message << std::endl; }
  void clog(const char* message) { std::cout << "clog: " << message << std::endl; }

  void log_int(int number) { std::cout << "log: number = " << number << std::endl; }
  int get_number() { return number_; }

private:
  int number_ = 42;
};

typedef void LogIntCallback(void*, int number);
typedef int GetIntCallback(void*);

void ApplyFunction(CallbackInfo<LogIntCallback> log_int, CallbackInfo<GetIntCallback> get_int) {
  RUN_CALLBACK_WITH_PARAMETERS(log_int, RUN_CALLBACK(get_int));
}

int main() {
  using namespace std::placeholders;

  Logger logger_;

  auto m = std::mem_fn(&Logger::log);
  auto mm = std::bind(m, &logger_, _1);
  ApplyFunction(CreateCallback<LogCallback>(mm));

  auto f = std::bind(&Logger::log, &logger_, _1);
  ApplyFunction(CreateCallback<LogCallback>(f));

  ApplyFunction(CreateCallback<LogCallback>(std::bind(&Logger::clog, &logger_, _1)));

  std::function<void(const char*)> ff = [](const char* message) { std::cout << "ff log: " << message << std::endl; };
  ApplyFunction(CreateCallback<LogCallback>([](const char *message) {
    std::cout << "ff log: " << message << std::endl;
  }));

  ApplyFunction(CreateCallback<LogCallback>(&MyLog));

  ApplyFunction(
    CreateCallback<LogIntCallback>(std::bind(&Logger::log_int, &logger_, _1)),
    CreateCallback<GetIntCallback>(std::bind(&Logger::get_number, &logger_))
  );
}

