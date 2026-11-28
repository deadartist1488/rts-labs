# Системы реального времени — лабораторные работы

**Студент:** Пряников Максим Андреевич

**ОС:** Windows 11 Pro. **IDE:** Visual Studio Community 2026 (MSVC 19.52), CMake 4.3.

Все работы — один CMake-проект, у каждой лабораторной своя папка с кодом, отчётом и скриншотами.

| № | Тема | Код | Отчёт |
|---|------|-----|-------|
| 1 | Утренняя зарядка: unsigned-цикл, факториал Debug/Release, 2 потока | [lab1](lab1) | [md](lab1/REPORT.md), [docx](lab1/REPORT.docx) |
| 2 | What's going on? 3 потока, 1 секунда, мьютекс и без него | [lab2](lab2) | [md](lab2/REPORT.md), [docx](lab2/REPORT.docx) |
| 3 | 100 и 1 монета: дележ на одном mutex | [lab3](lab3) | [md](lab3/REPORT.md), [docx](lab3/REPORT.docx) |
| 4 | Три толстяка | [lab4](lab4) | [md](lab4/REPORT.md), [docx](lab4/REPORT.docx) |
| 5 | Самодельный мьютекс (atomic + compare_exchange) | [lab5](lab5) | [md](lab5/REPORT.md), [docx](lab5/REPORT.docx) |
| 6 | Корутины C++20, прогресс-бар с именем | [lab6](lab6) | [md](lab6/REPORT.md), [docx](lab6/REPORT.docx) |
| 7 | UDP и TCP сокеты (WinSock): сервер и клиент, выход по exit и таймауту, хеш в ответ | [lab7](lab7) | [md](lab7/REPORT.md), [docx](lab7/REPORT.docx) |
| X | «По ком звонит Колокольчик!»: lock-free стек из лекции, 1 писатель и 4 читателя, проверка консистентности, переполнение версии | [lab8](lab8) | [md](lab8/REPORT.md), [docx](lab8/REPORT.docx) |

## Как собирались и запускались проекты

Все лабораторные — подпроекты одного корневого `CMakeLists.txt` (`add_subdirectory(lab1)` … `add_subdirectory(lab8)`). Лаб.5 — это код лаб.4, где `std::mutex` заменён на свой `SpinMutex`.

Сборка шла из командной строки CMake, который входит в Visual Studio 2026, с генератором Visual Studio (x64), в обеих конфигурациях:

```
cmake -S . -B build
cmake --build build --config Debug
cmake --build build --config Release
```

Исполняемые файлы появляются в `build/labN/Debug` и `build/labN/Release`. Каждая программа запускалась в отдельном окне консоли (cmd) из папки `build`, например `lab4\Release\lab4.exe`. Скриншоты в отчётах — эти окна целиком.

Вспомогательные скрипты для запуска:

- `tools/run3.cmd` — запускает программу три раза подряд в одном окне (лаб.2, сравнение запусков);
- `tools/run_exitcode.cmd` — запускает программу и печатает её код возврата (лаб.1, п.1.1).

Тот же проект открывается в Visual Studio через «Файл → Открыть → Папка» (корень репозитория): VS сам находит `CMakeLists.txt`, и в нём можно собирать, запускать и отлаживать любую из программ.

## Прочее

- `student.cmake` — ФИО и имя для прогресс-бара в лаб. 6 (одно место на весь проект).
- `common.h` — переключение консоли Windows в UTF-8.
- `tools/md2docx.py` — собирает `REPORT.docx` из `REPORT.md`.
