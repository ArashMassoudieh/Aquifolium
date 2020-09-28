#ifndef QUAN_H
#define QUAN_H

#include "Expression.h"
#include "Rule.h"
#include "BTC.h"
#include <json/json.h>
#include "Condition.h"

#ifdef Q_version
#include <qjsonobject.h>
#endif

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
#ifdef Q_version
		Quan(QJsonObject& qjobject);
#endif // QT_version

        string GetStringValue() {return _string_value;}
        Quan& operator=(const Quan& other);
        enum class _type {constant, value, balance, expression, timeseries, prec_timeseries, global_quan, rule, source, string};
        enum class _role {none, copytoblocklinks, copytosources};
        double CalcVal(Object *, const Expression::timing &tmg=Expression::timing::past);
        double CalcVal(const Expression::timing &tmg=Expression::timing::past);
        double GetVal(const Expression::timing &tmg=Expression::timing::past);
		CTimeSeries* GetTimeSeries();
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
        Quan *Corresponding_flow_variable = nullptr;
		void Renew();
		void Update();
        void SetIncludeInOutput(bool x) {includeinoutput = x;}
        void SetEstimable(bool x) {estimable=x;}
        string GetName() {return _var_name;}
        CTimeSeries* TimeSeries();
        bool IncludeInOutput() {return includeinoutput;}
		bool SetTimeSeries(const string &filename, bool prec=false);
		string &Description() {return description;}
        string &Unit() {return unit;}
        string &DefaultUnit() {return default_unit;}
        string &Defaults() {return defaults;}
        string &Delegate() {return delegate;}
        bool Estimable() {return estimable;}
        string &Category() {return category;}
        string &Input() {return input;}
        string &Default() {return default_val;}
        bool &ExperimentDependent() {return experiment_dependent;}
        string &DescriptionCode() {return description_code;}
        string &Abbreviation() {return abbreviation;}
        string &WarningError() {return warning_error;}
        string &WarningMessage() {return warning_message;}
        Condition &Criteria () {return criteria;}
        string &InputType() {return input_type;}
        string ToString(int _tabs=1) const;
        bool &AskFromUser() {return ask_from_user;}
        bool WhenCopied() {
            if (role == _role::copytoblocklinks)
                return true;
            else {
                return false;
            }
        }
        void SetRole(const _role &r)
        {
            role = r;
        }
		void SetName(const string &name) {_var_name=name;}
		bool AppendError(const string &objectname, const string &cls, const string &funct, const string &description, const int &code);
        bool SetProperty(const string &val);
        string GetProperty();
		string SourceName() { return sourcename;}
		bool SetSourceName(const string& s) { sourcename = s; }
        string toCommand();
        void SetOutputItem(const string& s) { OutputItem = s; }
        string GetOutputItem() { return OutputItem; }
        void SetParameterAssignedTo(const string &s) {_parameterassignedto=s;}
        string GetParameterAssignedTo() {return _parameterassignedto;}
        bool Validate();
        bool HasCriteria() { if (criteria.Count() > 0) return true; else return false; }
        vector<string> GetAllRequieredStartingBlockProperties();
        vector<string> GetAllRequieredEndingBlockProperties();
        void Set_Value_Update(bool x) { value_star_updated = x; }
        bool ApplyLimit() { return applylimit; }
        bool isrigid() { return rigid; }
    protected:

    private:
        Expression _expression;
        Rule _rule;
        CTimeSeries _timeseries;
        Source *source = nullptr;
		string sourcename = ""; 
		string _var_name;
		string _string_value = ""; 
        double _val=0;
        double _val_star=0;
        bool value_star_updated = false; 
        vector<double> _parameters;
        _type type;
        bool perform_mass_balance = false;
        string corresponding_flow_quan;
        string corresponding_inflow_quan;
        Object *parent = nullptr;
        bool includeinoutput = false;
        bool estimable = false;
        string description;
        string unit;
        string default_unit;
        string default_val="";
        string input_type;
        string defaults;
        string delegate;
        string category;
        string input;
        bool experiment_dependent = false;
        string description_code;
        string abbreviation;
        Condition criteria;
        string warning_error;
        string warning_message;
        bool ask_from_user=false;
        _role role = _role::none;
        string OutputItem;
        string _parameterassignedto = "";
        bool applylimit = false; 
        bool rigid = false; 
};

string tostring(const Quan::_type &typ);


#endif // QUAN_H
