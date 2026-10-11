#include "Logger.h"

#include <fstream>
#include <iostream>
#include <ctime>
#include <filesystem>

using namespace std;

void logMessage(const string& message)
{
    filesystem::create_directories("logs");

    ofstream logFile("logs/network.log", ios::app);

    time_t now = time(nullptr);
    tm localTime{};

    localtime_s(&localTime, &now);

    char timeBuffer[32];

    strftime(
        timeBuffer,
        sizeof(timeBuffer),
        "%Y-%m-%d %H:%M:%S",
        &localTime
    );

    cout << "[" << timeBuffer << "] " << message << endl;

    if (logFile.is_open())
    {
        logFile << "[" << timeBuffer << "] "
                << message << endl;
    }
}