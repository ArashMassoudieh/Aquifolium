#include "Command.h"
#include "Script.h"

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
                assignment assn;
                assn.variable = prop[0];
                assn.value = prop[1];
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
