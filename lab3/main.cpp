// Лабораторная №3: дележ монет двумя ворами (Боб и Том).
// Для синхронизации используется только std::mutex.
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <cstdlib>
#include "common.h"

int coins = 101;       // общий мешок
int Bob_coins = 0;
int Tom_coins = 0;
int Dead_coins = 0;    // монета покойнику
bool verbose = true;   // печатать каждый шаг
bool force_verbose = false; // ключ -v: печатать шаги и для больших мешков

std::mutex m;

// Вор берёт монету, только если у него монет не больше, чем у подельника,
// — так воры берут строго по очереди. Если осталась последняя монета, а
// поровну у воров уже есть, её отдают покойнику.
void coin_sharing(std::string name, int& thief_coins, int& companion_coins)
{
    for (;;)
    {
        m.lock();
        if (coins == 0) {               // мешок пуст — дележ закончен
            m.unlock();
            break;
        }
        if (coins == 1 && thief_coins == companion_coins) {
            coins--;                    // последняя монета — покойнику
            Dead_coins++;
            if (verbose)
                std::cout << name << ": последнюю монету отдаём покойнику\n";
            m.unlock();
            break;
        }
        if (thief_coins <= companion_coins) {
            coins--;                    // берём ровно одну монету
            thief_coins++;
            if (verbose)
                std::cout << name << " :" << thief_coins << " " << companion_coins
                          << "  (осталось " << coins << ")\n";
        }
        m.unlock();
        std::this_thread::yield();      // даём подельнику шанс взять свою
    }
}

void run(int total)
{
    coins = total;
    Bob_coins = Tom_coins = Dead_coins = 0;
    verbose = force_verbose || total <= 10;

    std::cout << "=== Монет в мешке: " << total << " ===\n";
    std::thread bob(coin_sharing, "Bob", std::ref(Bob_coins), std::ref(Tom_coins));
    std::thread tom(coin_sharing, "Tom", std::ref(Tom_coins), std::ref(Bob_coins));
    bob.join();
    tom.join();

    int sum = Bob_coins + Tom_coins + Dead_coins;
    std::cout << "Итог: Bob = " << Bob_coins << ", Tom = " << Tom_coins
              << ", покойник = " << Dead_coins << ", сумма = " << sum
              << (sum == total && Bob_coins == Tom_coins ? "  [OK]" : "  [ОШИБКА]")
              << "\n\n";
}

int main(int argc, char** argv)
{
    setup_console();
    std::vector<int> tests = { 0, 1, 2, 5, 100, 101 };
    if (argc > 1) {                     // можно задать свои количества: lab3 7 8 9
        tests.clear();
        for (int k = 1; k < argc; ++k) {
            if (std::string(argv[k]) == "-v") force_verbose = true;
            else tests.push_back(std::atoi(argv[k]));
        }
        if (tests.empty()) tests = { 0, 1, 2, 5, 100, 101 };
    }
    for (int t : tests) run(t);

    // Прогон на устойчивость: 1000 раз для 100 и 101 монеты.
    int bad = 0;
    for (int rep = 0; rep < 1000; ++rep)
        for (int total : { 100, 101 }) {
            coins = total; Bob_coins = Tom_coins = Dead_coins = 0; verbose = false;
            std::thread a(coin_sharing, "Bob", std::ref(Bob_coins), std::ref(Tom_coins));
            std::thread b(coin_sharing, "Tom", std::ref(Tom_coins), std::ref(Bob_coins));
            a.join(); b.join();
            if (Bob_coins != Tom_coins || Bob_coins + Tom_coins + Dead_coins != total
                || Dead_coins != total % 2) ++bad;
        }
    std::cout << "Стресс-тест 2000 прогонов (100 и 101 монета): ошибок " << bad << "\n";

    system("pause");
    return 0;
}
