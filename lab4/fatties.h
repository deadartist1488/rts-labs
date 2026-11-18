#pragma once
// Модель «Три толстяка». Шаблон по типу мьютекса: в лаб.4 — std::mutex,
// в лаб.5 — самодельный мьютекс (интерфейс lock()/unlock()).
#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

enum class Outcome { None, Fired, NotPaid, Quit };

inline const char* outcome_text(Outcome o)
{
    switch (o) {
    case Outcome::Fired:   return "Кука уволили";
    case Outcome::NotPaid: return "Кук не получил зарплату.";
    case Outcome::Quit:    return "Кук уволился сам!";
    default:               return "???";
    }
}

struct Params {
    const char* title;
    Outcome expected;
    int gluttony;           // сколько наггетсов толстяк съедает за раз
    int efficiency_factor;  // сколько наггетсов Кук кладёт на КАЖДУЮ тарелку за раз
};

// Константы условия
constexpr int  START_DISH   = 3000;     // на каждой тарелке в начале
constexpr int  EXPLODE_AT   = 10000;    // съел больше — лопнул
constexpr auto DAY          = std::chrono::seconds(1);   // 1 день = 1 секунда
constexpr int  DAYS_TO_QUIT = 5;
// Время, за которое Кук готовит одну партию (одна итерация его цикла).
// sleep_for спит НЕ МЕНЬШЕ заданного, поэтому за 5 «дней» Кук успеет
// сделать не больше 5 с / 1 мс = 5000 партий — на этом держатся гарантии.
constexpr auto COOK_TIME    = std::chrono::milliseconds(1);

template <class Mutex>
class Table {
public:
    explicit Table(Params p) : p_(p) {
        dish_.fill(START_DISH);
        eaten_.fill(0);
        alive_.fill(true);
        ate_.fill(true);   // новой партии на столе ещё нет — первым ходит Кук
    }

    Outcome run() {
        start_ = std::chrono::steady_clock::now();
        std::thread cook(&Table::cook, this);
        std::thread f1(&Table::fatty, this, 0);
        std::thread f2(&Table::fatty, this, 1);
        std::thread f3(&Table::fatty, this, 2);
        cook.join(); f1.join(); f2.join(); f3.join();
        return outcome_;
    }

    void report() const {
        std::chrono::duration<double> d = end_ - start_;
        std::cout << "  итог через " << d.count() << " дн. (" << batches_ << " партий):\n";
        for (int k = 0; k < 3; ++k)
            std::cout << "    толстяк " << k + 1 << ": съел " << eaten_[k]
                      << ", на тарелке " << dish_[k]
                      << (alive_[k] ? "" : "  — ЛОПНУЛ") << "\n";
        std::cout << "  >>> " << outcome_text(outcome_) << "\n";
    }

private:
    // Вызывается под мьютексом.
    void finish(Outcome o) {
        if (outcome_ == Outcome::None) {
            outcome_ = o;
            end_ = std::chrono::steady_clock::now();
        }
    }

    bool days_passed() const {
        return std::chrono::steady_clock::now() - start_ >= DAYS_TO_QUIT * DAY;
    }

    // Очерёдность задаёт флаг ate_[k] «толстяк k уже съел свою порцию из
    // партии, которая сейчас на столе». Кук кладёт новую партию, только когда
    // все живые толстяки съели (все флаги true), и сбрасывает флаги в false.
    // Толстяк ест, только когда его флаг false, и сразу ставит true.
    // Флаги читаются и меняются только под мьютексом m_.
    void cook() {
        m_.lock();
        while (outcome_ == Outcome::None) {
            if (days_passed()) {
                finish(Outcome::Quit);           // 5 дней прошло — Кук уходит сам
                break;
            }

            bool everyone_ate = true;
            for (int k = 0; k < 3; ++k)
                if (alive_[k] && !ate_[k]) everyone_ate = false;

            if (everyone_ate) {
                for (int k = 0; k < 3; ++k) {          // толстяки ждут на мьютексе,
                    dish_[k] += p_.efficiency_factor;  // пока Кук раскладывает
                    ate_[k] = false;
                }
                ++batches_;                            // только для вывода
                m_.unlock();
                std::this_thread::sleep_for(COOK_TIME); // готовит следующую партию
            } else {
                m_.unlock();
                std::this_thread::yield();
            }
            m_.lock();
        }
        m_.unlock();
    }

    void fatty(int k) {
        m_.lock();
        while (outcome_ == Outcome::None && alive_[k]) {
            if (!ate_[k]) {                           // новая партия на столе
                int take = dish_[k] < p_.gluttony ? dish_[k] : p_.gluttony;
                dish_[k]  -= take;
                eaten_[k] += take;
                ate_[k] = true;

                if (eaten_[k] > EXPLODE_AT) {
                    alive_[k] = false;
                    std::chrono::duration<double> d = std::chrono::steady_clock::now() - start_;
                    std::cout << "    толстяк " << k + 1 << " лопнул на " << d.count() << " дн.\n";
                    if (!alive_[0] && !alive_[1] && !alive_[2])
                        finish(Outcome::NotPaid);
                } else if (dish_[k] == 0) {
                    finish(Outcome::Fired);           // наггетсы закончились
                }
            }
            m_.unlock();
            std::this_thread::yield();
            m_.lock();
        }
        m_.unlock();
    }

    Params p_;
    Mutex m_;
    std::array<int, 3> dish_{}, eaten_{};
    std::array<bool, 3> alive_{};
    std::array<bool, 3> ate_{};     // съел ли толстяк свою порцию из текущей партии
    long long batches_ = 0;         // сколько партий выложил Кук (только для вывода)
    Outcome outcome_ = Outcome::None;
    std::chrono::steady_clock::time_point start_{}, end_{};
};

// Три набора коэффициентов, каждый гарантирует свой исход (обоснование — в отчёте).
inline const std::array<Params, 3> SCENARIOS = { {
    { "Кука уволили",            Outcome::Fired,   100,  50 },
    { "Кук не получил зарплату", Outcome::NotPaid, 100, 100 },
    { "Кук уволился сам",        Outcome::Quit,      1,   1 },
} };

// Прогоняет все три сценария; repeats — сколько раз повторить каждый
// (повторы без подробного вывода). Возвращает число неожиданных исходов.
template <class Mutex>
int run_all(const char* mutex_name, int repeats)
{
    if (repeats < 1) repeats = 1;
    std::cout << "Мьютекс: " << mutex_name << "\n\n";
    int failures = 0;
    for (const auto& s : SCENARIOS) {
        std::cout << "Сценарий «" << s.title << "»: gluttony = " << s.gluttony
                  << ", efficiency_factor = " << s.efficiency_factor << "\n";
        Table<Mutex> t(s);
        if (t.run() != s.expected) ++failures;
        t.report();

        int bad_repeats = 0;
        for (int r = 1; r < repeats; ++r) {
            std::cout.setstate(std::ios::failbit);   // глушим вывод повторов
            Table<Mutex> tr(s);
            Outcome o = tr.run();
            std::cout.clear();
            if (o != s.expected) ++bad_repeats;
        }
        if (repeats > 1)
            std::cout << "  повторных прогонов: " << repeats - 1
                      << ", с другим исходом: " << bad_repeats << "\n";
        failures += bad_repeats;
        std::cout << "\n";
    }
    return failures;
}
