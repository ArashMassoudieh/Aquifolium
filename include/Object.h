#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <map>
#include "Quan.h"
#include "MetaModel.h"
#include <qdebug.h>
#include "ErrorHandler.h"

using namespace std;

class Object
{
    public:
        Object();
        virtual ~Object();
        Object(const Object& other);
        Object& operator=(const Object& other);
        double CalcVal(const string& s, const Expression::timing &tmg=Expression::timing::past);
        double GetVal(const string& s, const Expression::timing &tmg=Expression::timing::past, bool limit=false);
        bool AddQnantity(const string &name,const Quan &Q);
        bool SetQuantities(MetaModel &m, const string& typ);
        bool SetQuantities(MetaModel *m, const string& typ );
        bool SetQuantities(QuanSet &Q);
        bool HasQuantity(const string &q);
        bool SetVal(const string& s, double value, const Expression::timing &tmg = Expression::timing::both);
        bool SetVal(const string& s, const string & value, const Expression::timing &tmg = Expression::timing::both);
        double GetProperty(const string& s) { 
            if (Variable(s) != nullptr)
            {
                return Variable(s)->GetVal();
                //qDebug() << QString::fromStdString(s) << ": " << var.GetVar(s).GetVal();
            }
            else
            {
                //qDebug() << "NullPtr";
                return 0;
            }
        }
        System *GetParent() const 
        {
            return parent;
        }
        string GetName() const;
        void SetDefaults();
        bool SetName(const string &_name, bool setprop=true);
        Object* GetConnectedBlock(Expression::loc l);
        void SetConnectedBlock(Expression::loc l, const string &blockname);
        void AppendError(const string &s);
        void SetParent(System *s);
        Quan* CorrespondingFlowVariable(const string &s);
        Quan* Variable(const string &s);
        void SetType(const string &typ) {type = typ;}
        string GetType() {return type;}
        int s_Block_No() {return s_Block_no; }
        int e_Block_No() {return e_Block_no; }
        void Set_s_Block(Object *O) { s_Block = O; }
        void Set_e_Block(Object *O) { e_Block = O; }
        Object* Get_s_Block() { return s_Block; }
        Object* Get_e_Block() { return e_Block; }
		bool Renew(const string &variable);
		bool Update(const string &variable);
		bool CalcExpressions(const Expression::timing& tmg);
        void VerifyQuans(ErrorHandler *errorhandler);
		vector<CTimeSeries*> TimeSeries() {return var.TimeSeries();}
        string TypeCategory() {return GetVars()->CategoryType();}
		QuanSet* GetVars()
            {
                return &var;
            }
        void SetOutflowLimitFactor(const double &val, const Expression::timing &tmg) 
		{
			if (tmg == Expression::timing::past)
				outflowlimitfactor_past = val;
			else 
				outflowlimitfactor_current = val;
		}
        double GetOutflowLimitFactor(const Expression::timing &tmg)
		{
			
			if (tmg == Expression::timing::past)
				return outflowlimitfactor_past;
			else
				return outflowlimitfactor_current;
		
		}
        void SetLimitedOutflow(bool x) {limitoutflow = x;}
        bool GetLimitedOutflow() {return limitoutflow;}
		void SetVariableParents();
        void ShowMessage(const string &msg);
        System* Parent() {if (parent!=nullptr) return parent; else return nullptr;}
        void SetAllParents();
        bool SetProperty(const string &prop, const string &value);
        string toString(int _tabs=0);
        void SetPrimaryKey(const string &prmkey) {primary_key = prmkey;}
        string GetPrimaryKey() {return primary_key;}
        void AssignRandomPrimaryKey();
        string toCommand();
        string toCommandSetAsParam();
        vector<string> ItemswithOutput();
        vector<string> quantitative_variable_list() {return var.quantitative_variable_list();}
        vector<string> *operators();
        vector<string> *functions();
        string& lasterror() {
            return last_error;
        }
    protected:

    private:
        QuanSet var;
        vector<string> errors;
        string last_error;
        bool last_operation_success;
        map<string, string> setting;
        System *parent = nullptr;
        string name;
        Object *s_Block;
        Object *e_Block;
        int s_Block_no, e_Block_no;
        string type;
        double outflowlimitfactor_past = 1;
		double outflowlimitfactor_current = 1;
        bool limitoutflow = false;
        string primary_key = "";
};

#endif // OBJECT_H
