#pragma once

#include "Block.h"
#include "Link.h"
#include "Object.h"
#include "Source.h"
#include "constituent.h"
#include "Vector_arma.h"
#include "Matrix_arma.h"
#include "MetaModel.h"
#include "BTCSet.h"
#include "Objective_Function_Set.h"
#include "Parameter_Set.h"
#include "reaction.h"
#include "RxnParameter.h"
#include "solutionlogger.h"
#ifdef QT_version
    #include "runtimeWindow.h"
    class GWidget;
    class logWindow;
#endif
#include "ErrorHandler.h"
#include <string>

#if Q_version
#include <QStringList>
#include "runtimewindow.h"
#include "QTime"
#endif

using namespace std; 


#ifdef DEBUG
#define CVector_arma CVector
#define CMatrix_arma CMatrix
#endif // _DEBUG

class Script;

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
    bool scalediagonal = false; 
    double landtozero_factor = 0.1; 
    bool optimize_lambda = true; 
    bool direct_jacobian = true; 
    bool write_solution_details = false;
    

};

struct outputs
{
    CBTCSet AllOutputs;
    CBTCSet ObservedOutputs;
};

struct solvertemporaryvars
{
    vector<CMatrix_arma> Inverse_Jacobian;
    vector<double> NR_coefficient;
    vector<bool> updatejacobian;
	int MaxNumberOfIterations()
	{
		return aquiutils::max(numiterations);
	}

	void SetUpdateJacobian(bool x)
	{
        for (unsigned int i = 0; i < updatejacobian.size(); i++)
			updatejacobian[i] = x;
    }
    vector<int> numiterations;
    int epoch_count;
    vector<string> fail_reason;
    double t;
    double dt;
    double dt_base;


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
	string default_template_path; 
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
        System(Script& scr);
        bool TransferResultsFrom(System *other);
        System& operator=(const System& other);
        void CreateFromScript(Script& scr, const string &settingsfilename="");
        double &GetTime() 
        {
            return SolverTempVars.t;
        }
        bool AddBlock(Block &blk, bool SetQuantities=true);
        bool AddSource(Source &src);
        bool AddLink(Link &lnk, const string &source, const string &destination);
        bool AddConstituent(Constituent &cnst);
        bool AddReaction(Reaction &rxn);
        bool AddReactionParameter(RxnParameter &rxn);
        Block *block(const string &s);
        Block *block(unsigned int i)
        {
            if (i<blocks.size())
                return &blocks[i];
            else
                return nullptr;
        }
        Object *Setting(unsigned int i)
        {
            if (i<Settings.size())
                return &Settings[i];
            else
                return nullptr;
        }
        unsigned long BlockCount() {return blocks.size();}
        unsigned long SettingsCount() {return Settings.size();}
        unsigned long LinksCount() {return links.size();}
        unsigned long SourcesCount() {return sources.size();}
        unsigned long ReactionsCount() {return reactions.size();}
        unsigned long ParametersCount() {return Parameters().size();}
        unsigned long ObjectiveFunctionsCount() {return ObjectiveFunctions().size();}
        unsigned long ConstituentsCount() {return constituents.size();}
        unsigned long ReactionParametersCount() {return reaction_parameters.size();}
        vector<string> GetAllSourceNames();
        vector<string> GetAllBlockNames();
        vector<string> GetAllLinkNames();
        vector<string> GetAllReactionNames();

        Link *link(const string &s);
        Link *link(unsigned int i)
        {
            if (i<links.size())
                return &links[i];
            else
                return nullptr;
        }
        Source *source(const string &s);
        Constituent *constituent(const string &s);
        Reaction *reaction(const string &s);
        RxnParameter *reactionparameter(const string &s);
        Source *source(unsigned int i)
        {   if (i<sources.size())
                return &sources[i];
            else
                return nullptr;
        }
        Constituent *constituent(unsigned int i)
        {   if (i<constituents.size())
                return &constituents[i];
            else
                return nullptr;
        }
        RxnParameter *reactionparameter(unsigned int i)
        {   if (i<reaction_parameters.size())
                return &reaction_parameters[i];
            else
                return nullptr;
        }
        Reaction *reaction(unsigned int i)
        {   if (i<reactions.size())
                return &reactions[i];
            else
                return nullptr;
        }
        Parameter *parameter(const string &s);
        Objective_Function *objectivefunction(const string &s);
        Objective_Function *objectivefunction(unsigned int i)
        {
            if (i<objective_function_set.size())
                return objective_function_set[i];
            else
                return nullptr;
        }
        Object *object(const string &s);
        Object *settings(const string &s);

        int blockid(const string &s);
        int linkid(const string &s);
        bool GetQuanTemplate(const string &filename);
        bool AppendQuanTemplate(const string &filename);
        void CopyQuansToMembers();
        double &dt() {return SolverTempVars.dt;}
        double &tend() {return SimulationParameters.tend;}
        double &tstart() {return SimulationParameters.tstart;}
        bool OneStepSolve(unsigned int i);
        bool OneStepSolve_mv(unsigned int statevarno);
		bool Renew(const string &variable);
		bool Update(const string &variable="");
        void UnUpdateAllVariables(); 
		//bool Solve(const string &variable, bool ApplyParams = false);
		bool Solve(bool ApplyParams = false);
		bool SetProp(const string &s, const double &val);
		bool SetProperty(const string &s, const string &val);
        CBTCSet& GetOutputs() {return Outputs.AllOutputs;}
        vector<string> GetAllBlockTypes();
        vector<string> GetAllLinkTypes();
		vector<string> GetAllSourceTypes();
		vector<string> GetAllTypesOf(const string& type);
        void SetVariableParents();
        MetaModel *GetMetaModel() {return  &metamodel;}
        QuanSet* GetModel(const string &type) {if (metamodel.Count(type)==1) return metamodel[type]; else return nullptr;}
        void clear();
        int lookup_observation(const string &s) {return 0;}
        int EpochCount() {return SolverTempVars.epoch_count;}
        ErrorHandler *GetErrorHandler() {return &errorhandler;}
 //Objective Functions
        void AppendObjectiveFunction(const string &name, const Objective_Function&, double weight=1);
        bool AppendObjectiveFunction(const string &name, const string &location, const Expression &exp, double weight=1);
        void UpdateObjectiveFunctions(double t);
        double GetObjectiveFunctionValue();
        Objective_Function *ObjectiveFunction(const string &name); // returns a pointer to an objective function
// Parameters
        Parameter *GetParameter(const string &name) {return parameter_set[name];}
        Parameter *GetParameter(int i) {return parameter_set[i];}
        Parameter_Set &Parameters() {return parameter_set;}
        Objective_Function_Set &ObjectiveFunctions() {return objective_function_set;}
        bool AppendParameter(const string &paramname, const double &lower_limit, const double &upper_limit, const string &prior_distribution = "normal");
        bool AppendParameter(const string &paramname, const Parameter& param);
        bool SetAsParameter(const string &location, const string &quantity, const string &parametername);
        bool RemoveAsParameter(const string &location, const string &quantity, const string &parametername);
        bool SetParameterValue(const string &paramname, const double &val);
        bool SetParameterValue(int i, const double &val);
        bool ApplyParameters();
        CTimeSeries *GetObjectiveFunctionTimeSeries(const string &name) {return ObjectiveFunction(name)->GetTimeSeries();}
        void SetSilent(bool _s) {silent = _s;}
        bool IsSilent() {return silent;}
        void ShowMessage(const string &msg) {if (!silent) cout<<msg<<std::endl; }
        void SetAllParents();
        ErrorHandler errorhandler;
        bool Echo(const string &object, const string &quantity = "", const string &feature="");
		string& DefaultTemplatePath() { return paths.default_template_path; }
		string InputPath() {return paths.inputpath;}
        string OutputPath() {return paths.outputpath;}
        vector<CTimeSeries*> TimeSeries();
        double GetMinimumNextTimeStepSize();
        Object *GetObjectBasedOnPrimaryKey(const string &s);
        bool SavetoScriptFile(const string &filename, const string &templatefilename="", const vector<string> addedtemplates = vector<string>());
        bool ReadSystemSettingsTemplate(const string &filename);
        void SetSystemSettings();
        void DisconnectLink(const string linkname);
        bool SetSystemSettingsObjectProperties(const string &s, const string &val);
        bool Delete(const string& objectname);
        void PopulateOperatorsFunctions();
        bool VerifyAsSource(Block* blk, Link* lnk);
        bool VerifyAsDestination(Block* blk, Link* lnk);
        ErrorHandler VerifyAllQuantities();
        bool CalcAllInitialValues();
        void WriteObjectsToLogger();
#if defined(QT_version)
        logWindow *LogWindow() {return logwindow;}
        void SetLogWindow(logWindow *lgwnd) {logwindow=lgwnd;}
#endif
#if defined(QT_version) || defined (Q_version)
        bool stop_triggered = false;
        QStringList QGetAllCategoryTypes();
		QStringList QGetAllObjectsofTypes(QString _type);
		QStringList QGetAllObjectsofTypeCategory(QString _type);
        RunTimeWindow *RunTimewindow() {return rtw;}
        void SetRunTimeWindow(RunTimeWindow* _rtw) {rtw = _rtw;}
        vector<string> *operators;
        vector<string> *functions;
        void SetOutputItems();
        vector<string> addedtemplates;
        //constituents
        void AddPropertytoAllBlocks(const string &name, Quan &quan);
        void AddPropertytoAllLinks(const string &name, Quan &quan);
        void UpdateAddedPropertiestoAllBlockLinks();
        vector<Quan> GetToBeCopiedQuantities();
        vector<Quan> GetToBeCopiedQuantities(Constituent *consttnt, const object_type &object_typ = object_type::none);
        bool AddAllConstituentRelateProperties();
        bool AddConstituentRelateProperties(Constituent *constituent);
        bool AddAllConstituentRelateProperties(Block *blk);
        bool AddAllConstituentRelateProperties(Link *lnk);
        void RenameConstituents(const string &oldname, const string &newname);
        vector<string> AllConstituents();
        vector<string> AllReactionParameters();
        void SetSolutionLogger(SolutionLogger &slnlogger);
        bool SetSolutionLogger(const string &filename);
        const solversettings& GetSolverSettings() const {return SolverSettings;}
        SolutionLogger *GetSolutionLogger();

        //constituents
#endif

    protected:

    private:
        vector<string> solvevariableorder;
        vector<Block> blocks;
        vector<Link> links;
        vector<Source> sources;
        vector<Constituent> constituents;
        vector<Reaction> reactions;
        vector<RxnParameter> reaction_parameters;
        vector<Object> Settings;
        map<string, Quan> addedpropertiestoallblocks;
        map<string, Quan> addedpropertiestoalllinks;
        MetaModel metamodel;
        CVector_arma GetResiduals(const string &variable, CVector_arma &X);
		void CorrectStoragesBasedonFluxes(const string& variable);
        CVector_arma CalcStateVariables(const string &variable, const Expression::timing &tmg = Expression::timing::past);
        CVector_arma GetStateVariables(const string &variable, const Expression::timing &tmg = Expression::timing::past);
        solversettings SolverSettings;
        simulationparameters SimulationParameters;
        vector<bool> OneStepSolve();
        CMatrix_arma Jacobian(const string &variale, CVector_arma &X);
        CVector_arma Jacobian(const string &variable, CVector_arma &V, CVector_arma &F0, int i);  //Works also w/o reference (&)
        bool CalculateFlows(const string &var, const Expression::timing &tmg = Expression::timing::present);
        void SetStateVariables(const string &variable, CVector_arma &X, const Expression::timing &tmg = Expression::timing::present);
        vector<bool> GetOutflowLimitedVector();
        void SetOutflowLimitedVector(vector<bool>& x);
        solvertemporaryvars SolverTempVars;
        outputs Outputs;
        void InitiateOutputs();
        void PopulateOutputs();
        void TransferQuantitiesFromMetaModel();
        void AppendQuantitiesFromMetaModel();
        Objective_Function_Set objective_function_set;
        Parameter_Set parameter_set;
        bool silent;
        _directories paths;
        vector<CTimeSeries*> alltimeseries;
        void CalculateAllExpressions(Expression::timing tmg = Expression::timing::present);
        void SetNumberOfStateVariables(unsigned int n)
		{
			SolverTempVars.fail_reason.resize(n);
			SolverTempVars.Inverse_Jacobian.resize(n);
			SolverTempVars.NR_coefficient.resize(n);
			SolverTempVars.numiterations.resize(n);
			SolverTempVars.updatejacobian.resize(n);
		}
        SolutionLogger *solutionlogger = nullptr;

#ifdef Q_version
    RunTimeWindow *rtw = nullptr;
#endif

#ifdef QT_version
        GraphWidget *diagramview;
        runtimeWindow *rtw;
        void updateProgress(bool finished);
        logWindow *logwindow = nullptr;
#endif
};


