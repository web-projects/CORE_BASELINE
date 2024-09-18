#pragma once

#include "..\Vital\cofInfo.h"

#define CMIDATA_LENGTH 4

class StringHash;

class mccitmit : public CofInfo
{
public:
    mccitmit(StringHash& data,
        const std::string& transId,
        const char industryCode,
        const std::string& caller,
        std::string platform);

    char* getCMIndicator(char* buffer, unsigned int len) const;
    std::string _platform;
};

