#pragma once

#include "Block.h"
#include "Link.h"
#include "Object.h"
#include "Source.h"
#include "Vector_arma.h"
#include "Matrix_arma.h"
#include "MetaModel.h"
#include "BTCSet.h"
#include "Objective_Function_Set.h"
#include "Parameter_Set.h"
#ifdef QT_version
    #include "runtimeWindow.h"
    class GWidget;
    class logWindow;
#endif
#include "ErrorHandler.h"

struct solversettings
{
    double C_N_weight; //Crank-Nicholson Weight
    double NRtolerance = 1e-6; //Newton Raphson Tolerance
    double NR_coeff_reduction_factor = 0.8; //The coefficient to reduce the Newton-Raphson coefficient
    double NR_timestep_reduction_factor = 0.75;
    double NR_timestep_reduction_factor_fail = 0.3;
    double minimum_timestep = 1e-7;
    int NR_niteration_lower=20;
    int NR_niteration_upper=40;
    int NR_niteration_max=100;
    bool makeresultsuniform = false;

};

struct outputs
{
    CBTCSet AllOutputs;
    CBTCSet ObservedOutputs;
};

struct solvertemporaryvars
{
    CMatrix_arma Inverse_Jacobian;
    double NR_coefficient = 1;
    bool updatejacobian = true;
    int numiterations;
    int epoch_count=0;
    string fail_reason;
    double t;
    double dt;


};

struct simulationparameters
{
    double tstart = 0; //start time of simulation
    double tend = 1; //end time of simulation
    double dt0 = 0.01; // initial time-step size
};

struct _directories
{
    string inputpath;
    string outputpath;
};

class System: public Object
{
    public:
        System();
#ifdef QT_version
        System(GraphWidget* DiagramViewer,runtimeWindow *rtw);
        System(GraphWidget* diagramviewer,runtimeWindow *_rtw, const string &modelfilename);
        void GetModelConfiguration();
#endif
        virtual ~System();
        System(const System& other);
        System& operator=(const System& other);
        double &GetTime() {return SolverTempVars.t;}
        bool AddBlock(Block &blk);
        bool AddSource(Source &src);
        bool AddLink(Link &lnk, const string &source, const string &destination);
        Block *block(const string &s);
        Link *link(const string &s);
        Source *source(const string &s);
        Parameter *parameter(const string &s);
        Object *object(const string &s);
        int blockid(const string &s);
        int linkid(const string &s);
        bool GetQuanTemplate(const string &filename);
        void CopyQuansToMembers();
        double &dt() {return SolverTempVars.dt;}
        double &tend() {return SimulationParameters.tend;}
        double &tstart() {return SimulationParameters.tstart;}
        bool OneStepSolve(const string &s);
		bool Renew(const string &variable);
		bool Update(const string &variable);
		bool Solve(const string &variable, bool ApplyParams = false);
		bool Solve(bool ApplyParams = false);
		bool SetProp(const string &s, const double &val);
		bool SetProperty(const string &s, const string &val);
        CBTCSet& GetOutputs() {return Outputs.AllOutputs;}
        vector<string> GetAllBlockTypes();
        vector<string> GetAllLinkTypes();
        void SetVariableParents();
        MetaModel *GetMetaModel() {return  &metamodel;}
        QuanSet* GetModel(const string &type) {return metamodel[type];}
        void clear();
        int lookup_observation(const string &s) {return 0;}
        int EpochCount() {return SolverTempVars.epoch_count;}
 //Objective Functions
        void AppendObjectiveFunction(const string &name, const Objective_Function&, double weight=1);
        bool AppendObjectiveFunction(const string &name, const string &location, const Expression &exp, double weight=1);
        void UpdateObjectiveFunctions(double t);
        double GetObjectiveFunctionValue();
        Objective_Function *ObjectiveFunction(const string &name) {return &objective_function_set[name]->obj_funct;} // returns a pointer to an objective function
        Parameter *GetParameter(const string &name) {return parameter_set[name];}
        Parameter_Set &Parameters() {return parameter_set;}
        bool AppendParameter(const string &paramname, const double &lower_limit, const double &upper_limit, const string &prior_distribution = "normal");
        bool AppendParameter(const string &paramname, const Parameter& param);
        bool SetAsParameter(const string &location, const string &quantity, const string &parametername);
        bool SetParameterValue(const string &paramname, const double &val);
        bool SetParameterValue(int i, const double &val);
        bool ApplyParameters();
        CTimeSeries *GetObjectiveFunctionTimeSeries(const string &name) {return objective_function_set[name]->obj_funct.GetTimeSeries();}
        void SetSilent(bool _s) {silent = _s;}
        bool IsSilent() {return silent;}
        void ShowMessage(const string &msg) {if (!silent) cout<<msg<<endl; }
        void SetAllParents();
        ErrorHandler errorhandler;
        bool Echo(const string &object, const string &quantity = "", const string &feature="");
        string InputPath() {return paths.inputpath;}
        string OutputPath() {return paths.outputpath;}
#ifdef QT_version
        logWindow *LogWindow() {return logwindow;}
        void SetLogWindow(logWindow *lgwnd) {logwindow=lgwnd;}
        bool stop_triggered = false;
#endif

    protected:

    private:
        vector<string> solvevariableorder;
        vector<Block> blocks;
        vector<Link> links;
        vector<Source> sources;
        string last_error;
        MetaModel metamodel;
        CVector_arma GetResiduals(const string &variable, CVector_arma &X);
        CVector_arma CalcStateVariables(const string &variable, const Expression::timing &tmg = Expression::timing::past);
        CVector_arma GetStateVariables(const string &variable, const Expression::timing &tmg = Expression::timing::past);
        solversettings SolverSettings;
        simulationparameters SimulationParameters;
        bool OneStepSolve();
        CMatrix_arma Jacobian(const string &variale, CVector_arma &X);
        CVector_arma Jacobian(const string &variable, CVector_arma &V, CVector_arma &F0, int i);  //Works also w/o reference (&)
        bool CalculateFlows(const string &var, const Expression::timing &tmg = Expression::timing::present);
        void SetStateVariables(const string &variable, CVector_arma &X, const Expression::timing &tmg = Expression::timing::present);
        solvertemporaryvars SolverTempVars;
        outputs Outputs;
        void InitiateOutputs();
        void PopulateOutputs();
        void TransferQuantitiesFromMetaModel();
        Objective_Function_Set objective_function_set;
        Parameter_Set parameter_set;
        bool silent;
        _directories paths;

#ifdef QT_version
        GraphWidget *diagramview;
        runtimeWindow *rtw;
        void updateProgress(bool finished);
        logWindow *logwindow = nullptr;
#endif
};


