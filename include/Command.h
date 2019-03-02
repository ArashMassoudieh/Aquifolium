#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>
#include "Expression.h"

using namespace std;


struct assignment
{
    string variable;
    string value;
};

class System;
class Script;


class Command
{
    public:
        Command(Script *parnt = nullptr);
        virtual ~Command();
        Command(const Command& other);
        Command& operator=(const Command& other);
        Command(const string &s, Script *parnt = nullptr);
        bool Validate() {return validated;}
        System *GetSystem();
        string LastError() {return last_error;}
    protected:

    private:
        string keyword;
        vector<string> arguments;
        vector<assignment> assignments;
        string last_error;
        bool validated = false;
        Script *parent;
};

#endif // COMMAND_H
