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
        vector<string> GetQuans() {return _quan;}
        void SetRange(double low, double high) {prior_range.low = low; prior_range.high=high;}
        Range GetRange() {return prior_range;}
        void SetPriorDistribution(const string &prior) {prior_distribution = prior; }
        string GetPriorDistribution() {return prior_distribution; }
        void SetValue(const double &val) {value = val;}
        double GetValue() {return value;}
        string LastError() {return last_error;}
        bool SetProperty(const string &s, const string &val)
        {
            if (s=="low") {prior_range.low = atof(val.c_str()); return true;}
            if (s=="high") {prior_range.high = atof(val.c_str()); return true;}
            if (s=="value") {value = atof(val.c_str()); return true;}
            if (s=="prior_distribution") {prior_distribution = val;return true;}
            return false;
        }
        string toString(int _tabs = 0);
        bool SetName(const string &nm) {name = nm; return true;}
        string GetName() {return name;}
        bool HasQuantity(const string &qntty);
        string Variable(const string &qntty);


    protected:

    private:
        vector<string> _location;
        vector<string> _quan;
        Range prior_range;
        string prior_distribution;
        double value;
        string last_error;
        string name;
};

#endif // PARAMETER_H
