// Пункт 1.1 «Разминка»: что выведет программа и почему?
#include <iostream>
#include <string>
#include <cstdlib>
#include "common.h"

using namespace std;

int main(int argc, char** argv)
{
    setup_console();
    cout << "Пункт 1.1 (" << BUILD_CONFIG << ")\n";

    string s = "01234";

    // --- Исходный код из задания (с одним добавлением: выводим i,
    //     чтобы было видно, что происходит со счётчиком) ---
    // Цикл бесконечный, поэтому ограничиваем число итераций для демонстрации.
    int guard = 0;
    for (unsigned int i = s.size() - 1; i >= 0; i--)
    {
        cout << "i = " << i << "  -> ";
        if (i >= s.size()) {
            // Обращение s[i] здесь — выход за границы строки (UB).
            // В Debug-сборке MSVC это assert «string subscript out of range»,
            // в Release — чтение чужой памяти / падение. Не делаем его.
            cout << "s[" << i << "] — выход за границы строки!\n";
            if (++guard >= 3) {
                cout << "... и так бесконечно: условие i >= 0 для unsigned всегда истинно.\n";
                break;
            }
            continue;
        }
        cout << s[i] << endl;
    }

    // Если запустить с аргументом "raw", выполняется ровно код из задания без защиты.
    if (argc > 1 && string(argv[1]) == "raw") {
        cout << "\nЗапуск исходного кода без защиты:\n";
        for (unsigned int i = s.size() - 1; i >= 0; i--)
        {
            std::cout << s[i] << std::endl;
        }
    }

    // --- Исправленный вариант ---
    cout << "\nИсправленный вариант (i-- > 0):\n";
    for (size_t i = s.size(); i-- > 0; )
        cout << s[i] << endl;

    system("pause");
    return 0;
}
