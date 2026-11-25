#include <iostream>
#include <string>
#include <WS2tcpip.h> // WINAPI библиотека сокетов

//Подключаем библиотеку на этапе компоновки
#pragma comment (lib, "ws2_32.lib")

using namespace std;

// Главная функция сервера
int main()
{
    ////////////////////////////////////////////////////////////
    // Инициализация WINSOCK
    ////////////////////////////////////////////////////////////

    // Структура хранящая информацию о версии сокета
    // необходима для вызова WSAStartup()
    WSADATA data;

    // При старте WinSock, нужно определить версию сокета
    // Существуют 1.0 1.1 2.0 2.1 2.2
    // Будем использовать WinSock версии 2.2
    // сохраним 2 и 2 в hex виде 0x0202
    WORD version = MAKEWORD(2, 2);

    int wsOk = WSAStartup(version, &data);// Запустим WinSock
    if (wsOk != 0)
    {
        cout << "Can't start Winsock! " << wsOk;
        return 1;
    }

    // Используем тип сокета - SOCK_DGRAM сокет датаграммы(UDP) - Передача данных в виде отдельных сообщений
    SOCKET in = socket(AF_INET, SOCK_DGRAM, 0); //Создаем сокет

    // Создаем hint сервера - "подсказка"
    sockaddr_in serverHint;
    serverHint.sin_addr.S_un.S_addr = ADDR_ANY; // Любой доступный IP на машине
    serverHint.sin_family = AF_INET; // Задаем формат адресов IPv4
    // Задаем порт
    // сконвертировав обратный порядок байтов в прямой (little to big endian)
    serverHint.sin_port = htons(54000);

    // Пытаемся привязать сокет с IP и портом
    if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
    {
        cout << "Can't bind socket! " << WSAGetLastError() << endl;
        return 1;
    }

    // Таймаут приёма 60 секунд: если сообщений нет, recvfrom вернёт ошибку WSAETIMEDOUT
    DWORD timeout = 60000;
    setsockopt(in, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    sockaddr_in client; // Используется для хранения информации о клиенте (port / ip address)
    int clientLength = sizeof(client); // Размер информации о клиенте
    char buf[1024];//буфер для приема инф-и

    // Запускаем жизненный цикл
    while (true)
    {
        ZeroMemory(&client, clientLength); // Очистим структуру клиента
        ZeroMemory(buf, 1024); // Очистим буфер приема (переопределим)

        // Ждем сообщение
        int bytesIn = recvfrom(in, buf, 1024, 0, (sockaddr*)&client, &clientLength);
        if (bytesIn == SOCKET_ERROR)
        {
            if (WSAGetLastError() == WSAETIMEDOUT)
            {
                cout << "No messages for 60 seconds. Quitting" << endl;
                break;
            }
            cout << "Error receiving from client " << WSAGetLastError() << endl;
            continue;
        }

        char clientIp[256]; // Выделяем 256 байт для конвертации адреса клиента в строку
        ZeroMemory(clientIp, 256); // заполняем память нулями

        // Конвертируем массив байт в символы
        inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);
        // Показываем сообщение и кто его отправил
        cout << "Message recv from " << clientIp << " : " << buf << endl;

        // Выход по сообщению exit
        if (string(buf) == "exit")
        {
            cout << "Got exit. Quitting" << endl;
            break;
        }
    }

    // Закрываем socket
    closesocket(in);
    // Отключаем winsock
    WSACleanup();
}
