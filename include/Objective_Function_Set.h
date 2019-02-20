#ifndef OBJECTIVE_FUNCTION_SET_H
#define OBJECTIVE_FUNCTION_SET_H

#include "Objective_Function.h"
#include <map>

struct obj_funct_weight
{
    Objective_Function obj_funct;
    double weight = 1;
};

class Objective_Function_Set
{
    public:
        Objective_Function_Set();
        virtual ~Objective_Function_Set();
        Objective_Function_Set(const Objective_Function_Set& other);
        Objective_Function_Set& operator=(const Objective_Function_Set& other);
        void Append(const string &name, const Objective_Function &o_func, double weight = 1);
        obj_funct_weight* operator[](string name);
        double Calculate();
        void Update(double t);
        string LastError() {return lasterror;}
    protected:

    private:
        map<string,obj_funct_weight> objectivefunctions;
        string lasterror;


};

#endif // OBJECTIVE_FUNCTION_SET_H
