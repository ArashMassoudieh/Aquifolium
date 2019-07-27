#ifndef SOURCE_H
#define SOURCE_H

#include "Object.h"
#include "Quan.h"

using namespace std;

enum class _type {timeseries, constant};

class System;
class Source: public Object
{
    public:
        Source();
        virtual ~Source();
        Source(System *_parent);
        Source(const Source& other);
        Source& operator=(const Source& rhs);
        double GetValue(Object *obj);
    protected:
        Quan timeseries;
        Quan coefficient;
        _type type;
    private:
};

#endif // SOURCE_H
