// SpawnApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include <vector>
#include <map>
#include <filesystem>

#include "fileprocessor/IniParser.h"

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

//using namespace System;
//using namespace System::Configuration;
//using System.Configuration;

using namespace std;
namespace fs = std::filesystem;

#define MAX_LEN 255

struct _opt
{
    int argc;
    char** argv;
    char* options_file;
} opt;

map<string, string> options_map;

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

static std::vector<std::string> Split(const std::string& s, char seperator)
{
    std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while ((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring(s.substr(prev_pos, pos - prev_pos));

        output.push_back(substring);

        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

    return output;
}

/** @fn OptHandler(...)
 * Callback function used with the INI parser to populate the internal data map.
 * Return 0 if the key has already been used, 1 if successfully mapped.
 */
static int OptHandler(void* user, const char* section, const char* name, const char* value)
{
    if (options_map.count(name) == 0)
    {
        options_map[name] = value;
    }
    else
    {
        fprintf(stderr, "ERROR: Key already defined in file, correct key %s.\n", name);
        return 0;
    }
    return 1;
}

static int Help(char* prog)
{
    fprintf(stderr, "%s -c config_file\n", prog);
    return -1;
}

static const char* GetRunningDirectory()
{
    //std::filesystem::path cwd = 
    //char* path = std::filesystem::current_path()
    //string fullName = Assembly.GetEntryAssembly().Location;
    //string logname = Path.GetFileNameWithoutExtension(fullName) + ".log";
    //char* path = Directory.GetCurrentDirectory();
    //logfilenamePath = path + "\\logs\\" + logname;

    //std::string currentPath = std::filesystem::current_path().string();
    std::string currentPath = std::filesystem::current_path().string();

    return currentPath.c_str();
}

TCHAR* GetThisPath(TCHAR* dest, size_t destSize)
{
    if (!dest)
    {
        return NULL;
    }

    if (MAX_PATH > destSize)
    {
        return NULL;
    }

    DWORD length = GetModuleFileName(NULL, dest, destSize);
    PathRemoveFileSpec(dest);
    
    return dest;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        return Help(argv[0]);
    }

    if (strcmp(argv[1], "-c"))
    {
        return Help(argv[0]);
    }

    opt.argc = argc;
    opt.argv = argv;
    opt.options_file = argv[2];

    int i;
    map<string, string>::iterator key;
    FILE* fl;

    IniParser iniParser = IniParser();

    //const char* current_path = GetRunningDirectory();
    TCHAR dest[MAX_PATH];
    GetThisPath(dest, MAX_PATH);

    char c_szText[MAX_LEN];
    size_t charsConverted = 0;
    wcstombs_s(&charsConverted, c_szText, MAX_LEN, dest, wcslen(dest) + 1);
    strcat_s(c_szText, "\\");
    strcat_s(c_szText, opt.options_file);

    //i = iniParser.ini_parse(opt.options_file, OptHandler, NULL);
    i = iniParser.ini_parse(c_szText, OptHandler, NULL);

    if (i != 0)
    {
        if (i < 0)
        {
            fprintf(stderr, "ERROR: Unable to open configuration file: %s\n", opt.options_file);
        }
        else
        {
            fprintf(stderr, "ERROR: Error in configuration file, %s, on line %d.\n", opt.options_file, i);
        }

        return -5;
    }

    fprintf(stdout, "OPTIONS loaded: %d\n", (int)options_map.size());

    for (auto const& [key, val] : options_map)
    {
        std::cout << key    // string (key)
            << ':'
            << val          // string's value
            << std::endl;
    }
}
