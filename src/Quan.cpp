#include "Quan.h"
#include "Block.h"
#include "Link.h"
#include "System.h"
#include "Precipitation.h"


Quan::Quan()
{
    //ctor
}

Quan::Quan(Json::ValueIterator &it)
{

    SetName(it.key().asString());
    if ((*it)["type"].asString()=="balance")
    {
        SetType(Quan::_type::balance);
        SetCorrespondingFlowVar((*it)["flow"].asString());
        SetCorrespondingInflowVar((*it)["inflow"].asString());
    }
    if ((*it)["type"].asString()=="constant")
        SetType(Quan::_type::constant);
    if ((*it)["type"].asString()=="expression")
    {
        SetType(Quan::_type::expression);
        SetExpression((*it)["expression"].asString());
    }
    if ((*it)["type"].asString()=="rule")
    {
        SetType(Quan::_type::rule);
        for (Json::ValueIterator itrule=(*it)["rule"].begin(); itrule!=(*it)["rule"].end(); ++itrule)
        {
            _condplusresult Rle;
            GetRule()->Append(itrule.key().asString(),itrule->asString());
        }
    }

    if ((*it)["type"].asString()=="global")
        SetType(Quan::_type::global_quan);
    if ((*it)["type"].asString()=="timeseries")
        SetType(Quan::_type::timeseries);
	if ((*it)["type"].asString() == "timeseries_prec")
		SetType(Quan::_type::prec_timeseries);
    if ((*it)["type"].asString()=="source")
        SetType(Quan::_type::source);
    if ((*it)["type"].asString()=="value")
        SetType(Quan::_type::value);
	if ((*it)["type"].asString() == "string")
		SetType(Quan::_type::string);
    if (it->isMember("includeinoutput"))
    {
        if ((*it)["includeinoutput"].asString()=="true")
            SetIncludeInOutput(true);
        else
            SetIncludeInOutput(false);
    }
    else
        SetIncludeInOutput(false);
    if (it->isMember("description"))
    {
        Description() = (*it)["description"].asString();
    }

    if (it->isMember("unit"))
        Unit() = (*it)["unit"].asString();

    if (it->isMember("default_unit"))
        DefaultUnit() = (*it)["default_unit"].asString();

    if (it->isMember("default"))
        Default() = (*it)["default"].asString();

    if (it->isMember("delegate"))
        Delegate() = (*it)["delegate"].asString();

    if (it->isMember("category"))
        Category() = (*it)["category"].asString();

    if (it->isMember("input"))
        Input() = (*it)["input"].asString();

    if (it->isMember("experiment_dependent"))
    {   if ((*it)["experiment_dependent"].asString()=="Yes")
            ExperimentDependent() = true;
        else
            ExperimentDependent() = false;

    }

    if (it->isMember("description_code"))
        DescriptionCode() = (*it)["description_code"].asString();

    if (it->isMember("criteria"))
        Criteria() = (*it)["criteria"].asString();

    if (it->isMember("warningerror"))
        WarningError() = (*it)["warningerror"].asString();

    if (it->isMember("warningmessage"))
        WarningMessage() = (*it)["warningmessage"].asString();

    if (it->isMember("inputtype"))
        InputType() = (*it)["inputtype"].asString();

    if (it->isMember("ask_user"))
    {    if (aquiutils::tolower((*it)["ask_user"].asString())=="true")
            AskFromUser() = true;
    }
    else
       AskFromUser() = false;

}

Quan::~Quan()
{
    //dtor
}

Quan::Quan(const Quan& other)
{
    _expression = other._expression;
    _timeseries = other._timeseries;
	_string_value = other._string_value; 
    _rule = other._rule;
	sourcename = other.sourcename;
    _var_name = other._var_name;
    _val = other._val;
    _val_star = other._val_star;
    _parameters = other._parameters;
    type = other.type;
	corresponding_flow_quan = other.corresponding_flow_quan;
	corresponding_inflow_quan = other.corresponding_inflow_quan;
	includeinoutput = other.includeinoutput;
	description = other.description;
    unit = other.unit;
    default_unit = other.default_unit;
    default_val = other.default_val;
    input_type = other.input_type;
    defaults = other.defaults;
    delegate = other.delegate;
    category = other.category;
    input = other.input;
    experiment_dependent = other.experiment_dependent;
    description_code = other.description_code;
    abbreviation = other.abbreviation;
    criteria = other.criteria;
    warning_error = other.warning_error;
    warning_message = other.warning_message;
    ask_from_user = other.ask_from_user;
	//parent = other.parent;
}

Quan& Quan::operator=(const Quan& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    _expression = rhs._expression;
    _timeseries = rhs._timeseries;
    _rule = rhs._rule;
    _var_name = rhs._var_name;
	_string_value = rhs._string_value;
    _val = rhs._val;
    _val_star = rhs._val_star;
    _parameters = rhs._parameters;
	sourcename = rhs.sourcename;
    type = rhs.type;
	corresponding_flow_quan = rhs.corresponding_flow_quan;
	corresponding_inflow_quan = rhs.corresponding_inflow_quan;
	includeinoutput = rhs.includeinoutput;
	description = rhs.description;
    unit = rhs.unit;
    default_unit = rhs.default_unit;
    default_val = rhs.default_val;
    input_type = rhs.input_type;
    defaults = rhs.defaults;
    delegate = rhs.delegate;
    category = rhs.category;
    input = rhs.input;
    experiment_dependent = rhs.experiment_dependent;
    description_code = rhs.description_code;
    abbreviation = rhs.abbreviation;
    criteria = rhs.criteria;
    warning_error = rhs.warning_error;
    warning_message = rhs.warning_message;
    ask_from_user = rhs.ask_from_user;
    //parent = rhs.parent;
    return *this;
}

double Quan::CalcVal(Object *block, const Expression::timing &tmg)
{
    if (type == _type::constant)
        return _val;
    if (type == _type::expression)
        return _expression.calc(block,tmg);
    if (type == _type::rule)
        return _rule.calc(block,tmg);
    if (type == _type::timeseries)
    {
        if (_timeseries.n>0)
            return _timeseries.interpol(block->GetParent()->GetTime());
        else
            return 0;
    }
    if (type == _type::value)
        return _val;
    if (type == _type::source)
    {
        if (source!=nullptr)
            return source->GetValue(block);
        else
            return 0;
    }
    last_error = "Quantity cannot be evaluated";
    return 0;
}

double Quan::GetVal(const Expression::timing &tmg)
{
    if (tmg==Expression::timing::past)
        return _val;
    else
        return _val_star;
}

double Quan::CalcVal(const Expression::timing &tmg)
{

    if (type == _type::constant)
    {
        if (tmg==Expression::timing::past)
            return _val;
        else
            return _val_star;
    }
    if (type == _type::expression)
        return _expression.calc(parent,tmg);
    if (type == _type::rule)
        return _rule.calc(parent,tmg);

    if (type == _type::timeseries || type == _type::prec_timeseries)
    {
        if (_timeseries.n>0)
            return _timeseries.interpol(parent->GetParent()->GetTime());
        else
            return 0;
    }
    if (type == _type::value)
    {
        if (tmg==Expression::timing::past)
            return _val;
        else
            return _val_star;
    }
    if (type == _type::balance)
    {
        if (tmg==Expression::timing::past)
            return _val;
        else
            return _val_star;
    }
    if (type == _type::source)
    {
        if (source!=nullptr)
            return source->GetValue(parent);
        else
            return 0;
    }
    last_error = "Quantity cannot be evaluated";
    return 0;
}

bool Quan::SetExpression(const string &E)
{
    _expression = E;
	return true;
}

bool Quan::SetRule(const string &R)
{
    _expression = R;
	return true;
}

string tostring(const Quan::_type &typ)
{
    if (typ==Quan::_type::balance) return "Balance";
    if (typ==Quan::_type::constant) return "Constant";
    if (typ==Quan::_type::expression) return "Expression";
    if (typ==Quan::_type::global_quan) return "Global";
    if (typ==Quan::_type::rule) return "Rule";
    if (typ==Quan::_type::timeseries) return "TimeSeries";
    if (typ==Quan::_type::value) return "Value";
    if (typ==Quan::_type::source) return "Source";
    return "";
}


bool Quan::SetVal(const double &v, const Expression::timing &tmg)
{
    if (tmg == Expression::timing::past)
        _val = v;
    else if (tmg == Expression::timing::present)
        _val_star = v;
    else if (tmg == Expression::timing::both)
    {
        _val = v;
        _val_star = v;
    }
	return true;
}

Expression* Quan::GetExpression()
{
    return &_expression;
}

Rule* Quan::GetRule()
{
    return &_rule;
}

Source* Quan::GetSource()
{
    return source;
}


string Quan::ToString(int _tabs)
{
    string out = aquiutils::tabs(_tabs) + _var_name + ":\n";
    out += aquiutils::tabs(_tabs) + "{\n";
    if (type==_type::constant)
        out += aquiutils::tabs(_tabs+1) + "type: constant\n";
    if (type==_type::balance)
        out += aquiutils::tabs(_tabs+1) + "type: balance\n";
    if (type==_type::expression)
        out += aquiutils::tabs(_tabs+1) + "type: expression\n";
    if (type==_type::constant)
        out += aquiutils::tabs(_tabs+1) + "type: constant\n";
    if (type==_type::global_quan)
        out += aquiutils::tabs(_tabs+1) + "type: global_quantity\n";
    if (type==_type::timeseries)
        out += aquiutils::tabs(_tabs+1) + "type: time_series\n";
    if (type==_type::value)
        out += aquiutils::tabs(_tabs+1) + "type: value\n";

    if (type==_type::expression)
        out += aquiutils::tabs(_tabs+1) + "expression: " + _expression.ToString() + "\n";

    if (type==_type::rule)
        out += aquiutils::tabs(_tabs+1) + "rule: " + _rule.ToString(_tabs) + "\n";

	if (type == _type::source)
	{
		if (source==nullptr)
			out += aquiutils::tabs(_tabs + 1) + "source: " + source->GetName() + "\n";
		else
			out += aquiutils::tabs(_tabs + 1) + "source: " + sourcename + "\n";

	}


    out += aquiutils::tabs(_tabs+1) + "val: ";
    out += aquiutils::numbertostring(_val);
    out += string("\n");
    out += aquiutils::tabs(_tabs+1) + "val*:";
    out += aquiutils::numbertostring(_val_star);
    out += string("\n");
    out += aquiutils::tabs(_tabs) + "}";
    return out;
}

void Quan::SetCorrespondingFlowVar(const string &s)
{
    corresponding_flow_quan = s;
}

void Quan::SetCorrespondingInflowVar(const string &s)
{
    corresponding_inflow_quan = s;
}



void Quan::SetMassBalance(bool on)
{
    perform_mass_balance = on;
}

void Quan::SetParent(Object *o)
{
    parent = o;
}

void Quan::Renew()
{
	_val_star = _val;
}

void Quan::Update()
{
	_val = _val_star;
}


CTimeSeries* Quan::TimeSeries()
{
    if (_timeseries.C.size()!=0)
        return &_timeseries;
    else
        return nullptr;
}

bool Quan::SetTimeSeries(const string &filename, bool prec)
{
	if (!prec)
	{
		_timeseries.readfile(filename);
		if (_timeseries.file_not_found)
		{
			AppendError(GetName(), "Quan", "SetTimeSeries", filename + " was not found!", 3001);
			return false;
		}
		else
			return true;
	}
	else
	{
		CPrecipitation Prec;
		if (!Prec.isFileValid(filename))
		{
			AppendError(GetName(), "Quan", "SetTimeSeries", filename + " was not is not a valid precipitation file", 3023);
			return false;
		}
		else
		{
			Prec.getfromfile(filename);
			_timeseries = Prec.getflow(1).BTC[0];
			return true;
		}
	}
}

bool Quan::SetSource(const string &sourcename)
{
	if (parent->GetParent()->source(sourcename))
	{
	    source = parent->GetParent()->source(sourcename);
        return true;
	}
	else
	{
		AppendError(GetName(),"Quan", "Source", sourcename + " was not found!", 3062);
		return false;
	}

}


string Quan::GetProperty()
{
    if (type == _type::balance || type== _type::constant || type==_type::global_quan || type==_type::value)
        return aquiutils::numbertostring(GetVal(Expression::timing::present));
    if (type == _type::timeseries)
    {
        return _timeseries.filename;
    }
    if (type == _type::prec_timeseries)
    {
        return _timeseries.filename;
    }
    else if (type == _type::source)
    {
        return sourcename;
    }
	else if (type == _type::string)
	{
		return _string_value;
	}
    return "";

}

bool Quan::SetProperty(const string &val)
{
    if (type == _type::balance || type== _type::constant || type==_type::global_quan || type==_type::value)
        return SetVal(aquiutils::atof(val),Expression::timing::both);
    if (type == _type::timeseries)
    {
        if (parent->Parent()->InputPath() != "")
            return SetTimeSeries(parent->Parent()->InputPath() + val);
        else
            return SetTimeSeries(val);
    }
	if (type == _type::prec_timeseries)
	{
		if (parent->Parent()->InputPath() != "")
			return SetTimeSeries(parent->Parent()->InputPath() + val,true);
		else
			return SetTimeSeries(val,true);
	}
    if (type == _type::source)
    {
		sourcename = val; 
		return SetSource(val);

    }
    if (type == _type::expression || type== _type::rule)
    {
        AppendError(GetName(),"Quan","SetProperty","Expression or rule cannot be set during runtime", 3011);
        return false;
    }
	if (type == _type::string)
	{
		_string_value = val; 
	}

    return true;
}

bool Quan::AppendError(const string &objectname, const string &cls, const string &funct, const string &description, const int &code)
{
    if (!parent)
        return false;
    if (!parent->Parent())
        return false;
    parent->Parent()->errorhandler.Append(objectname, cls, funct, description, code);
    return true;
}
