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
    double xmax() {return t[n-1];}
    double xmin() {return t[0];}
    bool logarithmic=false;

};

#endif // PRECALCULATEDFUNCTION_H
