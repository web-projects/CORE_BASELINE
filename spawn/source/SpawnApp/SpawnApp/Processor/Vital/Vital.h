#pragma once

#include "..\TCProcessor.h"

class Vital : public Platform
{
public:
	Vital(TCProcessor* p);

	virtual void runTransaction(StringHash& data, StringHash& response);

protected:
	bool buildSaleReq(StringHash& data, StringHash& store);
};

