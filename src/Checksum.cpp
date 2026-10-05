#include "Checksum.h"

unsigned int calculateChecksum(const std::string& data)
{
    unsigned int checksum = 0;

    for (unsigned char character : data)
    {
        checksum += character;
    }

    return checksum;
}

bool verifyChecksum(const std::string& data, unsigned int expectedChecksum)
{
    return calculateChecksum(data) == expectedChecksum;
}