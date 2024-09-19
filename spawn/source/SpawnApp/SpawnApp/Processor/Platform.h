#ifndef _PLATFORM_H
#define _PLATFORM_H

#include "..\Common\StringHash.h"

class TCProcessor;

class Platform
{
public:
	Platform(TCProcessor* p);
	virtual ~Platform();

	// runTransaction should always be overloaded, even if it's just to do nothing
	/// Attempts to run a transaction with a set of data.
	virtual void runTransaction(StringHash& data, StringHash& response) = 0;

	// actual: processing_platform.cpp
	void put(const char* string, int len);

protected:
	TCProcessor* Proc;
	char* transID;
};

#endif // _PLATFORM_H
