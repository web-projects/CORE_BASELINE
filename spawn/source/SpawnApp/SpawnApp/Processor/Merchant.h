#ifndef _MERCHANT_H
#define _MERCHANT_H

#pragma once
class Merchant
{
public:
	Merchant() {
		platform[0] = 'v';
		platform[1] = 'i';
		platform[2] = 't';
		platform[3] = 'a';
		platform[4] = 'l';
		platform[5] = '\0';
		industry_code[0] = 'D';
	}

	const char* getPlatform() const { return platform; }
	const char* getIndustryCode() const { return industry_code; }

private:
	// Merchant account fields
	static const size_t platform_max = 40;
	char platform[platform_max];
	char industry_code[1 + 1];
};

#endif // _MERCHANT_H
