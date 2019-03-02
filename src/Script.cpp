#include "Script.h"
#include <iostream>
#include <fstream>

Script::Script()
{
    //ctor
}

Script::~Script()
{
    //dtor
}

Script::Script(const string &filename)
{
    fstream file(filename);
    while (!file.eof())
    {
        string s;
        getline(file,s);
        Command command(s);
        if (command.Validate())
            commands.push_back(command);
        else
            errors.push_back(command.LastError());
    }
}
