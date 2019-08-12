#ifndef QUAN_H
#define QUAN_H

#include "Expression.h"
#include "Rule.h"
#include "BTC.h"
#include <json/json.h>

class Block;
class Link;
class System;
class Object;
class Source;


class Quan
{
    public:
        Quan();
        virtual ~Quan();
        Quan(const Quan& other);
        Quan(Json::ValueIterator &it);
        Quan& operator=(const Quan& other);
        enum class _type {constant, value, balance, expression, timeseries, global_quan, rule, source};
        double CalcVal(Object *, const Expression::timing &tmg=Expression::timing::past);
        double CalcVal(const Expression::timing &tmg=Expression::timing::past);
        double GetVal(const Expression::timing &tmg=Expression::timing::past);
        string last_error;
        void SetType(const _type &t) {type = t;}
        _type GetType() {return type;}
        Expression* GetExpression();
        Rule* GetRule();
        Source* GetSource();
        bool SetExpression(const string &E);
        bool SetRule(const string &R);
        bool SetVal(const double &v, const Expression::timing &tmg=Expression::timing::past);
        bool SetSource(const string &sourcename);
        void SetCorrespondingFlowVar(const string &s);
        string GetCorrespondingFlowVar()
        {
            return corresponding_flow_quan;
        }
        void SetCorrespondingInflowVar(const string &s);
        string GetCorrespondingInflowVar() {return corresponding_inflow_quan;}
        void SetMassBalance(bool on);
        void SetParent(Object *);
        Quan *Corresponding_flow_variable;
		void Renew();
		void Update();
        void SetIncludeInOutput(bool x) {includeinoutput = x;}
        string GetName() {return _var_name;}
        bool IncludeInOutput() {return includeinoutput;}
		bool SetTimeSeries(const string &filename);
		string &Description() {return description;}
        string &Unit() {return unit;}
        string &DefaultUnit() {return default_unit;}
        string &Defaults() {return defaults;}
        string &Delegate() {return delegate;}
        string &Category() {return category;}
        string &Input() {return input;}
        string &Default() {return default_val;}
        bool &ExperimentDependent() {return experiment_dependent;}
        string &DescriptionCode() {return description_code;}
        string &Abbreviation() {return abbreviation;}
        string &WarningError() {return warning_error;}
        string &WarningMessage() {return warning_message;}
        string &Criteria () {return criteria;}
        string &InputType() {return input_type;}
		string ToString(int _tabs=1);
        bool &AskFromUser() {return ask_from_user;}
		void SetName(const string &name) {_var_name=name;}
		bool AppendError(const string &objectname, const string &cls, const string &funct, const string &description, const int &code);
        bool SetProperty(const string &val);
    protected:

    private:
        Expression _expression;
        Rule _rule;
        CTimeSeries _timeseries;
        Source *source;
        string _var_name;
        double _val;
        double _val_star;
        vector<double> _parameters;
        _type type;
        bool perform_mass_balance = false;
        string corresponding_flow_quan;
        string corresponding_inflow_quan;
        Object *parent = nullptr;
        bool includeinoutput = false;
        string description;
        string unit;
        string default_unit;
        string default_val;
        string input_type;
        string defaults;
        string delegate;
        string category;
        string input;
        bool experiment_dependent = false;
        string description_code;
        string abbreviation;
        string criteria;
        string warning_error;
        string warning_message;
        bool ask_from_user=false;
};

string tostring(const Quan::_type &typ);


#endif // QUAN_H
