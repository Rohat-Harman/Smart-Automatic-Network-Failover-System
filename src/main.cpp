#include <iostream>
#include "SocketManager.h"

using namespace std;

int main()
{
    cout << "Smart Automatic Network Failover System started." << endl;

    bool socketResult = performSocketTest();

    if (socketResult)
    {
        cout << "Socket test successful." << endl;
    }
    else
    {
        cout << "Socket test failed." << endl;
    }

    return 0;
}