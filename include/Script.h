#ifndef SCRIPT_H
#define SCRIPT_H


#include "Command.h"
#include <vector>

class System;

using namespace std;

class Script
{
    public:
        Script();
        Script(const string &filename);
        virtual ~Script();
        Command* operator[](int i) {return &commands[i];}
        System *GetSystem() {return system;}
    protected:

    private:
        vector<Command> commands;
        vector<string> errors;
        System *system;
};

#endif // SCRIPT_H
