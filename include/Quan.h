#ifndef QUAN_H
#define QUAN_H

#include "Expression.h"
#include "BTC.h"

class Block;
class Link;
class System;
class Object;

class Quan
{
    public:
        Quan();
        virtual ~Quan();
        Quan(const Quan& other);
        Quan& operator=(const Quan& other);
        enum class _type {constant, value, balance, expression, timeseries, global_quan};
        double CalcVal(Object *, const Expression::timing &tmg=Expression::timing::past);
        double CalcVal(const Expression::timing &tmg=Expression::timing::past);
        double GetVal(const Expression::timing &tmg=Expression::timing::past);
        string last_error;
        void SetType(const _type &t) {type = t;};
        _type GetType() {return type;}
        bool SetExpression(const string &E);
        bool SetVal(const double &v, const Expression::timing &tmg=Expression::timing::past);
        void SetCorrespondingFlowVar(const string &s);
        string GetCorrespondingFlowVar() {return corresponding_flow_quan;};
        void SetMassBalance(bool on);
        void SetParent(Object *);
        Quan *Corresponding_flow_variable;
		void Renew();
		void Update();
        void SetIncludeInOutput(bool x) {includeinoutput = x;};
        string GetName() {return _var_name;};
        bool IncludeInOutput() {return includeinoutput;};
		bool SetTimeSeries(string filename);
    protected:

    private:
        Expression _expression;
        CTimeSeries _timeseries;
        string _var_name;
        double _val;
        double _val_star;
        vector<double> _parameters;
        _type type;
        bool perform_mass_balance = false;
        string corresponding_flow_quan;
        Object *parent;
        bool includeinoutput = false;

};

#endif // QUAN_H
