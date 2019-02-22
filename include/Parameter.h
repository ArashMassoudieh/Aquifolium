#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>

using namespace std;
struct range
{
    double low = 0;
    double high = 0;
};

class Parameter
{
    public:
        Parameter();
        virtual ~Parameter();
        Parameter(const Parameter& other);
        Parameter& operator=(const Parameter& other);
        void SetLocation(const string &loc) {_location = loc;}
        string GetLocation() {return _location;}
        void SetRange(double low, double high) {prior_range.low = low; prior_range.high=high;}
        range GetRange() {return prior_range;}
        void SetValue(const double &val) {value = val;}
        double GetValue() {return value;}
        string LastError() {return last_error;}
    protected:

    private:
        string _location;
        range prior_range;
        string prior_distribution;
        double value;
        string last_error;
};

#endif // PARAMETER_H
