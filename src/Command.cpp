#include "Command.h"
#include "Script.h"
#include <iostream>

using namespace std;

Command::Command(Script *parnt)
{
    parent = parnt;
}

Command::~Command()
{
    //dtor
}

Command::Command(const string &s, Script *parnt)
{
    parent = parnt;
    vector<string> firstlevelbreakup = split(s,';');
    {
        if (firstlevelbreakup.size()==0)
        {
            last_error = "Empty line!";
            validated = false;
            return;
        }
    }
    vector<string> maincommad = split(firstlevelbreakup[0],' ');
    if (tolower(maincommad[0])=="loadtemplate")
    {
        if (maincommad.size()!=1)
            {
                last_error = "Command 'loadtemplate' does not require an argument!";
                validated = false;
                return;
            }
        else
        {

        }
    }

    if (tolower(maincommad[0])=="create")
    {
        if (maincommad.size()!=2)
        {
            last_error = "Command 'create' requires one argument! ";
            validated = false;
            return;
        }
        else
        {
            keyword = tolower(maincommad[0]);
            arguments.push_back(maincommad[1]);
            validated = true;
        }
    }

    for (int i=1; i<firstlevelbreakup.size(); i++)
    {
        vector<string> properties = split(firstlevelbreakup[i],';');
        for (int j=0; j<properties.size(); j++)
        {
            vector<string> prop = split(properties[j],'=');
            if (prop.size()<2)
            {
                last_error = "Property '" + prop[0] + "' does not have a right hand side!";
                validated = false;
                return;
            }
            if (prop.size() == 0)
            {
                validated = false;
                return;
            }
            if (prop.size() == 2)
            {
                if (assignments.count(prop[0])==0)
                    assignments[prop[0]] = prop[1];
                else
                {
                    last_error = "In command '" + s + "': Property '" + prop[0] + "' has been already specified!";
                }
            }
        }
    }
}

Command::Command(const Command& other)
{
    keyword = other.keyword;
    arguments = other.arguments;
    assignments = other.assignments;
    last_error = other.last_error;
    validated = other.validated;
    parent = other.parent;
}

Command& Command::operator=(const Command& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    keyword = rhs.keyword;
    arguments = rhs.arguments;
    assignments = rhs.assignments;
    last_error = rhs.last_error;
    validated = rhs.validated;
    parent = rhs.parent;
    return *this;
}

System *Command::GetSystem()
{
    if (parent)
        return parent->GetSystem();
    else
        return nullptr;

}

bool Command::Execute(System *_sys)
{
    System *sys;
    if (_sys==nullptr)
    {
        if (parent->GetSystem()!=nullptr)
        {
            sys = parent->GetSystem();
        }
    }
    else
        sys = _sys;
    if (tolower(keyword) == "loadtemplate")
    {

    }
    if (tolower(keyword)=="create")
    {
        if (tolower(arguments[0])=="block")
        {

        }
    }
}

bool Command::Validate(System *sys)
{
    bool out = true;
    for (map<string,map<string,vector<string>>>::iterator it = parent->MustBeSpecified()->begin(); it!=parent->MustBeSpecified()->end(); it++)
    {
        for (map<string,vector<string>>::iterator it1 = it->second.begin(); it1!=it->second.end(); it++)
            for (int i=0; i<it1->second.size(); i++)
                cout<<it1->second[i];
    }
}
