#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

bool checkInternetConnection();
bool checkConnectionStability(
    int requiredSuccessfulTests,
    int delaySeconds
);

#endif