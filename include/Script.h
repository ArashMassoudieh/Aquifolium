#ifndef SCRIPT_H
#define SCRIPT_H


#include "Command.h"
#include <vector>
#include "ErrorHandler.h"
#include "GA.h"

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
        void SetGA(CGA<System> *G) {GA = G;}
        CGA<System> *GetGA() {return GA;}
    protected:

    private:
        vector<Command> commands;
        vector<string> errors;
        System *system;
        map<string, map<string, vector<string>>> mustbespecifiedatcreation;
        ErrorHandler errorhandler;
        CGA<System> *GA = nullptr;
};

#endif // SCRIPT_H
