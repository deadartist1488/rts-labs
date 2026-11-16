// Пункт 2: прогресс-бар на корутине; шкала заполняется именем, а не '='.
#include <coroutine>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include "common.h"

#ifndef STUDENT_NAME
#define STUDENT_NAME "Максим"
#endif

struct promise_type
{
    int current_value = 0;

    auto get_return_object()
    {
        return std::coroutine_handle<promise_type>::from_promise(*this);
    }

    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}

    std::suspend_always yield_value(int value)
    {
        current_value = value;
        return {};
    }
};

struct task
{
    std::coroutine_handle<promise_type> handle;

    task(std::coroutine_handle<promise_type> h) : handle(h) {}
    task(const task&) = delete;
    task& operator=(const task&) = delete;
    ~task() { if (handle) handle.destroy(); }

    void resume() { handle.resume(); }
    bool done() const { return handle.done(); }
    int get_value() const { return handle.promise().current_value; }
};

namespace std
{
    template<>
    struct coroutine_traits<task, int>
    {
        using promise_type = ::promise_type;
    };
}

// Корутинная функция: «работает» steps шагов и после каждого отдаёт прогресс.
task long_computation(int steps)
{
    for (int i = 1; i <= steps; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        co_yield i; // отдаём прогресс
    }
    co_return;
}

// Разбиваем UTF-8 строку на символы (кириллица занимает 2 байта),
// чтобы шкала считалась в символах, а не в байтах.
std::vector<std::string> utf8_chars(const std::string& s)
{
    std::vector<std::string> out;
    for (size_t i = 0; i < s.size();)
    {
        unsigned char c = static_cast<unsigned char>(s[i]);
        size_t len = c < 0x80 ? 1 : (c >> 5) == 0x6 ? 2 : (c >> 4) == 0xE ? 3 : 4;
        out.push_back(s.substr(i, len));
        i += len;
    }
    return out;
}

// Функция отображения прогресса: заполненная часть шкалы — это имя,
// повторённое по кругу; на границе — '>'.
void print_progress(int current, int total)
{
    static const std::vector<std::string> name = utf8_chars(STUDENT_NAME " ");
    const int bar_width = 50;

    float percent = static_cast<float>(current) / total * 100.f;
    size_t pos = static_cast<size_t>(percent / 100.f * bar_width);

    std::string bar;
    for (size_t i = 0; i < static_cast<size_t>(bar_width); ++i)
    {
        if (i < pos)
            bar += (i + 1 == pos && pos < static_cast<size_t>(bar_width)) ? std::string(">")
                                                                          : name[i % name.size()];
        else
            bar += ' ';
    }

    std::cout << "\r[" << bar << "] " << std::fixed << std::setprecision(1) << percent << "%";
    std::flush(std::cout);
}

int main()
{
    setup_console();
    std::cout << "Лаб.6, пункт 2 (" << BUILD_CONFIG << ")\n";

    constexpr int TOTAL_STEPS = 100;
    auto coro = long_computation(TOTAL_STEPS);

    while (!coro.done())
    {
        coro.resume();
        if (coro.done()) break;            // после co_return значения уже нет
        int progress = coro.get_value();
        print_progress(progress, TOTAL_STEPS);
        std::this_thread::yield();
    }
    std::cout << "\nDONE" << std::endl;

    system("pause");
    return 0;
}
