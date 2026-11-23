// Лабораторная №4 «Три толстяка» (std::mutex)
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdlib>
#include "common.h"

using namespace std;

int dish1 = 3000, dish2 = 3000, dish3 = 3000;  // тарелки толстяков
int eaten1 = 0, eaten2 = 0, eaten3 = 0;        // сколько съел каждый
bool ate1 = true, ate2 = true, ate3 = true;    // съел ли толстяк порцию из текущей партии
int gluttony = 3;            // сколько толстяк съедает за раз
int efficiency_factor = 3;   // сколько Кук кладёт на каждую тарелку за раз
int batches = 0;             // сколько партий выложил Кук (только для вывода)
const char* result = nullptr;  // исход; пока nullptr — всё продолжается
chrono::steady_clock::time_point start;
mutex m;

// Сколько прошло «дней» (1 день = 1 секунда)
double days()
{
    return chrono::duration<double>(chrono::steady_clock::now() - start).count();
}

// Кук кладёт новую партию, только когда все живые толстяки съели свою порцию.
// Пока он раскладывает, мьютекс занят — толстяки ждут.
void cook()
{
    while (true) {
        m.lock();
        if (result) {
            m.unlock();
            break;
        }
        if (days() >= 5) {
            result = "Кук уволился сам!";
            m.unlock();
            break;
        }
        if (ate1 && ate2 && ate3) {
            dish1 += efficiency_factor;
            dish2 += efficiency_factor;
            dish3 += efficiency_factor;
            // лопнувший толстяк больше не ест — считаем, что он уже съел
            ate1 = eaten1 > 10000;
            ate2 = eaten2 > 10000;
            ate3 = eaten3 > 10000;
            batches++;
            m.unlock();
            this_thread::sleep_for(chrono::milliseconds(2));  // готовит следующую партию
        }
        else {
            m.unlock();
            this_thread::yield();
        }
    }
}

// Толстяк ест, только когда на столе новая партия (ate == false).
void fatty(int n, int& dish, int& eaten, bool& ate)
{
    while (true) {
        m.lock();
        if (result || eaten > 10000) {
            m.unlock();
            break;
        }
        if (!ate) {
            int take = dish < gluttony ? dish : gluttony;
            dish -= take;
            eaten += take;
            ate = true;
            if (eaten > 10000) {
                cout << "    толстяк " << n << " лопнул на " << days() << " дн.\n";
                if (eaten1 > 10000 && eaten2 > 10000 && eaten3 > 10000)
                    result = "Кук не получил зарплату.";
            }
            else if (dish == 0) {
                result = "Кука уволили";
            }
        }
        m.unlock();
        this_thread::yield();
    }
}

void run(int g, int e)
{
    gluttony = g;
    efficiency_factor = e;
    dish1 = dish2 = dish3 = 3000;
    eaten1 = eaten2 = eaten3 = 0;
    ate1 = ate2 = ate3 = true;  // новой партии ещё нет — первым ходит Кук
    batches = 0;
    result = nullptr;

    cout << "gluttony = " << g << ", efficiency_factor = " << e << "\n";
    start = chrono::steady_clock::now();
    thread c(cook);
    thread f1(fatty, 1, ref(dish1), ref(eaten1), ref(ate1));
    thread f2(fatty, 2, ref(dish2), ref(eaten2), ref(ate2));
    thread f3(fatty, 3, ref(dish3), ref(eaten3), ref(ate3));
    c.join();
    f1.join();
    f2.join();
    f3.join();

    cout << "  итог через " << days() << " дн. (" << batches << " партий):\n";
    cout << "    толстяк 1: съел " << eaten1 << ", на тарелке " << dish1 << "\n";
    cout << "    толстяк 2: съел " << eaten2 << ", на тарелке " << dish2 << "\n";
    cout << "    толстяк 3: съел " << eaten3 << ", на тарелке " << dish3 << "\n";
    cout << "  >>> " << result << "\n\n";
}

int main()
{
    setup_console();
    cout << "Лаб.4 «Три толстяка» (" << BUILD_CONFIG << ")\n\n";
    run(100, 50);   // Кука уволили
    run(100, 100);  // Кук не получил зарплату
    run(1, 1);      // Кук уволился сам
    system("pause");
    return 0;
}
