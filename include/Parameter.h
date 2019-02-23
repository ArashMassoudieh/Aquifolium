#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>
#include <vector>

using namespace std;
struct Range
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
        void SetLocations(const vector<string> &loc) {_location = loc;}
        void AppendLocationQuan(const string &loc, const string &quan) {_location.push_back(loc); _quan.push_back(quan);}
        vector<string> GetLocations() {return _location;}
        void SetRange(double low, double high) {prior_range.low = low; prior_range.high=high;}
        Range GetRange() {return prior_range;}
        void SetPriorDistribution(const string &prior) {prior_distribution = prior; }
        void SetValue(const double &val) {value = val;}
        double GetValue() {return value;}
        string LastError() {return last_error;}
    protected:

    private:
        vector<string> _location;
        vector<string> _quan;
        Range prior_range;
        string prior_distribution;
        double value;
        string last_error;
};

#endif // PARAMETER_H
