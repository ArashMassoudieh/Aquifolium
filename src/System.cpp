#include "System.h"
#include <fstream>
#pragma warning(pop)
#pragma warning(disable : 4996)
#include <json/json.h>
#include <Script.h>


#ifdef Q_version
    #include <QDebug>
#endif


#ifdef QT_version
#include "node.h"
#include "edge.h"
#include "GWidget.h"
#include "QDebug"
#include "logwindow.h"
#endif

System::System():Object::Object()
{
   PopulateOperatorsFunctions();
   Object::AssignRandomPrimaryKey();
}

void System::PopulateOperatorsFunctions()
{
    operators = new vector<string>;
    operators->push_back("+");
    operators->push_back("-");
    operators->push_back("*");
    operators->push_back("/");
    operators->push_back("^");
    operators->push_back("(");
    operators->push_back(")");
    operators->push_back(":");

    functions = new vector<string>;
    functions->push_back("abs");
    functions->push_back("sgn");
    functions->push_back("exp");
    functions->push_back("pos");
    functions->push_back("min");
    functions->push_back("max");
    functions->push_back("mon");
    functions->push_back("mbs");
}

#ifdef QT_version
System::System(GraphWidget* diagramviewer,runtimeWindow *_rtw):Object::Object()
{
    diagramview = diagramviewer;
    rtw = _rtw;
    GetModelConfiguration();

}
System::System(GraphWidget* diagramviewer,runtimeWindow *_rtw, const string &modelfilename):Object::Object()
{
    diagramview = diagramviewer;
    rtw = _rtw;
    GetQuanTemplate(modelfilename);
    GetModelConfiguration();

}

#endif

System::~System()
{
    //dtor
}

System::System(const System& other):Object::Object(other)
{
    PopulateOperatorsFunctions();
    SolverTempVars.SetUpdateJacobian(true);
	sources = other.sources;
	blocks = other.blocks;
    links = other.links;
    objective_function_set = other.objective_function_set;
    parameter_set = other.parameter_set;
    silent = other.silent;
    SimulationParameters = other.SimulationParameters;
    SolverSettings = other. SolverSettings;
    solvevariableorder = other.solvevariableorder;
	SolverTempVars = other.SolverTempVars;
    paths = other.paths;
    Settings = other.Settings;

    SetAllParents();
    Object::AssignRandomPrimaryKey();
}

System& System::operator=(const System& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    Object::operator=(rhs);
	SolverTempVars.SetUpdateJacobian(true);
	blocks = rhs.blocks;
    links = rhs.links;
    sources = rhs.sources;
    silent = rhs.silent;
    objective_function_set = rhs.objective_function_set;
    parameter_set = rhs.parameter_set;
    SimulationParameters = rhs.SimulationParameters;
    SolverSettings = rhs. SolverSettings;
    solvevariableorder = rhs.solvevariableorder;
	SolverTempVars = rhs.SolverTempVars;
    paths = rhs.paths;
    Settings = rhs.Settings;
    SetAllParents();
    PopulateOperatorsFunctions();
    Object::AssignRandomPrimaryKey();
    return *this;
}


bool System::AddBlock(Block &blk, bool SetQuantities)
{
    blocks.push_back(blk);
    block(blk.GetName())->SetParent(this);
    if (SetQuantities) 
        block(blk.GetName())->SetQuantities(metamodel, blk.GetType());
    block(blk.GetName())->SetParent(this);
	return true;
}

bool System::AddSource(Source &src)
{
    sources.push_back(src);
    source(src.GetName())->SetParent(this);
    source(src.GetName())->SetQuantities(metamodel, src.GetType());
    source(src.GetName())->SetParent(this);
	return true;
}

bool System::AddLink(Link &lnk, const string &source, const string &destination)
{
	if (!block(source))
	{
		errorhandler.Append("System", "System", "AddLink", "Block '" + source + "' does not exist!", 8791);
		return false;
	}

	if (!block(destination))
	{
		errorhandler.Append("System", "System", "AddLink", "Block '" + destination + "' does not exist!", 8792);
		return false;
	}
	
	
	if (!VerifyAsDestination(block(destination), &lnk))
        return false; 
    if (!VerifyAsSource(block(source), &lnk))
        return false;
    links.push_back(lnk);
    link(lnk.GetName())->SetParent(this);
    link(lnk.GetName())->SetConnectedBlock(Expression::loc::source, source);
    link(lnk.GetName())->SetConnectedBlock(Expression::loc::destination, destination);
	block(source)->AppendLink(links.size()-1,Expression::loc::source);
	block(destination)->AppendLink(links.size()-1,Expression::loc::destination);
	link(lnk.GetName())->SetQuantities(metamodel, lnk.GetType());
	link(lnk.GetName())->SetParent(this);
	return true;
}

Block *System::block(const string &s)
{
    for (unsigned int i=0; i<blocks.size(); i++)
        if (blocks[i].GetName() == s) return &blocks[i];

    //errorhandler.Append(GetName(),"System","block","Block '" + s + "' was not found",101);
    return nullptr;
}

int System::blockid(const string &s)
{
    for (unsigned int i=0; i<blocks.size(); i++)
        if (blocks[i].GetName() == s) return int(i);

    errorhandler.Append(GetName(),"System","blockid","Block '" + s + "' was not found",102);
    return -1;
}

int System::linkid(const string &s)
{
    for (unsigned int i=0; i<links.size(); i++)
        if (links[i].GetName() == s) return int(i);

    errorhandler.Append(GetName(),"System","linkid","Link '" + s + "' was not found",103);

    return -1;
}

Link *System::link(const string &s)
{
    for (unsigned int i=0; i<links.size(); i++)
        if (links[i].GetName() == s) return &links[i];

    //errorhandler.Append(GetName(),"System","link","Link '" + s + "' was not found",104);

    return nullptr;
}

Source *System::source(const string &s)
{
    for (unsigned int i=0; i<sources.size(); i++)
        if (sources[i].GetName() == s) return &sources[i];

    //errorhandler.Append(GetName(),"System","link","Link '" + s + "' was not found",104);

    return nullptr;
}

Object *System::settings(const string &s)
{
    for (unsigned int i=0; i<Settings.size(); i++)
        if (Settings[i].GetName() == s) return &Settings[i];

    //errorhandler.Append(GetName(),"System","link","Link '" + s + "' was not found",104);

    return nullptr;
}


Parameter *System::parameter(const string &s)
{
    if (Parameters().count(s) == 0)
    {
        errorhandler.Append(GetName(),"System","parameter","parameter '" + s + "' was not found",110);
        return nullptr;
    }
    else
        return (Parameters()[s]);
}

Objective_Function *System::objectivefunction(const string &s)
{
    if (ObjectiveFunctions().count(s) == 0)
    {
        errorhandler.Append(GetName(),"System","objective function","objective function '" + s + "' was not found",111);
        return nullptr;
    }
    else
        return (ObjectiveFunctions()[s]);
}

Object *System::object(const string &s)
{
    for (unsigned int i=0; i<Settings.size(); i++)
        if (Settings[i].GetName() == s) return &Settings[i];

    for (unsigned int i=0; i<links.size(); i++)
        if (links[i].GetName() == s) return &links[i];

    for (unsigned int i=0; i<blocks.size(); i++)
        if (blocks[i].GetName() == s) return &blocks[i];

    for (unsigned int i=0; i<sources.size(); i++)
        if (sources[i].GetName() == s) return &sources[i];

    for (unsigned int i=0; i<ParametersCount(); i++)
        if (Parameters()[i]->GetName() == s) return Parameters()[i];

    for (unsigned int i=0; i<ObjectiveFunctionsCount(); i++)
        if (ObjectiveFunctions()[i]->GetName() == s) return ObjectiveFunctions()[i];

    //errorhandler.Append(GetName(),"System","object","Object '" + s + "' was not found",105);

    return nullptr;
}

ErrorHandler System::VerifyAllQuantities()
{
    ErrorHandler errs;
    for (unsigned int i=0; i<Settings.size(); i++)
        Settings[i].VerifyQuans(&errs);

    for (unsigned int i=0; i<links.size(); i++)
        links[i].VerifyQuans(&errs);

    for (unsigned int i=0; i<blocks.size(); i++)
        blocks[i].VerifyQuans(&errs);

    for (unsigned int i=0; i<sources.size(); i++)
        sources[i].VerifyQuans(&errs);

    for (unsigned int i=0; i<ParametersCount(); i++)
        Parameters()[i]->VerifyQuans(&errs);

    for (unsigned int i=0; i<ObjectiveFunctionsCount(); i++)
        ObjectiveFunctions()[i]->VerifyQuans(&errs);

    return errs;
}

void System::UnUpdateAllVariables()
{
    for (unsigned int i = 0; i < links.size(); i++)
        links[i].UnUpdateAllValues();

    for (unsigned int i = 0; i < blocks.size(); i++)
        blocks[i].UnUpdateAllValues();

    for (unsigned int i = 0; i < sources.size(); i++)
        sources[i].UnUpdateAllValues();

    for (unsigned int i = 0; i < ObjectiveFunctionsCount(); i++)
        ObjectiveFunctions()[i]->UnUpdateAllValues();
}

Object *System::GetObjectBasedOnPrimaryKey(const string &s)
{
    for (unsigned int i=0; i<links.size(); i++)
        if (links[i].GetPrimaryKey() == s) return &links[i];

    for (unsigned int i=0; i<blocks.size(); i++)
        if (blocks[i].GetPrimaryKey() == s) return &blocks[i];

    for (unsigned int i=0; i<sources.size(); i++)
        if (sources[i].GetPrimaryKey() == s) return &sources[i];

    for (unsigned int i=0; i<ParametersCount(); i++)
        if (Parameters()[i]->GetPrimaryKey() == s) return Parameters()[i];

    //errorhandler.Append(GetName(),"System","object","Object '" + s + "' was not found",105);

    return nullptr;

}

bool System::GetQuanTemplate(const string &filename)
{
    //qDebug()<<QString::fromStdString(filename);
    if (!metamodel.GetFromJsonFile(filename)) return false;
    TransferQuantitiesFromMetaModel();
    return true;
}

bool System::AppendQuanTemplate(const string &filename)
{
    if (!metamodel.AppendFromJsonFile(filename)) return false;
    addedtemplates.push_back(filename);
    TransferQuantitiesFromMetaModel();
    return true;
}


void System::CopyQuansToMembers()
{
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        blocks[i].SetQuantities(metamodel,blocks[i].GetType());
    }
    for (unsigned int i=0; i<links.size(); i++)
        links[i].SetQuantities(metamodel,blocks[i].GetType());

    for (unsigned int i=0; i<sources.size(); i++)
        sources[i].SetQuantities(metamodel,sources[i].GetType());


}

vector<bool> System::OneStepSolve()
{
	vector<bool> success(solvevariableorder.size());
	for (int i = 0; i < solvevariableorder.size(); i++)
		success[i] = OneStepSolve(i);

	return success;
}

bool System::Solve(bool applyparameters)
{
    double timestepminfactor = 100000;
    double timestepmaxfactor = 50; 
    SetAllParents();
	SolverTempVars.SetUpdateJacobian(true);
	alltimeseries = TimeSeries();
	bool success = true;
    errorhandler.SetRunTimeWindow(rtw);
	if (applyparameters) ApplyParameters();
    InitiateOutputs();

    SolverTempVars.dt_base = SimulationParameters.dt0;
    SolverTempVars.dt = SolverTempVars.dt_base;
    SolverTempVars.t = SimulationParameters.tstart;
    CalculateAllExpressions();
    PopulateOutputs();
    for (unsigned int i=0; i<ObjectiveFunctionsCount(); i++)
    {
        ObjectiveFunctions()[i]->GetTimeSeries()->clear();
    }
#ifdef Q_version
    if (rtw)
    {
        rtw->AppendText("Simulation Started at " + QTime::currentTime().toString(Qt::RFC2822Date) + "!");
        rtw->SetXRange(SimulationParameters.tstart,SimulationParameters.tend);
        rtw->SetYRange(0,SimulationParameters.dt0*timestepmaxfactor);
    }
#endif
    CalculateAllExpressions(Expression::timing::past);
    int counter = 0; 
    int fail_counter = 0; 
    while (SolverTempVars.t<SimulationParameters.tend+SolverTempVars.dt && !stop_triggered)
    {
        counter++; 
        //cout << "\r Simulation Time: " + aquiutils::numbertostring(SolverTempVars.t);
		if (counter%50==0) 
            SolverTempVars.SetUpdateJacobian(true);
        SolverTempVars.dt = min(SolverTempVars.dt_base,GetMinimumNextTimeStepSize());
        if (SolverTempVars.dt<SimulationParameters.dt0/ timestepminfactor) SolverTempVars.dt=SimulationParameters.dt0/ timestepminfactor;
        #ifdef Debug_mode
        ShowMessage(string("t = ") + aquiutils::numbertostring(SolverTempVars.t) + ", dt_base = " + aquiutils::numbertostring(SolverTempVars.dt_base) + ", dt = " + aquiutils::numbertostring(SolverTempVars.dt) + ", SolverTempVars.numiterations =" + aquiutils::numbertostring(SolverTempVars.numiterations));
        #endif // Debug_mode


        vector<bool> _success = OneStepSolve();
		success = aquiutils::And(_success);
		if (!success)
        {
            fail_counter++; 
            #ifdef Debug_mode
            ShowMessage("failed!");
            #endif // Debug_mode
#ifdef Q_version
            if (rtw)
            {
                if (rtw->detailson)
                {
                    rtw->AppendtoDetails(QString::fromStdString(SolverTempVars.fail_reason[SolverTempVars.fail_reason.size() - 1]) + ", dt = " + QString::number(SolverTempVars.dt,'e'));
                    if (rtw->stoptriggered) stop_triggered = true;

                }
                QCoreApplication::processEvents();
            }
#endif
            SolverTempVars.dt_base *= SolverSettings.NR_timestep_reduction_factor_fail;
            SolverTempVars.SetUpdateJacobian(true);

            if (fail_counter > 20)
            {
#ifdef Q_version
                if (rtw)
                {
                    if (rtw->detailson)
                    {
                        rtw->AppendtoDetails("The attempt to solve the problem failed");
                        
                    }
                    rtw->AppendErrorMessage("The attempt to solve the problem failed!");
                    QCoreApplication::processEvents();
                }
#endif
                stop_triggered = true;
            }
        }
        else
        {
#ifdef Q_version
            if (rtw)
            {
                rtw->SetProgress((SolverTempVars.t - SimulationParameters.tstart) / (SimulationParameters.tend - SimulationParameters.tstart));
                rtw->AddDataPoint(SolverTempVars.t, SolverTempVars.dt);
                if (rtw->detailson) rtw->AppendtoDetails("Number of iterations:" + QString::number(SolverTempVars.MaxNumberOfIterations()) + ",dt = " + QString::number(SolverTempVars.dt) + ",t = " + QString::number(SolverTempVars.t, 'f', 6) + ", NR_factor = " + QString::fromStdString(CVector(SolverTempVars.NR_coefficient).toString()));
                if (rtw->stoptriggered) stop_triggered = true;
                QCoreApplication::processEvents();
            }
#endif
            fail_counter = 0; 
            SolverTempVars.t += SolverTempVars.dt;
            if (SolverTempVars.MaxNumberOfIterations()>SolverSettings.NR_niteration_upper)
            {
                SolverTempVars.dt_base = max(SolverTempVars.dt*SolverSettings.NR_timestep_reduction_factor,SolverSettings.minimum_timestep);
                SolverTempVars.SetUpdateJacobian(true);
                SolverTempVars.NR_coefficient = (CVector(SolverTempVars.NR_coefficient.size()) + 1).vec;
            }
            if (SolverTempVars.MaxNumberOfIterations() < SolverSettings.NR_niteration_lower)
            {
                SolverTempVars.dt_base = min(SolverTempVars.dt_base / SolverSettings.NR_timestep_reduction_factor, SimulationParameters.dt0 * timestepmaxfactor);
                SolverTempVars.NR_coefficient = (CVector(SolverTempVars.NR_coefficient.size()) + 1).vec;
            }

            for (unsigned int i=0; i<solvevariableorder.size(); i++)
                Update(solvevariableorder[i]);
            UpdateObjectiveFunctions(SolverTempVars.t);
            PopulateOutputs();
        }

    }
#ifdef Q_version
    if (rtw)
    {
        if (!stop_triggered)
            rtw->SetProgress(1);
        rtw->AddDataPoint(SolverTempVars.t,SolverTempVars.dt);
        rtw->AppendText("Simulation finished!" + QTime::currentTime().toString(Qt::RFC2822Date) + "!");
        
        QCoreApplication::processEvents();
    }
#endif

    #ifdef QT_version
    updateProgress(true);
    if (LogWindow())
    {
        LogWindow()->append("Simulation finished!");
    }
    #else
    ShowMessage("Simulation finished!");
    #endif
    return true;
}

#ifdef QT_version
void System::updateProgress(bool finished)
{
    // t, dtt (graph), epoch_count
    if (rtw != nullptr)
    {
        QMap<QString, QVariant> vars;
        vars["mode"] = "forward";
        if (finished)
        {
            vars["progress"] = 100;
            vars["finished"] = true;
        }
        else
        {
            int progress;
            progress = 100.0*(SolverTempVars.t - SimulationParameters.tstart) / (SimulationParameters.tend - SimulationParameters.tstart);
            vars["t"] = SolverTempVars.t;
            vars["progress"] = progress;
            vars["dtt"] = SolverTempVars.dt;
            vars["epoch count"] = SolverTempVars.epoch_count;
            QString reason = QString::fromStdString(SolverTempVars.fail_reason[SolverTempVars.fail_reason.size()-1]);
            ////qDebug() << reason;
            if (!reason.toLower().contains("none"))
                vars["label"] = reason;
            ////qDebug()<< t<<dtt;

            if (rtw->sln_dtl_active)
                if (!reason.toLower().contains("none"))
                    rtw->slndetails_append(QString::number(SolverTempVars.epoch_count) + ":" + reason + ", time step size: " + QString::number(SolverTempVars.dt));
        }
        rtw->update(vars);
        if (finished)
        {
            //QMessageBox::StandardButton reply;
            //QMessageBox::question(runtimewindow, "Simulation Ended", "Simulation Finished!", QMessageBox::Ok);
        }
    }
}
#endif

bool System::SetProp(const string &s, const double &val)
{
    if (s=="cn_weight")
    {   SolverSettings.C_N_weight = val; return true;}
    if (s=="nr_tolerance")
    {   SolverSettings.NRtolerance = val; return true;}
    if (s=="nr_coeff_reduction_factor")
    {   SolverSettings.NR_coeff_reduction_factor = val; return true;}
    if (s=="nr_timestep_reduction_factor")
    {   SolverSettings.NR_timestep_reduction_factor = val; return true;}
    if (s=="nr_timestep_reduction_factor_fail")
    {   SolverSettings.NR_timestep_reduction_factor_fail = val; return true;}
    if (s=="minimum_timestep")
    {   SolverSettings.minimum_timestep = val; return true;}
    if (s=="nr_niteration_lower")
    {   SolverSettings.NR_niteration_lower=int(val); return true;}
    if (s=="nr_niteration_upper")
    {   SolverSettings.NR_niteration_upper=int(val); return true;}
    if (s=="nr_niteration_max")
    {   SolverSettings.NR_niteration_max=int(val); return true;}
    if (s=="make_results_uniform")
    {   SolverSettings.makeresultsuniform = bool(val); return true;}

    if (s=="tstart")
    {   SimulationParameters.tstart = val; return true;}
    if (s=="tend")
    {   SimulationParameters.tend = val; return true;}
    if (s=="tend")
    {   SimulationParameters.dt0 = val; return true;}

    errorhandler.Append("","System","SetProp","Property '" + s + "' was not found!", 621);
    return false;
}

bool System::SetSystemSettingsObjectProperties(const string &s, const string &val)
{
    for (unsigned int i=0; i<Settings.size(); i++)
    {
        for (map<string, Quan>::iterator j=Settings[i].GetVars()->begin(); j!=Settings[i].GetVars()->end(); j++)
        {   if (j->first==s)
            {   j->second.SetProperty(val);
                return true;
            }

        }
    }
    errorhandler.Append("","System","SetSystemSettingsObjectProperties","Property '" + s + "' was not found!", 631);
    return false;

}
bool System::SetProperty(const string &s, const string &val)
{

    if (s=="name") return true;
    if (s=="c_n_weight")
    {   SolverSettings.C_N_weight = aquiutils::atof(val); return true;}
    if (s=="nr_tolerance")
    {   SolverSettings.NRtolerance = aquiutils::atof(val); return true;}
    if (s=="nr_coeff_reduction_factor")
    {   SolverSettings.NR_coeff_reduction_factor = aquiutils::atof(val); return true;}
    if (s=="nr_timestep_reduction_factor")
    {   SolverSettings.NR_timestep_reduction_factor = aquiutils::atof(val); return true;}
    if (s=="nr_timestep_reduction_factor_fail")
    {   SolverSettings.NR_timestep_reduction_factor_fail = aquiutils::atof(val); return true;}
    if (s=="minimum_timestep")
    {   SolverSettings.minimum_timestep = aquiutils::atof(val); return true;}
    if (s=="nr_niteration_lower")
    {   SolverSettings.NR_niteration_lower= aquiutils::atoi(val); return true;}
    if (s=="nr_niteration_upper")
    {   SolverSettings.NR_niteration_upper= aquiutils::atoi(val); return true;}
    if (s=="nr_niteration_max")
    {   SolverSettings.NR_niteration_max= aquiutils::atoi(val); return true;}
    if (s=="make_results_uniform")
    {   SolverSettings.makeresultsuniform = aquiutils::atoi(val); return true;}

    if (s=="tstart" || s== "simulation_start_time")
    {   SimulationParameters.tstart = aquiutils::atof(val); return true;}
    if (s=="tend" || s=="simulation_end_time")
    {   SimulationParameters.tend = aquiutils::atof(val); return true;}
    if (s=="dt" || s=="initial_time_step")
    {   
        SimulationParameters.dt0 = aquiutils::atof(val); return true;
    }
    if (s=="silent")
    {
        SetSilent(aquiutils::atoi(val)); return true;
    }
    if (s=="inputpath")
    {
        paths.inputpath = val; return true;
    }
    if (s=="outputpath")
    {
        paths.outputpath = val; return true;
    }

    errorhandler.Append("","System","SetProperty","Property '" + s + "' was not found!", 622);
    return false;
}


void System::InitiateOutputs()
{
    Outputs.AllOutputs.clear();
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        for (map<string, Quan>::iterator it = blocks[i].GetVars()->begin(); it != blocks[i].GetVars()->end(); it++)
            if (it->second.IncludeInOutput())
            {
                Outputs.AllOutputs.append(CBTC(), blocks[i].GetName() + "_" + it->first);
                it->second.SetOutputItem(blocks[i].GetName() + "_" + it->first);
            }
    }

    for (unsigned int i=0; i<links.size(); i++)
    {
        for (map<string, Quan>::iterator it = links[i].GetVars()->begin(); it != links[i].GetVars()->end(); it++)
            if (it->second.IncludeInOutput())
            {
                Outputs.AllOutputs.append(CBTC(), links[i].GetName() + "_" + it->first);
                it->second.SetOutputItem(links[i].GetName() + "_" + it->first);
            }
    }

    for (unsigned int i=0; i<objective_function_set.size(); i++)
    {
        Outputs.AllOutputs.append(CBTC(), "Obj_" + objective_function_set[i]->GetName());
        objective_function_set[i]->SetOutputItem("Obj_" + objective_function_set[i]->GetName());
    }

}

void System::SetOutputItems()
{
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        for (map<string, Quan>::iterator it = blocks[i].GetVars()->begin(); it != blocks[i].GetVars()->end(); it++)
            if (it->second.IncludeInOutput())
            {
                it->second.SetOutputItem(blocks[i].GetName() + "_" + it->first);
            }
    }

    for (unsigned int i=0; i<links.size(); i++)
    {
        for (map<string, Quan>::iterator it = links[i].GetVars()->begin(); it != links[i].GetVars()->end(); it++)
            if (it->second.IncludeInOutput())
            {
                it->second.SetOutputItem(links[i].GetName() + "_" + it->first);
            }
    }

    for (unsigned int i=0; i<objective_function_set.size(); i++)
    {
        objective_function_set[i]->SetOutputItem("Obj_" + objective_function_set[i]->GetName());
    }

}

bool System::TransferResultsFrom(System *other)
{
    Outputs = other->Outputs;
    return true; 
}

void System::PopulateOutputs()
{
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        for (map<string, Quan>::iterator it = blocks[i].GetVars()->begin(); it != blocks[i].GetVars()->end(); it++)
			if (it->second.IncludeInOutput())
			{
				blocks[i].CalcExpressions(Expression::timing::present); 
				Outputs.AllOutputs[blocks[i].GetName() + "_" + it->first].append(SolverTempVars.t, blocks[i].GetVal(it->first, Expression::timing::present));
			}
    }

    for (unsigned int i=0; i<links.size(); i++)
    {
        for (map<string, Quan>::iterator it = links[i].GetVars()->begin(); it != links[i].GetVars()->end(); it++)
            if (it->second.IncludeInOutput())
            {
				links[i].CalcExpressions(Expression::timing::present); 
				Outputs.AllOutputs[links[i].GetName() + "_" + it->first].append(SolverTempVars.t,links[i].GetVal(it->first,Expression::timing::present,true));
            }
    }

    for (unsigned int i=0; i<objective_function_set.size(); i++)
    {
        Outputs.AllOutputs["Obj_" + objective_function_set[i]->GetName()].append(SolverTempVars.t,objectivefunction(objective_function_set[i]->GetName())->Value());
    }

}


vector<bool> System::GetOutflowLimitedVector()
{
    vector<bool> out; 
    for (unsigned int i = 0; i<blocks.size(); i++)
        out.push_back(blocks[i].GetLimitedOutflow());
    
    return out; 
}

void System::SetOutflowLimitedVector(vector<bool> &x)
{
    for (unsigned int i = 0; i < blocks.size(); i++)
        blocks[i].SetLimitedOutflow(x[i]);

}

bool System::OneStepSolve(int statevarno)
{
	string variable = solvevariableorder[statevarno];
	Renew(variable);
    for (unsigned int i = 0; i < links.size(); i++) links[i].SetOutflowLimitFactor(links[i].GetOutflowLimitFactor(Expression::timing::past), Expression::timing::present);
    vector<bool> outflowlimitstatus_old = GetOutflowLimitedVector(); 
    SolverTempVars.numiterations[statevarno] = 0;
    bool switchvartonegpos = true;
    int attempts = 0;
    while (attempts<2 && switchvartonegpos)
    {
		CVector_arma X = GetStateVariables(variable, Expression::timing::past);
        for (unsigned int i = 0; i < blocks.size(); i++)
		{
			if (blocks[i].GetLimitedOutflow())
				X[i] = blocks[i].GetOutflowLimitFactor(Expression::timing::past);
		}

		CVector_arma X_past = X;

		CVector_arma F = GetResiduals(variable, X);
        double error_increase_counter = 0; 
		double err_ini = F.norm2();
		double err;
		double err_p = err = err_ini;
        
		//if (SolverTempVars.NR_coefficient[statevarno]==0) 
            SolverTempVars.NR_coefficient[statevarno] = 1;
		while ((err>SolverSettings.NRtolerance && err>1e-12) || SolverTempVars.numiterations[statevarno]>SolverSettings.NR_niteration_max)
        {
            SolverTempVars.numiterations[statevarno]++;
            if (SolverTempVars.updatejacobian[statevarno])
            {
                CMatrix_arma J = Jacobian(variable, X);
                if (SolverSettings.scalediagonal)
                    J.ScaleDiagonal(1.0 / SolverTempVars.NR_coefficient[statevarno]);
				if (det(J) == 0)
				{
					SolverTempVars.fail_reason.push_back("at " + aquiutils::numbertostring(SolverTempVars.t) + ": The Jacobian Matrix is not full-ranked");
                    SetOutflowLimitedVector(outflowlimitstatus_old);
                    return false; 
				}
				if (!SolverSettings.direct_jacobian)
                    SolverTempVars.Inverse_Jacobian[statevarno] = Invert(J);
                else
                    SolverTempVars.Inverse_Jacobian[statevarno] = J;
                SolverTempVars.updatejacobian[statevarno] = false;
                
            }
            CVector_arma X1;
            if (SolverSettings.scalediagonal)
            {
                if (!SolverSettings.direct_jacobian)
                    X = X - SolverTempVars.Inverse_Jacobian[statevarno] * F;
                else
                    X = X - F/SolverTempVars.Inverse_Jacobian[statevarno];
            }
            else
            {
                if (!SolverSettings.direct_jacobian)
                    X = X - SolverTempVars.NR_coefficient[statevarno] * SolverTempVars.Inverse_Jacobian[statevarno] * F;
                else
                    X -= SolverTempVars.NR_coefficient[statevarno] * (F/ SolverTempVars.Inverse_Jacobian[statevarno]);
                if (SolverSettings.optimize_lambda)
                {
                    if (!SolverSettings.direct_jacobian)
                        X1 = X + 0.5 * SolverTempVars.NR_coefficient[statevarno] * SolverTempVars.Inverse_Jacobian[statevarno] * F;
                    else
                        X1 = X + 0.5 * SolverTempVars.NR_coefficient[statevarno] * (F / SolverTempVars.Inverse_Jacobian[statevarno]);
                }
            }
			if (!X.is_finite())
			{
				SolverTempVars.fail_reason.push_back("at " + aquiutils::numbertostring(SolverTempVars.t) + ": X is infinite");
                SetOutflowLimitedVector(outflowlimitstatus_old);
                return false;
			}

			F = GetResiduals(variable, X);
            CVector_arma F1;
            if (SolverSettings.optimize_lambda)
                F1 = GetResiduals(variable, X1);
			if (!F.is_finite())
			{
				SolverTempVars.fail_reason.push_back("at " + aquiutils::numbertostring(SolverTempVars.t) + ": F is infinite");
                SetOutflowLimitedVector(outflowlimitstatus_old);
                return false;
			}
            err_p = err;
            err = F.norm2();
            double err2; 
            if (SolverSettings.optimize_lambda)
            {
                err2 = F1.norm2();
                if (err2 < err)
                {
                    SolverTempVars.NR_coefficient[statevarno] = max(SolverTempVars.NR_coefficient[statevarno]/2.0, 0.05);
                    SolverTempVars.updatejacobian[statevarno] = true;
                    X = X1;
                }
                else
                {
                    SolverTempVars.NR_coefficient[statevarno] = max(min(SolverTempVars.NR_coefficient[statevarno] * 1.25, 1.0), 0.05);
                }
                if (min(err2, err) > err_p)
                {
                    error_increase_counter++;
                }
            }
            #ifdef Debug_mode
            //ShowMessage(numbertostring(err));
            #endif // Debug_mode
			if (err > err_p*0.9 && !SolverSettings.optimize_lambda)
			{
				SolverTempVars.NR_coefficient[statevarno] = max(SolverTempVars.NR_coefficient[statevarno]*SolverSettings.NR_coeff_reduction_factor,0.05);
				SolverTempVars.updatejacobian[statevarno] = true;
                X = X_past;
			}
            if (err>err_p && !SolverSettings.optimize_lambda)
                error_increase_counter++;
            else if (err<err_p/2.0 && !SolverSettings.optimize_lambda)
            {
                if (SolverTempVars.NR_coefficient[statevarno] < 0.99)
                    SolverTempVars.updatejacobian[statevarno] = true;
                SolverTempVars.NR_coefficient[statevarno] = max(min(SolverTempVars.NR_coefficient[statevarno] / SolverSettings.NR_coeff_reduction_factor, 1.0), 0.05);
             }
            if (error_increase_counter > 10)
            {
                SolverTempVars.fail_reason.push_back("at " + aquiutils::numbertostring(SolverTempVars.t) + ": Error kept increasing");
                SetOutflowLimitedVector(outflowlimitstatus_old);
                return false; 
            }
            
            if (SolverTempVars.numiterations[statevarno] > SolverSettings.NR_niteration_max)
            {
                SolverTempVars.fail_reason.push_back("at " + aquiutils::numbertostring(SolverTempVars.t) + ": numbe of iterations exceeded the maximum threshold");
                SetOutflowLimitedVector(outflowlimitstatus_old);
                return false;
            }
        }
        switchvartonegpos = false;
        
        for (unsigned int i=0; i<blocks.size(); i++)
        {
            if (X[i]<-1e-13 && !blocks[i].GetLimitedOutflow())
            {
                blocks[i].SetLimitedOutflow(true);
                switchvartonegpos = true;
                SolverTempVars.updatejacobian[statevarno] = true;
            }
            else if (X[i]>1 && blocks[i].GetLimitedOutflow())
            {
                blocks[i].SetLimitedOutflow(false);
                switchvartonegpos = true;
                SolverTempVars.updatejacobian[statevarno] = true;
            }
        }
        if (switchvartonegpos) attempts++;
    }
    //CorrectStoragesBasedonFluxes(variable);
    /*if (attempts == 2)
	{
		SolverTempVars.fail_reason.push_back("at " + aquiutils::numbertostring(SolverTempVars.t) + ": attempts > 1");
        SetOutflowLimitedVector(outflowlimitstatus_old);
        return false;
	}
	if (SolverTempVars.numiterations[statevarno] > SolverSettings.NR_niteration_max)
	{
		SolverTempVars.fail_reason.push_back("at " + aquiutils::numbertostring(SolverTempVars.t) + ": number of iterations exceeded the limit");
        SetOutflowLimitedVector(outflowlimitstatus_old);
		return false;
	}*/
	#ifdef Debug_mode
//	CMatrix_arma M = Jacobian("Storage",X);
//	M.writetofile("M.txt");
	#endif // Debug_mode
	return true;
}

bool System::Renew(const string & variable)
{
	bool out = true;
	for (unsigned int i = 0; i < blocks.size(); i++)
		out &= blocks[i].Renew(variable);

	for (unsigned int i = 0; i < links.size(); i++)
  		out &= links[i].Renew(Variable(variable)->GetCorrespondingFlowVar());

	return out;
}

bool System::Update(const string & variable)
{
	bool out = true;
	for (unsigned int i = 0; i < blocks.size(); i++)
		out &= blocks[i].Update(variable);

//	for (unsigned int i = 0; i < links.size(); i++)
//		out &= links[i].Update(variable);

	return out;
}


CVector_arma System::CalcStateVariables(const string &variable, const Expression::timing &tmg)
{
    CVector_arma X(blocks.size());
    for (unsigned int i=0; i<blocks.size(); i++)
        X[i] = blocks[i].CalcVal(variable,tmg);
    return X;
}

CVector_arma System::GetStateVariables(const string &variable, const Expression::timing &tmg)
{
    CVector_arma X(blocks.size());
    for (unsigned int i=0; i<blocks.size(); i++)
        X[i] = blocks[i].GetVal(variable,tmg);
    return X;
}

void System::SetStateVariables(const string &variable, CVector_arma &X, const Expression::timing &tmg)
{
    for (unsigned int i=0; i<blocks.size(); i++)
    {
		if (!blocks[i].GetLimitedOutflow())
		{
			blocks[i].SetVal(variable, X[i], tmg);
			blocks[i].SetOutflowLimitFactor(1, tmg);
		}
		else
		{
			blocks[i].SetOutflowLimitFactor(X[i],tmg);
			blocks[i].SetVal(variable, 0, tmg);
		}

    }
}

void System::CalculateAllExpressions(Expression::timing tmg)
{
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        for (int j = 0; j < blocks[i].QuantitOrder().size(); j++)
        {
            if (blocks[i].Variable(blocks[i].QuantitOrder()[j])->GetType() == Quan::_type::expression)
                blocks[i].Variable(blocks[i].QuantitOrder()[j])->SetVal(blocks[i].Variable(blocks[i].QuantitOrder()[j])->CalcVal(tmg), tmg);
        }
    }

    for (unsigned int i=0; i<links.size(); i++)
    {
        for (int j = 0; j < links[i].QuantitOrder().size(); j++)
        {
            if (links[i].Variable(links[i].QuantitOrder()[j])->GetType() == Quan::_type::expression)
                links[i].Variable(links[i].QuantitOrder()[j])->SetVal(links[i].Variable(links[i].QuantitOrder()[j])->CalcVal(tmg), tmg);
        }
    }
}

CVector_arma System::GetResiduals(const string &variable, CVector_arma &X)
{
    CVector_arma F(blocks.size());
    SetStateVariables(variable,X,Expression::timing::present);
    UnUpdateAllVariables(); 
    //CalculateFlows(Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present);

    for (unsigned int i=0; i<blocks.size(); i++)
    {
        if (blocks[i].isrigid(variable))
        {
            F[i] = - blocks[i].GetInflowValue(variable, Expression::timing::present);
        }
        else if (blocks[i].GetLimitedOutflow())
        {
            blocks[i].SetOutflowLimitFactor(X[i],Expression::timing::present);
            blocks[i].SetVal(variable, blocks[i].GetVal(variable, Expression::timing::past) * SolverSettings.landtozero_factor,Expression::timing::present);
            F[i] = -blocks[i].GetVal(variable,Expression::timing::past)*(1.0-SolverSettings.landtozero_factor)/dt() - blocks[i].GetInflowValue(variable,Expression::timing::present);
        }
        else
            F[i] = (X[i]-blocks[i].GetVal(variable,Expression::timing::past))/dt() - blocks[i].GetInflowValue(variable,Expression::timing::present);
    }


    for (unsigned int i=0; i<links.size(); i++)
    {
        if (blocks[links[i].s_Block_No()].GetLimitedOutflow() && links[i].GetVal(blocks[links[i].s_Block_No()].Variable(variable)->GetCorrespondingFlowVar(), Expression::timing::present) > 0)
            links[i].SetOutflowLimitFactor(blocks[links[i].s_Block_No()].GetOutflowLimitFactor(Expression::timing::present), Expression::timing::present);
        else if (blocks[links[i].e_Block_No()].GetLimitedOutflow() && links[i].GetVal(blocks[links[i].e_Block_No()].Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present)<0)
            links[i].SetOutflowLimitFactor(blocks[links[i].e_Block_No()].GetOutflowLimitFactor(Expression::timing::present), Expression::timing::present);
        else
            links[i].SetOutflowLimitFactor(1, Expression::timing::present);

    }

    for (unsigned int i=0; i<links.size(); i++)
    {
        F[links[i].s_Block_No()] += links[i].GetVal(blocks[links[i].s_Block_No()].Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present)*links[i].GetOutflowLimitFactor(Expression::timing::present);
        F[links[i].e_Block_No()] -= links[i].GetVal(blocks[links[i].s_Block_No()].Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present)*links[i].GetOutflowLimitFactor(Expression::timing::present);
    }
    
    for (unsigned int i = 0; i < links.size(); i++)
        if (links[i].GetOutflowLimitFactor(Expression::timing::present) < 0)
        {
            F[links[i].e_Block_No()] += (double(sgn(F[links[i].e_Block_No()])) - 0.5) * pow(links[i].GetOutflowLimitFactor(Expression::timing::present), 2)*fabs(links[i].GetVal(blocks[links[i].s_Block_No()].Variable(variable)->GetCorrespondingFlowVar(), Expression::timing::present));
            F[links[i].s_Block_No()] += (double(sgn(F[links[i].s_Block_No()])) - 0.5) * pow(links[i].GetOutflowLimitFactor(Expression::timing::present), 2)*fabs(links[i].GetVal(blocks[links[i].s_Block_No()].Variable(variable)->GetCorrespondingFlowVar(), Expression::timing::present));
        }

    for (unsigned int i = 0; i < blocks.size(); i++)
    {
        bool alloutflowszero = true;
        if (blocks[i].GetLimitedOutflow())
        {
            double outflow = blocks[i].GetInflowValue(variable, Expression::timing::present);
            alloutflowszero &= !(outflow < 0);
            for (int j = 0; j < blocks[i].GetLinksFrom().size(); j++)
            {
                outflow = blocks[i].GetLinksFrom()[j]->GetVal(blocks[i].Variable(variable)->GetCorrespondingFlowVar(), Expression::timing::present);
                alloutflowszero &= !(outflow > 0);
            }
            for (int j = 0; j < blocks[i].GetLinksTo().size(); j++)
            {
                outflow = blocks[i].GetLinksTo()[j]->GetVal(blocks[i].Variable(variable)->GetCorrespondingFlowVar(), Expression::timing::present);
                alloutflowszero &= !(outflow < 0);
            }
        
            if (alloutflowszero)
            {
                F[i] = X[i] - 1.1;
            }

        }
        
    }
    return F;
}

void System::CorrectStoragesBasedonFluxes(const string& variable)
{
    CVector_arma UpdatedStorage(blocks.size());
    for (unsigned int i = 0; i < blocks.size(); i++)
        UpdatedStorage[i] = (blocks[i].GetVal(variable, Expression::timing::past)) + blocks[i].GetInflowValue(variable, Expression::timing::present)*dt();

    for (unsigned int i = 0; i < links.size(); i++)
    {
        if (blocks[links[i].s_Block_No()].GetLimitedOutflow() && links[i].GetVal(blocks[links[i].s_Block_No()].Variable(variable)->GetCorrespondingFlowVar(), Expression::timing::present) > 0)
            links[i].SetOutflowLimitFactor(blocks[links[i].s_Block_No()].GetOutflowLimitFactor(Expression::timing::present), Expression::timing::present);
        else if (blocks[links[i].e_Block_No()].GetLimitedOutflow() && links[i].GetVal(blocks[links[i].e_Block_No()].Variable(variable)->GetCorrespondingFlowVar(), Expression::timing::present) < 0)
            links[i].SetOutflowLimitFactor(blocks[links[i].e_Block_No()].GetOutflowLimitFactor(Expression::timing::present), Expression::timing::present);
        else
            links[i].SetOutflowLimitFactor(1, Expression::timing::present);

    }

    for (unsigned int i = 0; i < links.size(); i++)
    {
        UpdatedStorage[links[i].s_Block_No()] -= links[i].GetVal(blocks[links[i].s_Block_No()].Variable(variable)->GetCorrespondingFlowVar(), Expression::timing::present) * links[i].GetOutflowLimitFactor(Expression::timing::present)* dt();
        UpdatedStorage[links[i].e_Block_No()] += links[i].GetVal(blocks[links[i].s_Block_No()].Variable(variable)->GetCorrespondingFlowVar(), Expression::timing::present) * links[i].GetOutflowLimitFactor(Expression::timing::present)* dt();
    }
    
    for (unsigned int i = 0; i < blocks.size(); i++)
        blocks[i].SetVal(variable, UpdatedStorage[i], Expression::timing::present);
    
}

bool System::CalculateFlows(const string &var, const Expression::timing &tmg)
{
    for (int i=0; i<links.size(); i++)
    {
        links[i].SetVal(var,links[i].CalcVal(var,tmg));
    }
	return true;
}

CMatrix_arma System::Jacobian(const string &variable, CVector_arma &X)
{
    CMatrix_arma M(X.num);

    CVector_arma F0 = GetResiduals(variable, X);
    for (int i=0; i < X.num; i++)
    {
        CVector_arma V = Jacobian(variable, X, F0, i);
        for (int j=0; j<X.num; j++)
            M(i,j) = V[j];
    }

  return Transpose(M);
}


CVector_arma System::Jacobian(const string &variable, CVector_arma &V, CVector_arma &F0, int i)  //Works also w/o reference (&)
{
  double epsilon;
  epsilon = -1e-6*(fabs(V[i])+1);
  CVector_arma V1(V);
  V1[i] += epsilon;
  CVector_arma F1;
  F1 = GetResiduals(variable,V1);
  CVector_arma grad = (F1 - F0) / epsilon;
  if (grad.norm2() == 0 || !grad.is_finite())
  {
    epsilon = +1e-6*(fabs(V[i]) + 1);
    V1 = V;
    V1[i] += epsilon;
    F1 = GetResiduals(variable,V1);
    grad = (F1 - F0) / epsilon;
  }
  return grad;

}

void System::SetVariableParents()
{
    for (unsigned int i = 0; i < blocks.size(); i++) blocks[i].ClearLinksToFrom(); 
    
    for (unsigned int i = 0; i < links.size(); i++)
	{
		links[i].SetVariableParents();
        links[i].Set_s_Block(&blocks[int(links[i].s_Block_No())]);
		links[i].Set_e_Block(&blocks[links[i].e_Block_No()]);
        blocks[links[i].e_Block_No()].AppendLink(i, Expression::loc::destination);
        blocks[links[i].s_Block_No()].AppendLink(i, Expression::loc::source);
    }


	for (unsigned int i = 0; i < blocks.size(); i++)
	{
		blocks[i].SetVariableParents();
	}

	for (unsigned int i = 0; i < sources.size(); i++)
	{
		sources[i].SetVariableParents();
	}
}

vector<string> System::GetAllBlockTypes()
{
    vector<string> out;
    for (map<string, QuanSet>::iterator it = metamodel.GetMetaModel()->begin(); it != metamodel.GetMetaModel()->end(); it++)
        if (it->second.BlockLink == blocklink::block)
        {
            ShowMessage(it->first);
            out.push_back(it->first);
        }

    return out;

}

vector<string> System::GetAllLinkTypes()
{
    vector<string> out;
    for (map<string, QuanSet>::iterator it = metamodel.GetMetaModel()->begin(); it != metamodel.GetMetaModel()->end(); it++)
        if (it->second.BlockLink == blocklink::link)
        {
            ShowMessage(it->first);
            out.push_back(it->first);
        }

    return out;
}

vector<string> System::GetAllSourceTypes()
{
	vector<string> out;
	for (map<string, QuanSet>::iterator it = metamodel.GetMetaModel()->begin(); it != metamodel.GetMetaModel()->end(); it++)
		if (it->second.BlockLink == blocklink::source)
		{
			ShowMessage(it->first);
			out.push_back(it->first);
		}

	return out;
}

vector<string> System::GetAllSourceNames()
{
    vector<string> out;
    for (int i=0; i<sources.size(); i++)
        out.push_back(sources[i].GetName());
    return out;
}

vector<string> System::GetAllBlockNames()
{
    vector<string> out;
    for (int i=0; i<blocks.size(); i++)
        out.push_back(blocks[i].GetName());
    return out;
}

vector<string> System::GetAllLinkNames()
{
    vector<string> out;
    for (int i=0; i<links.size(); i++)
        out.push_back(links[i].GetName());
    return out;
}


vector<string> System::GetAllTypesOf(const string &type)
{
	vector<string> out;
	for (map<string, QuanSet>::iterator it = metamodel.GetMetaModel()->begin(); it != metamodel.GetMetaModel()->end(); it++)
		if (it->second.CategoryType() == type)
		{
			ShowMessage(it->first);
			out.push_back(it->first);
		}

	return out;
}

void System::clear()
{
    blocks.clear();
    links.clear();
    Outputs.AllOutputs.clear();
    Outputs.ObservedOutputs.clear();
    sources.clear();
    objective_function_set.clear();
    parameter_set.clear();
}

void System::TransferQuantitiesFromMetaModel()
{
    solvevariableorder = metamodel.solvevariableorder;
	SetNumberOfStateVariables(solvevariableorder.size()); // The size of the SolutionTemporaryVariables are adjusted based on the number of state variables.
    vector<string> out;
    for (map<string, QuanSet>::iterator it = metamodel.GetMetaModel()->begin(); it != metamodel.GetMetaModel()->end(); it++)
        GetVars()->Append(it->second);
}

void System::AppendQuantitiesFromMetaModel()
{
    for (map<string, QuanSet>::iterator it = metamodel.GetMetaModel()->begin(); it != metamodel.GetMetaModel()->end(); it++)
    {
        if (Variable(it->first)==nullptr)
            GetVars()->Append(it->second);
    }
}


void System::AppendObjectiveFunction(const string &name, const Objective_Function &obj, double weight)
{
    objective_function_set.Append(name,obj, weight);
    objective_function_set[name]->SetSystem(this);
    return;
}

bool System::AppendObjectiveFunction(const string &name, const string &location, const Expression &expr, double weight)
{
    Objective_Function obj(this,expr,location);
    obj.SetSystem(this);
    if (object(location)!=nullptr)
    {
        obj.SetQuantities(metamodel, "Objective_Function");
        objective_function_set.Append(name,obj, weight);
        objective_function_set[name]->SetSystem(this);
        return true;
    }

    errorhandler.Append(name,"System","AppendObjectiveFunction","Location '" + name + "' was not found", 501);
    return false;
}


void System::UpdateObjectiveFunctions(double t)
{
    objective_function_set.Update(t);
    return;
}
double System::GetObjectiveFunctionValue()
{
    return objective_function_set.Calculate();
}

bool System::RemoveAsParameter(const string &location, const string &quantity, const string &parametername)
{
    if (GetParameter(parametername)!=nullptr)
        return GetParameter(parametername)->RemoveLocationQuan(location,quantity);
    return false;
}
bool System::SetAsParameter(const string &location, const string &quantity, const string &parametername)
{
    if (object(location)==nullptr)
    {
        errorhandler.Append(GetName(),"System","SetAsParameter","Location '" + location + "' does not exist", 601);
        return false;
    }
    if (GetParameter(parametername) == nullptr)
    {
        errorhandler.Append(GetName(),"System","SetAsParameter","Parameter " + parametername + "does not exist!", 602);
        return false;
    }
    if (link(location)!=nullptr)
    {
        if (!link(location)->HasQuantity(quantity))
        {
            errorhandler.Append(GetName(),"System","SetAsParameter","In link" + location + ": variable " + quantity + " does not exist!", 603);
            return false;
        }
        else
        {
            GetParameter(parametername)->AppendLocationQuan(location, quantity);
            return true;
        }
    }

    if (block(location)!=nullptr)
    {
        if (!block(location)->HasQuantity(quantity))
        {
            lasterror() = "In block" + location + ": variable " + quantity + " does not exist";
            errorhandler.Append(GetName(),"System","SetAsParameter",lasterror(), 604);
            return false;
        }
        else
        {
            GetParameter(parametername)->AppendLocationQuan(location, quantity);
            return true;
        }
    }
}

bool System::AppendParameter(const string &paramname, const double &lower_limit, const double &upper_limit, const string &prior_distribution)
{
    if (Parameters().Contains(paramname))
    {
        errorhandler.Append(GetName(),"System","AppendParameter","Parameter '" + paramname + "' has already been defined!", 605);
        return false;
    }
    else
    {
        Parameter param;
        param.SetType("Parameter");
        param.SetRange(lower_limit, upper_limit);
        param.SetPriorDistribution(prior_distribution);
        param.SetQuantities(metamodel, "Parameter");

        Parameters().Append(paramname,param);
        return true;
    }
}

bool System::AppendParameter(const string &paramname, const Parameter& param)
{
    if (Parameters().Contains(paramname))
    {
        errorhandler.Append(GetName(),"System","AppendParameter","Parameter " + paramname + " already exists!", 606);
        return false;
    }
    else
    {
        Parameters().Append(paramname,param);
        return true;
    }
}

bool System::SetParameterValue(const string &paramname, const double &val)
{
    if (!Parameters().Contains(paramname))
    {
        errorhandler.Append(GetName(),"System","SetParameterValue","Parameter " + paramname + " does not exist!", 608);
        return false;
    }
    else
    {
        GetParameter(paramname)->SetValue(val);
        return true;
    }
}

bool System::SetParameterValue(int i, const double &val)
{
    GetParameter(Parameters().getKeyAtIndex(i))->SetValue(val);
	return true;
}

bool System::ApplyParameters()
{
    for (unsigned int i = 0; i < Parameters().size(); i++)
        for (unsigned int j=0; j<GetParameter(i)->GetLocations().size();j++)
        {
            if (object(GetParameter(i)->GetLocations()[j])!=nullptr)
                object(GetParameter(i)->GetLocations()[j])->SetVal(GetParameter(i)->GetQuans()[i],GetParameter(i)->GetValue());
            else
            {
                errorhandler.Append(GetName(),"System","ApplyParameters" ,"Location '" + GetParameter(i)->GetLocations()[j] + "' does not exist!", 607);
                return false;
            }
        }
    return true;

}

void System::SetAllParents()
{
    SetVariableParents();
    for (unsigned int i = 0; i < links.size(); i++)
        links[i].SetParent(this);

    for (unsigned int i = 0; i < blocks.size(); i++)
        blocks[i].SetParent(this);

    for (unsigned int i = 0; i < sources.size(); i++)
        sources[i].SetParent(this);
    for (unsigned int i = 0; i < objective_function_set.size(); i++)
    {
        objective_function_set[i]->SetParent(this);
        objective_function_set[i]->SetSystem(this);
    }
    for (unsigned int i = 0; i < ParametersCount(); i++)
    {
        Parameters()[i]->SetParent(this);
        //Parameters()[i]->SetSystem(this);
    }

}

bool System::Echo(const string &obj, const string &quant, const string &feature)
{
    if (object(obj)==nullptr && parameter(obj)==nullptr)
    {
        errorhandler.Append(GetName(),"System","Echo" ,"Object or parameter '" + obj + "' does not exits!", 608);
        return false;
    }
    if (quant=="")
    {
        if (object(obj)!=nullptr)
        {
            cout<<object(obj)->toString()<<std::endl;
            return true;
        }
        else if (parameter(obj)!=nullptr)
        {
            cout<<parameter(obj)->toString()<<std::endl;
            return true;
        }
        else
        {
            errorhandler.Append(GetName(),"System","Echo" ,"Object or parameter '" + obj + "' does not exits!", 608);
            return false;
        }
    }
    else
    {
        if (object(obj)!=nullptr)
        {
            if (!object(obj)->HasQuantity(quant))
            {
                errorhandler.Append(GetName(),"System","Echo","Object '" + obj + "' has no property/quantity '" + quant + "'",613);
                return false;
            }
            if (feature == "")
                cout<<object(obj)->Variable(quant)->ToString()<<std::endl;
            else if (aquiutils::tolower(feature) == "value")
                cout<<object(obj)->Variable(quant)->GetVal();
            else if (aquiutils::tolower(feature) == "rule")
                cout<<object(obj)->Variable(quant)->GetRule()->ToString();
            else if (aquiutils::tolower(feature) == "type")
                cout<<tostring(object(obj)->Variable(quant)->GetType());
            else
            {
                errorhandler.Append(GetName(),"System","Echo","Feature '" + feature + "' is not valid!",612);
                return false;
            }
            return true;
        }
        else if (parameter(obj)!=nullptr)
        {
            if (!parameter(obj)->HasQuantity(quant))
            {
                errorhandler.Append(GetName(),"System","Echo","Parameter '" + obj + "' has no property/quantity '" + quant + "'",618);
                return false;
            }
            else
            {
                cout<<parameter(obj)->Variable(quant)<<std::endl;
                return true;
            }
        }
    }


}

vector<CTimeSeries*> System::TimeSeries()
{
    vector<CTimeSeries*> out;
    for (unsigned int i=0; i<links.size(); i++)
    {
        for (int j=0; j<links[i].TimeSeries().size(); j++)
        {
            links[i].TimeSeries()[j]->assign_D();
            out.push_back(links[i].TimeSeries()[j]);
        }
    }

    for (unsigned int i=0; i<blocks.size(); i++)
    {
        for (int j=0; j<blocks[i].TimeSeries().size(); j++)
        {
            blocks[i].TimeSeries()[j]->assign_D();
            out.push_back(blocks[i].TimeSeries()[j]);
        }
    }

    for (unsigned int i=0; i<sources.size(); i++)
    {
        for (int j=0; j<sources[i].TimeSeries().size(); j++)
        {
            sources[i].TimeSeries()[j]->assign_D();
            out.push_back(sources[i].TimeSeries()[j]);
        }
    }

    return out;
}

double System::GetMinimumNextTimeStepSize()
{
    double x=1e12;

    for (int i=0; i<alltimeseries.size(); i++)
    {
        x = min(x,alltimeseries[i]->interpol_D(this->SolverTempVars.t));
    }
    return x;
}

#if defined(QT_version) || defined(Q_version)
QStringList System::QGetAllCategoryTypes()
{
	QStringList out; 
	for (map<string, QuanSet>::iterator it = metamodel.GetMetaModel()->begin(); it != metamodel.GetMetaModel()->end(); it++)
	{
		if (!out.contains(QString::fromStdString(it->second.CategoryType())))
			out.append(QString::fromStdString(it->second.CategoryType()));
	}

	return out; 
}

QStringList System::QGetAllObjectsofTypeCategory(QString _type)
{
	QStringList out;
	if (_type == "Blocks")
		for (unsigned int i = 0; i < blocks.size(); i++)
			out.append(QString::fromStdString(blocks[i].GetName()));

	if (_type == "Connectors")
		for (unsigned int i = 0; i < links.size(); i++)
			out.append(QString::fromStdString(links[i].GetName()));
		
	if (_type == "Sources")
		for (unsigned int i = 0; i < sources.size(); i++)
			out.append(QString::fromStdString(sources[i].GetName()));


	if (_type == "Parameters")
		for (unsigned int i = 0; i < Parameters().size()  ; i++)
			out.append(QString::fromStdString(Parameters().getKeyAtIndex(i)));
	
	if (_type == "Objective_Functions")
		
	return out;
}
#endif // Qt_version


bool System::SavetoScriptFile(const string &filename, const string &templatefilename, const vector<string> addedtemplates)
{

    fstream file(filename,ios_base::out);
    if (templatefilename!="")
    {
        file << "loadtemplate; filename = " << templatefilename << std::endl;
    }

    for (int i=0; i<addedtemplates.size();i++)
    {
        file << "addtemplate; filename = " << addedtemplates[i] << std::endl;
    }

    for (unsigned int i=0; i<Settings.size(); i++)
        for (map<string, Quan>::iterator j=Settings[i].GetVars()->begin(); j!=Settings[i].GetVars()->end(); j++)
            if (j->second.AskFromUser())
                file << "setvalue; object=system, quantity=" + j->first + ", value=" << j->second.GetProperty() << std::endl;

    for (unsigned int i=0; i<sources.size(); i++)
        file << "create source;" << sources[i].toCommand() << std::endl;

    for (unsigned int i=0; i<ParametersCount(); i++)
        file << "create parameter;" << Parameters()[i]->toCommand() << std::endl;

    for (unsigned int i=0; i<blocks.size(); i++)
        file << "create block;" << blocks[i].toCommand() << std::endl;

    for (unsigned int i=0; i<links.size(); i++)
        file << "create link;" << links[i].toCommand() << std::endl;

    for (unsigned int i=0; i<blocks.size(); i++)
        if (blocks[i].toCommandSetAsParam()!="")
			file << blocks[i].toCommandSetAsParam() << std::endl;

    for (unsigned int i=0; i<links.size(); i++)
		if (links[i].toCommandSetAsParam() != "")
			file << links[i].toCommandSetAsParam() << std::endl;

    for (unsigned int i=0; i<ObjectiveFunctionsCount(); i++)
        file << "create objectivefunction;" << ObjectiveFunctions()[i]->toCommand() << std::endl;

    file.close();

    return true; 
}

System::System(Script& scr)
{
    for (int i=0; i<scr.CommandsCount(); i++)
    {
        if (!scr[i]->Execute(this))
        {
            errorhandler.Append("","Script","CreateSystem",scr[i]->LastError(),6001);
            scr.Errors().push_back(scr[i]->LastError());
        }
    }
    PopulateOperatorsFunctions();
    SetVariableParents();
    Object::AssignRandomPrimaryKey();

}

void System::CreateFromScript(Script& scr, const string& settingsfilename)
{
    for (int i=0; i<scr.CommandsCount(); i++)
    {
        if (!scr[i]->Execute(this))
        {
            errorhandler.Append("","Script","CreateSystem",scr[i]->LastError(),6001);
            scr.Errors().push_back(scr[i]->LastError());
        }
        if (scr[i]->Keyword() == "loadtemplate")
        {
            if (settingsfilename!="")
                ReadSystemSettingsTemplate(settingsfilename);
        }
    }
    PopulateOperatorsFunctions();
    SetVariableParents();

}

bool System::ReadSystemSettingsTemplate(const string &filename)
{
    Settings.clear();
    Json::Value root;
    Json::Reader reader;

    std::ifstream file(filename);
    if (!file.good())
    {
        cout << "File " + filename + " was not found!";
        return false;
    }

    file >> root;

    if (!reader.parse(file, root, true)) {
        //for some reason it always fails to parse
        std::cout << "Failed to parse configuration\n"
            << reader.getFormattedErrorMessages();
        lasterror() = "Failed to parse configuration\n";
    }

    Settings.clear();
    for (Json::ValueIterator object_types = root.begin(); object_types != root.end(); ++object_types)
    {
        QuanSet quanset(object_types);
        Object settingsitem;
        quanset.SetParent(this);
        settingsitem.SetQuantities(quanset);
        settingsitem.SetDefaults();
        Settings.push_back(settingsitem);
        metamodel.Append(object_types.key().asString(), quanset);

    }
    return true;
}

void System::SetSystemSettings()
{
    for (unsigned int i=0; i<Settings.size(); i++)
    {
        for (map<string, Quan>::iterator j=Settings[i].GetVars()->begin(); j!=Settings[i].GetVars()->end(); j++)
            SetProperty(j->first,j->second.GetProperty());

    }

}

void System::DisconnectLink(const string linkname)
{
    for (unsigned int i = 0; i < links.size(); i++)
        if (links[i].GetName() == linkname)
        {
            block(links[i].GetConnectedBlock(Expression::loc::source)->GetName())->deletelinkstofrom(links[i].GetName());
        }
}

bool System::Delete(const string& objectname)
{
    for (unsigned int i = 0; i < links.size(); i++)
        if (links[i].GetName() == objectname)
        {
            block(links[i].GetConnectedBlock(Expression::loc::source)->GetName())->deletelinkstofrom(links[i].GetName());
            for (unsigned int j = 0; j < blocks.size(); j++)
            {
                blocks[j].shiftlinkIds(i);
            }
            links.erase(links.begin()+i);

            return true;
        }

    for (unsigned int i = 0; i < blocks.size(); i++)
        if (blocks[i].GetName() == objectname)
        {
            vector<string> links_to_be_deleted; 
            for (unsigned int j = 0; j < blocks[i].GetLinksFrom().size(); j++)
                links_to_be_deleted.push_back(blocks[i].GetLinksFrom()[j]->GetName());
                
            for (unsigned int j = 0; j < blocks[i].GetLinksTo().size(); j++)
                links_to_be_deleted.push_back(blocks[i].GetLinksTo()[j]->GetName());

            //for (unsigned int j = 0; j < links_to_be_deleted.size(); j++)
            //    DisconnectLink(links_to_be_deleted[j]);

            for (unsigned int j = 0; j < links_to_be_deleted.size(); j++)
                Delete(links_to_be_deleted[j]);

            blocks.erase(blocks.begin() + i);
            return true; 
        }

    for (unsigned int i = 0; i < sources.size(); i++)
        if (sources[i].GetName() == objectname)
        {
            sources.erase(sources.begin() + i);
            return true; 
        }

    for (unsigned int i = 0; i < ParametersCount(); i++)
        if (Parameters()[i]->GetName() == objectname)
        {
            return Parameters().erase(i);
        }

    for (unsigned int i = 0; i < ObjectiveFunctionsCount(); i++)
        if (ObjectiveFunctions()[i]->GetName() == objectname)
        {
            return ObjectiveFunctions().erase(i);
        }

    errorhandler.Append(GetName(),"System","object","Object '" + objectname + "' was not found",105);

    return false;

}

bool System::VerifyAsSource(Block* blk, Link* lnk)
{
    for (unsigned int i = 0; i < lnk->GetAllRequieredStartingBlockProperties().size(); i++)
    {
        if (!blk->HasQuantity(lnk->GetAllRequieredStartingBlockProperties()[i]))
        {
            errorhandler.Append(lnk->GetName(), "System", "VerifyAsSource", "The source block must have a '" + lnk->GetAllRequieredStartingBlockProperties()[i] + "' property",106);
            lasterror() = "The source block must have a '" + lnk->GetAllRequieredStartingBlockProperties()[i] + "' property";
            return false; 
        }
    }
    return true; 
}


bool System::VerifyAsDestination(Block* blk, Link* lnk)
{
    for (unsigned int i = 0; i < lnk->GetAllRequieredDestinationBlockProperties().size(); i++)
    {
        if (!blk->HasQuantity(lnk->GetAllRequieredDestinationBlockProperties()[i]))
        {
            errorhandler.Append(lnk->GetName(), "System", "VerifyAsSource", "The destination block must have a '" + lnk->GetAllRequieredStartingBlockProperties()[i] + "' property", 106);
            lasterror() = "The destination block must have a '" + lnk->GetAllRequieredStartingBlockProperties()[i] + "' property";
            return false;
        }
    }
    return true;

}

void System::AddPropertytoAllBlocks(const string &name, Quan &quan)
{
    if (addedpropertiestoallblocks.count(name)==0)
        addedpropertiestoallblocks[name] = quan;
    else
        errorhandler.Append("System", "System", "AddPropertytoAllBlocks", "Property '" + name + "' has already been added to all blocks", 7649);
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        if (!blocks[i].HasQuantity(name))
        {
            blocks[i].AddQnantity(name, quan);
        }
    }
}
void System::AddPropertytoAllLinks(const string &name, Quan &quan)
{
    if (addedpropertiestoalllinks.count(name)==0)
        addedpropertiestoalllinks[name] = quan;
    else
        errorhandler.Append("System", "System", "AddPropertytoAllLinks", "Property '" + name + "' has already been added to all links", 7648);
    for (unsigned int i=0; i<links.size(); i++)
    {
        if (!links[i].HasQuantity(name))
        {
            links[i].AddQnantity(name, quan);
        }
    }
}
void System::UpdateAddedPropertiestoAllBlockLinks()
{
    for (map<string,Quan>::iterator it = addedpropertiestoallblocks.begin(); it!=addedpropertiestoallblocks.end(); it++  )
    {
        for (unsigned int i=0; i<blocks.size(); i++)
        {
            if (!blocks[i].HasQuantity(it->first))
            {
                blocks[i].AddQnantity(it->first, it->second);
            }
        }
    }

    for (map<string,Quan>::iterator it = addedpropertiestoalllinks.begin(); it!=addedpropertiestoalllinks.end(); it++  )
    {
        for (unsigned int i=0; i<links.size(); i++)
        {
            if (!links[i].HasQuantity(it->first))
            {
                links[i].AddQnantity(it->first, it->second);
            }
        }
    }
}

