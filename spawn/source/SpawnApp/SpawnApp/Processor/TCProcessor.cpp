#include <stddef.h>
//#include <strings.h>	// strcasecmp
#include <iostream>
#include <string>
#include <cstring>

#include "TCProcessor.h"
#include "Vital\Vital.h"

#define strcasecmp _stricmp

TCProcessor::TCProcessor()
{
	platform = NULL;
	merchant = new Merchant();
}

TCProcessor::~TCProcessor()
{
	merchant = NULL;
	if (platform)
	{
		delete platform;
		platform = NULL;
	}
}

void TCProcessor::runTransaction(StringHash& data, StringHash& response)
{
	if (platform)
	{
		platform->runTransaction(data, response);
	}
}

bool TCProcessor::assignPlatform(const char* media, Platform*& _platform)
{
	_platform = NULL;

	if (strcasecmp(merchant->getPlatform(), "vital") == 0)
	{
		std::cout << "platform:vital\n";
		_platform = new Vital(this);
	}

	return true;
}

bool TCProcessor::assignPlatform(const char* media)
{
	return assignPlatform(media, platform);
}