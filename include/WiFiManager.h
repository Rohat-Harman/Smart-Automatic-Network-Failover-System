
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <string>

// Connect to a saved Wi-Fi profile.
bool connectToWiFi(const std::string& profileName);

// Get the currently connected Wi-Fi SSID.
std::string getCurrentWiFi();

#endif
