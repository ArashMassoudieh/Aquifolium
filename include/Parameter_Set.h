#ifndef PARAMETER_SET_H
#define PARAMETER_SET_H

#include "Parameter.h"
#include <map>

class Parameter_Set
{
    public:
        Parameter_Set();
        virtual ~Parameter_Set();
        Parameter_Set(const Parameter_Set& other);
        Parameter_Set& operator=(const Parameter_Set& other);
        void Append(const string &name, const Parameter &param);
        Parameter* operator[](string name);
        string LastError() {return lasterror;}
    protected:

    private:
        map<string,Parameter> parameters;
        string lasterror;
};

#endif // PARAMETER_SET_H
