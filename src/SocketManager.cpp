#include "SocketManager.h"

#include <winsock2.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

bool performSocketTest()
{
    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        cout << "Winsock could not be started." << endl;
        return false;
    }

    cout << "Winsock started successfully." << endl;

    WSACleanup();

    return true;
}