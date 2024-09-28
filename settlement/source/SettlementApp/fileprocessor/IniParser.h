#pragma once
#include <cstdio>
#include <ctype.h>
#include <string.h>
#pragma once

#define MAX_LINE 2000
#define MAX_SECTION 50
#define MAX_NAME 50

class IniParser
{
public:
	int ini_parse(const char* filename,
		int (*handler)(void*, const char*, const char*, const char*),
		void* user);

private:
	char* rstrip(char* s);
	char* lskip(const char* s);
	char* find_char_or_comment(const char* s, char c);
	char* strncpy0(char* dest, const char* src, size_t size);
	int ini_parse_file(FILE* file,
		int (*handler)(void*, const char*, const char*,
			const char*),
		void* user);
};

