#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <string>

unsigned int calculateChecksum(const std::string& data);
bool verifyChecksum(const std::string& data, unsigned int expectedChecksum);

#endif