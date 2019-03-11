#include "Command.h"
#include "Script.h"
#include <iostream>
#include "System.h"

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
    vector<string> maincommand = split(firstlevelbreakup[0],' ');
    if (tolower(maincommand[0])=="loadtemplate" || tolower(maincommand[0])=="setasparameter" || tolower(maincommand[0])=="echo" || tolower(maincommand[0])=="setvalue")
    {
        if (maincommand.size()!=1)
            {
                last_error = "Command " + maincommand[0] + " does not require an argument!";
                validated = false;
                return;
            }
        else
        {
            keyword = maincommand[0];
            validated = true;

        }
    }

    if (tolower(maincommand[0])=="create")
    {
        if (maincommand.size()!=2)
        {
            last_error = "Command 'create' requires one argument! ";
            validated = false;
            return;
        }
        else
        {
            keyword = tolower(maincommand[0]);
            arguments.push_back(maincommand[1]);
            validated = true;
        }
    }

    for (int i=1; i<firstlevelbreakup.size(); i++)
    {
        vector<string> properties = split(firstlevelbreakup[i],',');
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
        if (Validate())
        {
            if (sys->GetQuanTemplate(assignments["filename"]))
                return true;
            else
            {
                last_error = "File '" + assignments["filename"] + "' was not found!";
            }
        }
        else
            return false;
    }

    if (tolower(keyword) == "echo")
    {
        if (assignments.count("feature")==1 && assignments.count("quantity")==0)
        {
            sys->errorhandler.Append("", "Command", "Execute","In echo command 'quantity' must be specified when feature property is needed.", 7008);
        }
        if (Validate())
        {
            if (assignments.count("quantity")==0)
            {
                if (assignments.count("object")==1)
                {
                    return sys->Echo(assignments["object"]);
                }
                else
                {
                    sys->errorhandler.Append("", "Command", "Execute", "In echo command, object must be indicated",7001);
                    return false;
                }
            }
            else
            {
                if (assignments.count("object")==1)
                {
                    if (assignments.count("feature")==1)
                    {
                        return sys->Echo(assignments["object"],assignments["quantity"],assignments["feature"]);

                    }
                    else
                    {
                        return sys->Echo(assignments["object"],assignments["quantity"]);
                    }
                }
                else
                {
                    sys->errorhandler.Append("", "Command", "Execute", "In echo command, object must be indicated",7001);
                    return false;
                }
            }
        }
        else
            return false;
    }

    if (tolower(keyword)=="setvalue")
    {
        if (assignments.count("object")==0)
        {
            sys->errorhandler.Append("", "Command", "Execute", "In the 'setvalue' command, 'object' must be indicated",7010);
            return false;
        }
        if (assignments.count("quantity")==0)
        {
            sys->errorhandler.Append("", "Command", "Execute", "In the 'setvalue' command, 'quantity' must be indicated",7011);
            return false;
        }
        if (assignments.count("value")==0)
        {
            sys->errorhandler.Append("", "Command", "Execute", "In the 'setvalue' command, 'value' must be indicated",7012);
            return false;
        }
        if (sys->object(assignments["object"])==nullptr && sys->parameter(assignments["object"])==nullptr)
        {
            sys->errorhandler.Append("","Command", "Execute", "Object or parameter '" + assignments["object"] + "' was not found.", 7013);
            return false;
        }
        if (sys->object(assignments["object"])!=nullptr)
        {
            if (!sys->object(assignments["object"])->HasQuantity(assignments["quantity"]))
            {
                sys->errorhandler.Append("","Command","Execute","Object '" + assignments["object"] + "' has no quantity '" + assignments["quantity"] + "'", 7014);
                return false;
            }
            else
            {
                return sys->object(assignments["object"])->Variable(assignments["quantity"])->SetVal(atof(assignments["value"]));
            }
        }
        else if (sys->parameter(assignments["object"])!=nullptr)
        {
            if (!sys->parameter(assignments["object"])->HasQuantity(assignments["quantity"]))
            {
                sys->errorhandler.Append("","Command","Execute","Parameter '" + assignments["object"] + "' has no quantity '" + assignments["quantity"] + "'", 7014);
                return false;
            }
            else
            {
                return sys->parameter(assignments["object"])->SetProperty(assignments["quantity"],assignments["value"]);
            }
        }
    }

    if (tolower(keyword) == "setasparameter")
    {
        if (Validate())
        {
            sys->SetAsParameter(assignments["object"],assignments["quantity"],assignments["parametername"]);
            return true;
        }
        else
            return false;
    }


    if (tolower(keyword)=="create")
    {
        if (tolower(arguments[0])=="block")
        {
            if (Validate())
            {
                Block B;
                B.SetName(assignments["name"]);
                B.SetType(assignments["type"]);
                sys->AddBlock(B);
                for (map<string,string>::iterator it=assignments.begin(); it!=assignments.end(); it++)
                {
                    if (it->first!="name" && it->first!="type" && it->first!="to" && it->first!="from")
                        sys->block(assignments["name"])->SetProperty(it->first,it->second);
                }
                return true;
            }
            else
                return false;
        }
        if (tolower(arguments[0])=="link")
        {
            if (Validate())
            {
                Link L;
                L.SetName(assignments["name"]);
                L.SetType(assignments["type"]);

                sys->AddLink(L,assignments["from"],assignments["to"]);
                for (map<string,string>::iterator it=assignments.begin(); it!=assignments.end(); it++)
                {
                    if (it->first!="name" && it->first!="type" && it->first!="to" && it->first!="from")
                        sys->link(assignments["name"])->SetProperty(it->first,it->second);
                }
                return true;
            }
            else
                return false;
        }
        if (tolower(arguments[0])=="parameter")
        {
            if (Validate())
            {
                Parameter P;

                sys->AppendParameter(assignments["name"],atof(assignments["low"]),atof(assignments["high"]));
                for (map<string,string>::iterator it=assignments.begin(); it!=assignments.end(); it++)
                {
                    if (it->first!="name" && it->first!="type" && it->first!="to" && it->first!="from")
                    {
                        if (!sys->parameter(assignments["name"])->SetProperty(it->first,it->second))
                            last_error = "Parameter does not have a '" + it->first + "' + property!";
                    }
                }
                return true;
            }
            else
                return false;

        }
        return true;
    }

}

bool Command::Validate(System *sys)
{
    bool out = true;
    if (parent->MustBeSpecified()->count(keyword)==0)
    {
        last_error = "Keyword '" + keyword + "' is not recognized!";
        if (sys) sys->errorhandler.Append("","Command","Validate",last_error,5001);
        return false;
    }
    if (arguments.size()>0)
        if (parent->MustBeSpecified()->at(keyword).count(arguments[0])==0)
        {
            last_error = "Argument '" + arguments[0] + "' is not recognized for keyword '" + keyword + "'";
            if (sys) sys->errorhandler.Append("","Command","Validate",last_error,5002);
            return false;
        }
    if (arguments.size()>0)
        for (int i=0; i<parent->MustBeSpecified()->at(keyword)[arguments[0]].size(); i++)
            if (assignments.count(parent->MustBeSpecified()->at(keyword)[arguments[0]][i])==0)
            {
                last_error = "'" + parent->MustBeSpecified()->at(keyword)[arguments[0]][i] + "' must be specified when " + keyword + "ing a " + arguments[0] + "'";
                if (sys) sys->errorhandler.Append("","Command","Validate",last_error,5003);
                return false;
            }

    for (int i=0; i<parent->MustBeSpecified()->at(keyword)["*"].size(); i++)
        if (assignments.count(parent->MustBeSpecified()->at(keyword)["*"][i])==0)
        {
            last_error = "'" + parent->MustBeSpecified()->at(keyword)["*"][i] + "' must be specified when " + keyword + "ing a " + arguments[0] + "'";
            if (sys) sys->errorhandler.Append("","Command","Validate",last_error,5003);
            return false;
        }

    return true;
}

void Command::SetParent (Script *scr)
{
    parent = scr;
}
