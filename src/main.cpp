
#include "NetworkMonitor.h"
#include "WiFiManager.h"
#include "Logger.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>

using namespace std;

int main()
{
    

    
    const string PRIMARY_WIFI = "ALGX-home";
    const string BACKUP_WIFI = "Rohat iPhone’u";

    const int CHECK_INTERVAL = 5;
    const int FAILURE_THRESHOLD = 3;

    int failureCount = 0;
    bool usingBackup = false;

    logMessage("Smart Automatic Network Failover System started.");

    while (true)
    {
        string currentWiFi = getCurrentWiFi();

        if (currentWiFi == BACKUP_WIFI)
        {
            usingBackup = true;
        }
        else if (currentWiFi == PRIMARY_WIFI)
        {
            usingBackup = false;
        }
        else
        {
            logMessage("Connected Wi-Fi is unknown or disconnected.");
        }

        if (!usingBackup)
        {
            bool internetAvailable = checkInternetConnection();

            if (internetAvailable)
            {
                failureCount = 0;
            }
            else
            {
                failureCount++;

                logMessage(
                    "Connection failure count: " +
                    to_string(failureCount)
                );

                if (failureCount >= FAILURE_THRESHOLD)
                {
                    logMessage("Attempting backup hotspot connection.");

                    if (connectToWiFi(BACKUP_WIFI))
                    {
                        logMessage("Backup Wi-Fi profile connected.");

                        if (checkInternetConnection())
                        {
                            logMessage("Backup internet connection verified.");
                            usingBackup = true;
                        }
                        else
                        {
                            logMessage("Backup connected, but internet test failed.");
                        }
                    }

                    failureCount = 0;
                }
            }
        }
        else
        {
            logMessage(
                "Backup hotspot active. "
                "Automatic failback is temporarily disabled."
            );
        }

        this_thread::sleep_for(
            chrono::seconds(CHECK_INTERVAL)
        );
    }

    return 0;
}
