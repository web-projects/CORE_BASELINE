// SpawnApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include <vector>

#include "Processor\TCProcessor.h"

using namespace std;

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

static StringHash ProcessArguments(int argc, char* argv[])
{
    StringHash data;

    // start at index 1
    for (int index = 1; index < argc; index++)
    {
        std::string runArgument = argv[index];
        if (runArgument.find(":") != std::string::npos)
        {
            std::vector<std::string> tokens = Split(runArgument, ':');
            if (tokens.size() == 2)
            {
                data.put(tokens[0], tokens[1]);
            }
        }
    }

    return data;
}

int main(int argc, char* argv[])
{
    TCProcessor Proc;

    char transID[10] = { "123456" };
    Proc.transID = transID;

    if (Proc.assignPlatform("M"))
    {
        StringHash data = ProcessArguments(argc, argv);

        // from arguments
        //data.put("action", "sale");
        //data.put("mediatype", "mc");
        //data.put("store", "y");
        //data.put("billingid", "y");
        //data.put("cit", "y");
        //data.put("cardonfileid", "y");

        StringHash response;

        Proc.runTransaction(data, response);

        if (response.size() > 0)
        {
            std::cout << "main: request has CMI value: " << response.get("CMI") << "\n";
        }
    }
}
