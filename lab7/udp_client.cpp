#include <iostream>
#include <string>
#include <WS2tcpip.h>
//Подключаем библиотеку на этапе компоновке
#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main()
{
    ////////////////////////////////////////////////////////////
    // ИНИЦИАЛИЗИРУЕМ WINSOCK
    ////////////////////////////////////////////////////////////

    WSADATA data;
    // Запускаем сокет версии 2.2
    int wsOk = WSAStartup(MAKEWORD(2, 2), &data);
    if (wsOk != 0)
    {
        cout << "Can't start Winsock! " << wsOk;
        return 1;
    }

    // Создаем hint сервера
    sockaddr_in server;
    server.sin_family = AF_INET; // AF_INET = IPv4 addresses
    server.sin_port = htons(54000); // Задаем порт
    //Задаем ip локальной машины.
    // Ф-ия преобразует строку ip в массив байт
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    // Создаем сокет с типом данных SOCK_DGRAM
    SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

    string msg = "";
    while (true)
    {
        msg.clear();
        cin >> msg; //читаем из консоли
        //Отправляем сообщение
        int sendOk = sendto(out, msg.c_str(), (int)msg.size() + 1, 0, (sockaddr*)&server, sizeof(server));
        if (sendOk == SOCKET_ERROR)
        {
            cout << "That didn't work! " << WSAGetLastError() << endl;
        }
        // Выход по слову exit (оно уже отправлено серверу)
        if (msg == "exit")
        {
            break;
        }
    }
    // Закрываем сокет
    closesocket(out);
    WSACleanup();
}
