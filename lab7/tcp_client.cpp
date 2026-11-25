#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
    string ipAddress = "127.0.0.1";
    int port = 54000;

    WSAData data;
    WORD ver = MAKEWORD(2, 2);
    int wsResult = WSAStartup(ver, &data);
    if (wsResult != 0)
    {
        cerr << "Can't start Winsock, Err #" << wsResult << endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons((u_short)port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR)
    {
        cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    char buf[4096];
    string userInput;

    while (true)
    {
        cout << "> ";
        // Ввод закончился (EOF, Ctrl+Z) — выходим
        if (!getline(cin, userInput))
        {
            break;
        }

        if (userInput.size() > 0)
        {
            // Send the text
            int sendResult = send(sock, userInput.c_str(), (int)userInput.size() + 1, 0);

            // Выход по слову exit (оно уже отправлено серверу, хеш не ждём)
            if (userInput == "exit")
            {
                break;
            }

            if (sendResult != SOCKET_ERROR)
            {
                ZeroMemory(buf, 4096);
                int bytesReceived = recv(sock, buf, 4096, 0);
                if (bytesReceived > 0)
                {
                    cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;

                    // Считаем хеш своего сообщения тем же способом и сравниваем с ответом сервера
                    string myHash = to_string(hash<string>{}(userInput));
                    if (string(buf) == myHash)
                    {
                        cout << "Hash OK" << endl;
                    }
                    else
                    {
                        cout << "Hash MISMATCH" << endl;
                    }
                }
            }
        }
    }

    closesocket(sock);
    WSACleanup();
}
