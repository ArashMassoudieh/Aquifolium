#ifndef SCRIPT_H
#define SCRIPT_H


#include "Command.h"
#include <vector>

using namespace std;

class Script
{
    public:
        Script();
        virtual ~Script();
        Command* operator[](int i) {return &commands[i];}
    protected:

    private:
        vector<Command> commands;
};

#endif // SCRIPT_H
