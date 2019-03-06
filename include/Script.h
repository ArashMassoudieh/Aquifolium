#ifndef SCRIPT_H
#define SCRIPT_H


#include "Command.h"
#include <vector>
#include "ErrorHandler.h"

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
        ErrorHandler errorhandler;
};

#endif // SCRIPT_H
