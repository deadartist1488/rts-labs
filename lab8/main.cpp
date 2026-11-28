// Лабораторная №X «По ком звонит Колокольчик!»: lock-free стек из лекции, 1 писатель и 4 читателя
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <string>
#include <cmath>
#include <atomic>
#include <cstdlib>
#include "common.h"
#include <psapi.h>
#include "lockfree_stack.hpp"

using namespace std;

struct Position {
    double x;
    double y;
};

typedef lf::Node<Position>* NodePtr;

const int N = 40;                  // число шагов по Ox
const double h = 4.0 / N;          // шаг
const double eps = 1e-9;           // допуск сравнения
const unsigned int seed = 12345;  // seed генератора писателя
const int READERS = 4;

lf::LockFreeVersionedStack<Position> stack(READERS);
// atomic: главный поток читает счётчики для прогресса, пока писатель и читатели их меняют
atomic<long long> versions{ 0 };     // сколько версий создал писатель
atomic<long long> reads[READERS];    // сколько снимков прочёл каждый читатель
atomic<long long> distinct[READERS]; // сколько разных версий увидел каждый читатель
atomic<long long> errors[READERS];   // сколько снимков с нарушениями
string first_error[READERS];         // первое нарушение у читателя (для отчёта)

double parabola(double x)
{
    return -x * x + 4 * x;
}

// Писатель: добавляет точки до x = 4, потом снимает с вершины случайное число точек и снова добавляет
void writer()
{
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(1, N + 1);  // сколько снять: от 1 до всех точек
    int top = -1;                                   // индекс i точки на вершине, -1 — стек пуст
    while (true) {
        if (stack.is_stopped())
            break;
        if (top < N) {
            top++;
            double x = 4.0 * top / N;
            stack.push(Position{ x, parabola(x) });
            versions.fetch_add(1, memory_order_relaxed);
        }
        else {
            int k = dist(gen);
            for (int j = 0; j < k; j++) {
                if (stack.pop()) {
                    top--;
                    versions.fetch_add(1, memory_order_relaxed);
                }
            }
        }
    }
}

// Проверка снимка: вершина — последняя добавленная точка, дальше x убывает на h
string check(NodePtr p)
{
    NodePtr prev = nullptr;
    while (p != nullptr) {
        double x = p->data.x, y = p->data.y;
        if (!isfinite(x) || !isfinite(y))
            return "не конечная координата";
        if (x < 0 || x > 4)
            return "x вне [0, 4]: " + to_string(x);
        if (fabs(y - parabola(x)) > eps)
            return "точка не на параболе: x=" + to_string(x) + " y=" + to_string(y);
        if (prev != nullptr && fabs(prev->data.x - x - h) > eps)
            return "шаг не h: " + to_string(prev->data.x) + " -> " + to_string(x);
        prev = p;
        p = p->next;
    }
    return "";
}

void reader(unsigned int id)
{
    unsigned int last = 0;  // номер версии предыдущего снимка этого читателя
    while (true) {
        if (stack.is_stopped())
            break;
        NodePtr p;
        if (!stack.subscribe(id, p))
            break;
        unsigned int v = stack.subscribed_version(id);
        string e = check(p);
        stack.unsubscribe(id);
        reads[id].fetch_add(1, memory_order_relaxed);
        if (v != last) {
            distinct[id].fetch_add(1, memory_order_relaxed);
            last = v;
        }
        if (!e.empty()) {
            if (errors[id] == 0)
                first_error[id] = e;
            errors[id].fetch_add(1, memory_order_relaxed);
        }
    }
}

int main(int argc, char* argv[])
{
    setup_console();
    int seconds = 30;
    if (argc > 1)
        seconds = atoi(argv[1]);
    cout << "Сборка: " << BUILD_CONFIG << "\n";
    cout << "seed = " << seed << ", N = " << N << ", h = " << h << ", читателей: " << READERS
         << ", время: " << seconds << " с" << endl;

    thread w(writer);
    thread r[READERS];
    for (int i = 0; i < READERS; i++)
        r[i] = thread(reader, i);

    // Главный поток раз в 100 мс замеряет память процесса (WorkingSetSize)
    double sum = 0, mx = 0;
    int samples = 0;
    long long next = 60;  // когда печатать следующую строку прогресса, с
    auto start = chrono::steady_clock::now();
    while (true) {
        if (chrono::steady_clock::now() - start >= chrono::seconds(seconds))
            break;
        this_thread::sleep_for(chrono::milliseconds(100));
        PROCESS_MEMORY_COUNTERS pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
        double mb = pmc.WorkingSetSize / 1024.0 / 1024.0;
        sum += mb;
        if (mb > mx)
            mx = mb;
        samples++;
        auto t = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count();
        if (t >= next) {  // прогресс раз в 60 с
            next += 60;
            long long rs = 0, es = 0;
            for (int i = 0; i < READERS; i++) {
                rs += reads[i];
                es += errors[i];
            }
            cout << "[" << t << " с] версий: " << versions << ", last_version() = " << stack.last_version()
                 << ", память " << mb << " МБ, снимков: " << rs << ", ошибок: " << es << endl;
        }
    }

    stack.stop();
    w.join();
    for (int i = 0; i < READERS; i++)
        r[i].join();

    cout << "Писатель создал версий: " << versions << " (last_version() = " << stack.last_version() << ")\n";
    for (int i = 0; i < READERS; i++) {
        cout << "Читатель " << i << ": снимков " << reads[i] << ", разных версий " << distinct[i]
             << ", ошибок " << errors[i];
        if (errors[i] > 0)
            cout << " (первая: " << first_error[i] << ")";
        cout << "\n";
    }
    cout << "Память (WorkingSetSize), замеров " << samples << ": средняя " << sum / samples
         << " МБ, максимальная " << mx << " МБ\n";
    return 0;
}
