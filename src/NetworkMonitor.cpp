#include "NetworkMonitor.h"
#include "SocketManager.h"
#include "Logger.h"

#include <thread>
#include <chrono>

using namespace std;

bool checkInternetConnection()
{
    bool result = performSocketTest();

    if (result)
    {
        logMessage("Internet connection test successful.");
    }
    else
    {
        logMessage("Internet connection test failed.");
    }

    return result;
}

bool checkConnectionStability(
    int requiredSuccessfulTests,
    int delaySeconds)
{
    for (int i = 0;
         i < requiredSuccessfulTests;
         i++)
    {
        if (!checkInternetConnection())
        {
            return false;
        }

        this_thread::sleep_for(
            chrono::seconds(delaySeconds)
        );
    }

    return true;
}