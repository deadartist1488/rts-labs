// Лабораторная №4 «Три толстяка» (std::mutex)
#include <cstdlib>
#include <mutex>
#include "common.h"
#include "fatties.h"

int main(int argc, char** argv)
{
    setup_console();
    std::cout << "Лаб.4 «Три толстяка» (" << BUILD_CONFIG << ")\n";
    int repeats = argc > 1 ? std::atoi(argv[1]) : 1;   // «lab4 5» — по 5 прогонов
    int bad = run_all<std::mutex>("std::mutex", repeats);
    std::cout << (bad == 0 ? "Все сценарии дали ожидаемый исход.\n"
                           : "ЕСТЬ НЕОЖИДАННЫЕ ИСХОДЫ!\n");
    system("pause");
    return bad;
}
