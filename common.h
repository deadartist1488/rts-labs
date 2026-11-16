#pragma once
// Общий хелпер: включает UTF-8 в консоли Windows, чтобы русский текст выводился нормально.
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
inline void setup_console() { SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8); }
#else
inline void setup_console() {}
#endif

#ifdef NDEBUG
#define BUILD_CONFIG "Release"
#else
#define BUILD_CONFIG "Debug"
#endif
