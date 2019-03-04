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
        Script(const string &filename, System *sys = nullptr);
        System* CreateSystem();
        virtual ~Script();
        Command* operator[](int i) {return &commands[i];}
        System *GetSystem() {return system;}
        void FillMustBeSpecified();
        map<string, map<string, vector<string>>> *MustBeSpecified()
        {
            return &mustbespecifiedatcreation;
        }
        void Append(const Command &c);
    protected:

    private:
        vector<Command> commands;
        vector<string> errors;
        System *system;
        map<string, map<string, vector<string>>> mustbespecifiedatcreation;
};

#endif // SCRIPT_H
