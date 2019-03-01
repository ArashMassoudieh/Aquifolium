#include "Command.h"

Command::Command()
{
    //ctor
}

Command::~Command()
{
    //dtor
}

Command::Command(const Command& other)
{
    //copy ctor
}

Command& Command::operator=(const Command& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}
