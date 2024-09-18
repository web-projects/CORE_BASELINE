#pragma once

#include <string>

class TSYSCofInfo : public CofInfo
{
public:

    TSYSCofInfo(StringHash& data,
        const std::string& transId,
        const char industryCode,
        const std::string& caller);

    bool  isFirstCOFtrans() const;
	char* getCMIndicator(char* buffer, unsigned int len) const;
};

