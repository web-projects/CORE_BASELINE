#ifndef _TCPROCESSOR_H
#define _TCPROCESSOR_H

#include "StringHash.h"
#include "Platform.h"
#include "Merchant.h"

class TCProcessor
{
public:
	TCProcessor();
	~TCProcessor();

	// Merchant data
	class Merchant* merchant;

	char* transID;

//protected:
	// Platform (Vital, NDC, FirstData, etc)
	Platform* platform;

	bool assignPlatform(const char* media);
	void runTransaction(StringHash& data, StringHash& response);

protected:
	bool assignPlatform(const char* media, Platform*& _platform);
};

#endif // _TCPROCESSOR_H
