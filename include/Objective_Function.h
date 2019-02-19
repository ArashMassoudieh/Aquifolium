#ifndef OBJECTIVE_FUNCTION_H
#define OBJECTIVE_FUNCTION_H

#include "Expression.h"
#include "BTC.h"

class System;

class Objective_Function
{
    public:
        Objective_Function();
        Objective_Function(System *_system) ;
        virtual ~Objective_Function();
        Objective_Function(const Objective_Function& other);
        Objective_Function& operator=(const Objective_Function& other);
        double GetValue(const Expression::timing &tmg = Expression::timing::present); //return the integral of the time series of the calculated values
        void append_value(double t, double val); //append a value to the stored time-series;
        void append_value(double t); //append the evaluated expression value to the stored time-series;
        void SetSystem(System *_system) {system = _system;}
        string GetLastError() {return lasterror;}
        void SetLastError(const string &lerror) {lasterror = lerror;}

    protected:

    private:
        string location; // location at which the objective function will be evaluated
        Expression expression; // Function
        CTimeSeries stored_time_series; // Stored time series values;
        System *system; // pointer to the system the objective function is evaluated at
        string lasterror; //the last error that occurred
};

#endif // OBJECTIVE_FUNCTION_H
