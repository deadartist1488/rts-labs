// Пункт 1: пример с cppreference, переписанный со своим числом.
#include <coroutine>
#include <iostream>
#include "common.h"

struct promise;

struct coroutine : std::coroutine_handle<promise>
{
    using promise_type = ::promise;
};

struct promise
{
    coroutine get_return_object() { return { coroutine::from_promise(*this) }; }
    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
};

int main()
{
    setup_console();
    std::cout << "Лаб.6, пункт 1 (" << BUILD_CONFIG << ")\n";

    // Корутина создаётся приостановленной (initial_suspend -> suspend_always),
    // поэтому до resume() ничего не печатается.
    coroutine h = [](int i) -> coroutine
    {
        std::cout << "Моё число: " << i << std::endl;
        co_return;
    }(2026);

    std::cout << "Корутина создана, но ещё не выполнялась. done() = " << std::boolalpha << h.done() << "\n";
    h.resume();      // выполняем тело до co_return
    std::cout << "После resume(): done() = " << h.done() << "\n";
    h.destroy();     // final_suspend -> suspend_always, кадр уничтожаем сами

    system("pause");
    return 0;
}
