#ifndef PARAMETER_SET_H
#define PARAMETER_SET_H

#include "Parameter.h"
#include <map>

class System;

class Parameter_Set
{
    public:
        Parameter_Set();
        virtual ~Parameter_Set();
        Parameter_Set(const Parameter_Set& other);
        Parameter_Set& operator=(const Parameter_Set& other);
        void Append(const string &name, const Parameter &param);
        Parameter* operator[](string name);
        Parameter* operator[](int i);
        string LastError() {return lasterror;}
        bool Contains(const string &name) {if (parameters.count(name)==0) return false; else return true;}
        map<string, Parameter>::iterator begin() {return parameters.begin();}
        map<string, Parameter>::iterator end() {return parameters.end();}
        bool ApplyParameters(System *system);
        unsigned int size() {return parameters.size();}
        string getKeyAtIndex (int index);
    protected:

    private:
        map<string,Parameter> parameters;
        string lasterror;
};

#endif // PARAMETER_SET_H
