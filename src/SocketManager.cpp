#include "SocketManager.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

bool performSocketTest()
{
    // Windows socket system information
    WSADATA wsaData;

    // Start Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        cout << "Winsock could not be started." << endl;
        return false;
    }

    cout << "Winsock started successfully." << endl;

    // Create a TCP socket
    SOCKET tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (tcpSocket == INVALID_SOCKET)
    {
        cout << "TCP socket could not be created." << endl;
        WSACleanup();
        return false;
    }

    cout << "TCP socket created successfully." << endl;

    // Define the server address
    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(80);

    // Convert the IP address into binary form
    if (inet_pton(AF_INET, "1.1.1.1", &serverAddress.sin_addr) != 1)
    {
        cout << "Invalid server IP address." << endl;

        closesocket(tcpSocket);
        WSACleanup();

        return false;
    }

    // Try to establish a TCP connection
    int connectResult = connect(
        tcpSocket,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    );

    if (connectResult == SOCKET_ERROR)
    {
        cout << "TCP connection failed." << endl;

        closesocket(tcpSocket);
        WSACleanup();

        return false;
    }

    cout << "TCP connection successful." << endl;

    // Close the socket
    closesocket(tcpSocket);

    // Shut down Winsock
    WSACleanup();

    return true;
}