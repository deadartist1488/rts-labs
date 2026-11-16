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

## Сборка

В Visual Studio: «Открыть локальную папку» → корень репозитория, VS сам подхватит `CMakeLists.txt`.

Из командной строки (Developer PowerShell for VS):

```
cmake -S . -B build
cmake --build build --config Debug
cmake --build build --config Release
```

Исполняемые файлы появятся в `build/labN/Debug` и `build/labN/Release`.

## Прочее

- `student.cmake` — ФИО и имя для прогресс-бара в лаб. 6 (одно место на весь проект).
- `common.h` — переключение консоли Windows в UTF-8.
- `tools/run3.cmd` — запуск программы три раза подряд (для лаб. 2).
- `tools/md2docx.py` — собирает `REPORT.docx` из `REPORT.md`.
