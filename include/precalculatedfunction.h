#ifndef PRECALCULATEDFUNCTION_H
#define PRECALCULATEDFUNCTION_H

#include "BTC.h"

class PreCalculatedFunction : public CTimeSeries
{
public:
    PreCalculatedFunction();
    PreCalculatedFunction(const PreCalculatedFunction& other);
    PreCalculatedFunction& operator=(const PreCalculatedFunction& rhs);
    virtual ~PreCalculatedFunction();
    double xmax() const {return x_max;}
    double xmin() const {return x_min;}
    bool setminmax(const double &xmin, const double &xmax)
    {
        if (x_max<=x_min) return false;
        x_min = xmin;
        x_max = xmax;
        return true;
    }
    void SetLogarithmic(bool x) {logarithmic = x;}
    bool Logarithmic() const {return logarithmic;}
    bool SetIndependentVariable(const string &indvar) {indepenentvariable = indvar;return true;}
    string IndependentVariable() const {return indepenentvariable;}
private:
    string indepenentvariable;
    bool logarithmic=false;
    double x_min;
    double x_max;
};

#endif // PRECALCULATEDFUNCTION_H
