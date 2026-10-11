#include "SocketManager.h"
#include "Checksum.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

namespace
{
    const char* TEST_IP = "1.1.1.1";
    const int TCP_PORT = 80;
    const int UDP_PORT = 53;

    bool initializeWinsock()
    {
        WSADATA wsaData;

        int result = WSAStartup(
            MAKEWORD(2, 2),
            &wsaData
        );

        return result == 0;
    }
}


bool performTCPTest()
{
    if (!initializeWinsock())
        return false;

    SOCKET tcpSocket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if (tcpSocket == INVALID_SOCKET)
    {
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(TCP_PORT);

    if (inet_pton(
            AF_INET,
            TEST_IP,
            &serverAddress.sin_addr
        ) != 1)
    {
        closesocket(tcpSocket);
        WSACleanup();
        return false;
    }

    // Set socket to non-blocking mode.
    u_long nonBlocking = 1;

    if (ioctlsocket(
            tcpSocket,
            FIONBIO,
            &nonBlocking
        ) == SOCKET_ERROR)
    {
        closesocket(tcpSocket);
        WSACleanup();
        return false;
    }

    int result = connect(
        tcpSocket,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    );

    bool success = false;

    if (result == 0)
    {
        success = true;
    }
    else if (WSAGetLastError() == WSAEWOULDBLOCK)
    {
        fd_set writeSet;
        fd_set errorSet;

        FD_ZERO(&writeSet);
        FD_ZERO(&errorSet);

        FD_SET(tcpSocket, &writeSet);
        FD_SET(tcpSocket, &errorSet);

        timeval timeout{};
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        int ready = select(
            0,
            nullptr,
            &writeSet,
            &errorSet,
            &timeout
        );

        if (ready > 0 && FD_ISSET(tcpSocket, &writeSet))
        {
            int socketError = 0;
            int errorLength = sizeof(socketError);

            if (getsockopt(
                    tcpSocket,
                    SOL_SOCKET,
                    SO_ERROR,
                    reinterpret_cast<char*>(&socketError),
                    &errorLength
                ) == 0)
            {
                success = (socketError == 0);
            }
        }
    }

    closesocket(tcpSocket);
    WSACleanup();

    return success;
}


bool performUDPTest()
{
    if (!initializeWinsock())
        return false;

    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (udpSocket == INVALID_SOCKET)
    {
        WSACleanup();
        return false;
    }

    DWORD timeout = 2000;
    setsockopt(
        udpSocket,
        SOL_SOCKET,
        SO_RCVTIMEO,
        reinterpret_cast<const char*>(&timeout),
        sizeof(timeout)
    );

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(53);

    inet_pton(AF_INET, "1.1.1.1", &serverAddress.sin_addr);

    // DNS query: example.com A record
    unsigned char query[] = {
        0x12, 0x34, 0x01, 0x00,
        0x00, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x07, 'e','x','a','m','p','l','e',
        0x03, 'c','o','m',
        0x00, 0x00, 0x01, 0x00, 0x01
    };

    int sent = sendto(
        udpSocket,
        reinterpret_cast<const char*>(query),
        sizeof(query),
        0,
        reinterpret_cast<sockaddr*>(&serverAddress),
        sizeof(serverAddress)
    );

    char response[512];

    int received = SOCKET_ERROR;

    if (sent == sizeof(query))
    {
        received = recvfrom(
            udpSocket,
            response,
            sizeof(response),
            0,
            nullptr,
            nullptr
        );
    }

    bool success =
        received >= 12 &&
        static_cast<unsigned char>(response[0]) == 0x12 &&
        static_cast<unsigned char>(response[1]) == 0x34 &&
        (static_cast<unsigned char>(response[2]) & 0x80) != 0 &&
        (static_cast<unsigned char>(response[3]) & 0x0F) == 0;

    closesocket(udpSocket);
    WSACleanup();

    return success;
}


bool performSocketTest()
{
    bool tcpResult = performTCPTest();
    bool udpResult = performUDPTest();

    return tcpResult && udpResult;
}