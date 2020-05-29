# Playing callback game
## Usage examples:

```C++
int main() {
  using namespace std::placeholders;

  ApplyFunction(CreateCallback<LogCallback>(&MyLog));

  Logger logger_;

  auto f = std::bind(&Logger::log, &logger_, _1);
  ApplyFunction(CreateCallback<LogCallback>(f));

  ApplyFunction(CreateCallback<LogCallback>(std::bind(&Logger::clog, &logger_, _1)));

  std::function<void(const char*)> ff = [](const char* message) { std::cout << "ff log: " << message << std::endl; };
  ApplyFunction(CreateCallback<LogCallback>([](const char *message) {
    std::cout << "ff log: " << message << std::endl;
  }));

  ApplyFunction(
    CreateCallback<LogIntCallback>(std::bind(&Logger::log_int, &logger_, _1)),
    CreateCallback<GetIntCallback>(std::bind(&Logger::get_number, &logger_))
  );

  auto m = std::mem_fn(&Logger::log);
  auto mm = std::bind(m, &logger_, _1);
  ApplyFunction(CreateCallback<LogCallback>(mm));
}
```
