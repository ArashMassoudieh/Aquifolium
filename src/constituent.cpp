#include "constituent.h"

Constituent::Constituent(): Object::Object()
{
    //ctor
}

Constituent::~Constituent()
{
    //dtor
}

Constituent::Constituent(const Constituent& other):Object::Object(other)
{
    Object::operator=(other);
}

Constituent& Constituent::operator=(const Constituent& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    Object::operator=(rhs);
    return *this;
}



