#include "Script.h"
#include <iostream>
#include <fstream>
#include "System.h"

Script::Script()
{
   FillMustBeSpecified();
}

Script::~Script()
{
    if (system) delete system;
}

Script::Script(const string &filename, System *sys)
{
    FillMustBeSpecified();
    if (system!=nullptr)
        system = sys;
    else
        system = new System();
    fstream file(filename);
    while (!file.eof())
    {
        string s;
        getline(file,s);
        Command command(s);
        if (command.Syntax())
            Append(command);
        else
            errors.push_back(command.LastError());
    }
}

System* Script::CreateSystem()
{
    for (int i=0; i<commands.size(); i++)
    {
        if (!commands[i].Execute(system))
        {
            system->errorhandler.Append("","Script","CreateSystem",commands[i].LastError(),5001);
            errors.push_back(commands[i].LastError());
        }
    }
    return system;
}

void Script::FillMustBeSpecified()
{
    mustbespecifiedatcreation["create"] = map<string, vector<string>>();
        mustbespecifiedatcreation["create"]["block"] = vector<string>();
            mustbespecifiedatcreation["create"]["block"].push_back("name");
            mustbespecifiedatcreation["create"]["block"].push_back("type");
        mustbespecifiedatcreation["create"]["link"] = vector<string>();
            mustbespecifiedatcreation["create"]["link"].push_back("name");
            mustbespecifiedatcreation["create"]["link"].push_back("type");
            mustbespecifiedatcreation["create"]["link"].push_back("from");
            mustbespecifiedatcreation["create"]["link"].push_back("to");
    mustbespecifiedatcreation["loadtemplate"] = map<string, vector<string>>();
        mustbespecifiedatcreation["loadtemplate"]["*"] = vector<string>();
            mustbespecifiedatcreation["loadtemplate"]["*"].push_back("filename");
}

void Script::Append(const Command &c)
{
    commands.push_back(c);
    commands[commands.size()-1].SetParent(this);
}
