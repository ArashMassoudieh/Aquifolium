#include "System.h"
#include <fstream>
#pragma warning(pop)
#pragma warning(disable : 4996)
#include <json/json.h>



#ifdef QT_version
#include "node.h"
#include "edge.h"
#include "GWidget.h"
#include "QDebug"
#include "logwindow.h"
#endif

System::System():Object::Object()
{
    //ctor
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
	SolverTempVars.SetUpdateJacobian(true);
	blocks = other.blocks;
    links = other.links;
    sources = other.sources;
    objective_function_set = other.objective_function_set;
    parameter_set = other.parameter_set;
    silent = other.silent;
    SimulationParameters = other.SimulationParameters;
    SolverSettings = other. SolverSettings;
    solvevariableorder = other.solvevariableorder;
    paths = other.paths;
    SetAllParents();
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
    paths = rhs.paths;
    SetAllParents();
    return *this;
}

bool System::AddBlock(Block &blk)
{
    blocks.push_back(blk);
    block(blk.GetName())->SetParent(this);
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
    links.push_back(lnk);
    link(lnk.GetName())->SetParent(this);
    link(lnk.GetName())->SetConnectedBlock(Expression::loc::source, source);
    link(lnk.GetName())->SetConnectedBlock(Expression::loc::destination, destination);
    block(source)->AppendLink(link(lnk.GetName()),Expression::loc::source);
    block(destination)->AppendLink(link(lnk.GetName()),Expression::loc::destination);
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

Object *System::object(const string &s)
{
    for (unsigned int i=0; i<links.size(); i++)
        if (links[i].GetName() == s) return &links[i];

    for (unsigned int i=0; i<blocks.size(); i++)
        if (blocks[i].GetName() == s) return &blocks[i];

    for (unsigned int i=0; i<sources.size(); i++)
        if (sources[i].GetName() == s) return &sources[i];

    //errorhandler.Append(GetName(),"System","object","Object '" + s + "' was not found",105);

    return nullptr;
}

bool System::GetQuanTemplate(const string &filename)
{
    metamodel.GetFromJsonFile(filename);
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
    #ifdef QT_version
    if (LogWindow())
    {
        LogWindow()->append("Simulation started!");
    }
    #else
        ShowMessage("Simulation started!");
    #endif

	SolverTempVars.SetUpdateJacobian(true);
	alltimeseries = TimeSeries();
	bool success = true;

	if (applyparameters) ApplyParameters();
    InitiateOutputs();

    SolverTempVars.dt_base = SimulationParameters.dt0;
    SolverTempVars.dt = SolverTempVars.dt_base;
    SolverTempVars.t = SimulationParameters.tstart;
	PopulateOutputs();

    while (SolverTempVars.t<SimulationParameters.tend+SolverTempVars.dt)
    {
		cout << "\r Simulation Time: " + aquiutils::numbertostring(SolverTempVars.t);
		SolverTempVars.dt = min(SolverTempVars.dt_base,GetMinimumNextTimeStepSize());
        if (SolverTempVars.dt<SimulationParameters.dt0/100) SolverTempVars.dt=SimulationParameters.dt0/100;
        #ifdef Debug_mode
        ShowMessage(string("t = ") + aquiutils::numbertostring(SolverTempVars.t) + ", dt_base = " + aquiutils::numbertostring(SolverTempVars.dt_base) + ", dt = " + aquiutils::numbertostring(SolverTempVars.dt) + ", SolverTempVars.numiterations =" + aquiutils::numbertostring(SolverTempVars.numiterations));
        #endif // Debug_mode
        #ifdef QT_version
        if (rtw)
        {
            updateProgress(false);
        }
        #endif

        vector<bool> _success = OneStepSolve();
		success = aquiutils::And(_success);
		if (!success)
        {
            #ifdef Debug_mode
            ShowMessage("failed!");
            #endif // Debug_mode
            SolverTempVars.dt_base *= SolverSettings.NR_timestep_reduction_factor_fail;
            SolverTempVars.SetUpdateJacobian(true);
        }
        else
        {
            SolverTempVars.t += SolverTempVars.dt;
            if (SolverTempVars.MaxNumberOfIterations()>SolverSettings.NR_niteration_upper)
            {
                SolverTempVars.dt_base = max(SolverTempVars.dt*SolverSettings.NR_timestep_reduction_factor,SolverSettings.minimum_timestep);
                SolverTempVars.SetUpdateJacobian(true);
            }
            if (SolverTempVars.MaxNumberOfIterations()<SolverSettings.NR_niteration_lower)
                SolverTempVars.dt_base = min(SolverTempVars.dt_base/SolverSettings.NR_timestep_reduction_factor,SimulationParameters.dt0*10);
            PopulateOutputs();
            for (int i=0; i<solvevariableorder.size(); i++)
			Update(solvevariableorder[i]);
            UpdateObjectiveFunctions(SolverTempVars.t);
        }

    }

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
            QString reason = QString::fromStdString(SolverTempVars.fail_reason);
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

bool System::SetProperty(const string &s, const string &val)
{
    if (s=="cn_weight")
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

    if (s=="tstart")
    {   SimulationParameters.tstart = aquiutils::atof(val); return true;}
    if (s=="tend")
    {   SimulationParameters.tend = aquiutils::atof(val); return true;}
    if (s=="dt")
    {   SimulationParameters.dt0 = aquiutils::atof(val); return true;}
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
                Outputs.AllOutputs.append(CBTC(), blocks[i].GetName() + "_" + it->first);
    }

    for (unsigned int i=0; i<links.size(); i++)
    {
        for (map<string, Quan>::iterator it = links[i].GetVars()->begin(); it != links[i].GetVars()->end(); it++)
            if (it->second.IncludeInOutput())
                Outputs.AllOutputs.append(CBTC(), links[i].GetName() + "_" + it->first);
    }

    for (map<string, obj_funct_weight>::iterator it=objective_function_set.begin(); it !=objective_function_set.end(); it++)
    {
        Outputs.AllOutputs.append(CBTC(), "Obj_" + it->first);
    }

}


void System::PopulateOutputs()
{
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        for (map<string, Quan>::iterator it = blocks[i].GetVars()->begin(); it != blocks[i].GetVars()->end(); it++)
            if (it->second.IncludeInOutput())
                Outputs.AllOutputs[blocks[i].GetName() + "_" + it->first].append(SolverTempVars.t,blocks[i].GetVal(it->first,Expression::timing::present));
    }

    for (unsigned int i=0; i<links.size(); i++)
    {
        for (map<string, Quan>::iterator it = links[i].GetVars()->begin(); it != links[i].GetVars()->end(); it++)
            if (it->second.IncludeInOutput())
            {
                Outputs.AllOutputs[links[i].GetName() + "_" + it->first].append(SolverTempVars.t,links[i].GetVal(it->first,Expression::timing::present,true));
            }
    }

    for (map<string, obj_funct_weight>::iterator it=objective_function_set.begin(); it !=objective_function_set.end(); it++)
    {
        Outputs.AllOutputs["Obj_" + it->first].append(SolverTempVars.t,ObjectiveFunction(it->first)->Value());
    }

}


bool System::OneStepSolve(int statevarno)
{
	string variable = solvevariableorder[statevarno];
	Renew(variable);


    SolverTempVars.numiterations[statevarno] = 0;
    bool switchvartonegpos = true;
    int attempts = 0;
    while (attempts<2 && switchvartonegpos)
    {
		CVector_arma X = GetStateVariables(variable, Expression::timing::past);
		for (int i = 0; i < blocks.size(); i++)
		{
			if (blocks[i].GetLimitedOutflow())
				X[i] = blocks[i].GetOutflowLimitFactor(Expression::timing::past);
		}

		CVector_arma X_past = X;

		CVector_arma F = GetResiduals(variable, X);

		double err_ini = F.norm2();
		double err;
		double err_p = err = err_ini;

		while ((err/err_ini>SolverSettings.NRtolerance && err>1e-12) || SolverTempVars.numiterations[statevarno]>SolverSettings.NR_niteration_max)
        {
            SolverTempVars.numiterations[statevarno]++;
            if (SolverTempVars.updatejacobian[statevarno])
            {
                SolverTempVars.Inverse_Jacobian[statevarno] = Invert(Jacobian(variable,X));
                SolverTempVars.updatejacobian[statevarno] = false;
                SolverTempVars.NR_coefficient[statevarno] = 1;
            }
            X = X - SolverTempVars.NR_coefficient[statevarno]*SolverTempVars.Inverse_Jacobian[statevarno]*F;
			if (!X.is_finite())
			{
				SolverTempVars.fail_reason.push_back("at " + aquiutils::numbertostring(SolverTempVars.t) + ": X is infinite");
				return false;
			}

			F = GetResiduals(variable, X);
			if (!F.is_finite())
			{
				SolverTempVars.fail_reason.push_back("at " + aquiutils::numbertostring(SolverTempVars.t) + ": F is infinite");
				return false;
			}
            err_p = err;
            err = F.norm2();
            #ifdef Debug_mode
            //ShowMessage(numbertostring(err));
            #endif // Debug_mode
			if (err > err_p)
			{
				SolverTempVars.NR_coefficient[statevarno] *= SolverSettings.NR_coeff_reduction_factor;
				SolverTempVars.updatejacobian[statevarno] = true;
				X = X_past;
			}
            //else
            //    SolverTempVars.NR_coefficient/=SolverSettings.NR_coeff_reduction_factor;
            if (SolverTempVars.numiterations[statevarno]>SolverSettings.NR_niteration_max)
                return false;
        }
        switchvartonegpos = false;
        for (unsigned int i=0; i<blocks.size(); i++)
        {
            if (X[i]<-1e-13 && !blocks[i].GetLimitedOutflow())
            {
                blocks[i].SetLimitedOutflow(true);
                switchvartonegpos = true;
                SolverTempVars.updatejacobian[statevarno] = true;
				attempts++;
            }
            else if (X[i]>1 && blocks[i].GetLimitedOutflow())
            {
                blocks[i].SetLimitedOutflow(false);
                switchvartonegpos = true;
                SolverTempVars.updatejacobian[statevarno] = true;
				attempts++;
            }
        }
    }
	if (attempts == 2)
	{
		SolverTempVars.fail_reason.push_back("at " + aquiutils::numbertostring(SolverTempVars.t) + ": attempts > 1");
		return false;
	}
	if (SolverTempVars.numiterations[statevarno] > SolverSettings.NR_niteration_max)
	{
		SolverTempVars.fail_reason.push_back("at " + aquiutils::numbertostring(SolverTempVars.t) + ": number of iterations exceeded the limit");
		return false;
	}
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


CVector_arma System::GetResiduals(const string &variable, CVector_arma &X)
{
    CVector_arma F(blocks.size());
    SetStateVariables(variable,X,Expression::timing::present);
    CalculateFlows(Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present);

    for (unsigned int i=0; i<blocks.size(); i++)
    {
        if (blocks[i].GetLimitedOutflow())
        {
            blocks[i].SetOutflowLimitFactor(X[i],Expression::timing::present);
            blocks[i].SetVal(variable,0);
            F[i] = (0-blocks[i].GetVal(variable,Expression::timing::past))/dt() - blocks[i].GetInflowValue(variable,Expression::timing::present);
        }
        else
            F[i] = (X[i]-blocks[i].GetVal(variable,Expression::timing::past))/dt() - blocks[i].GetInflowValue(variable,Expression::timing::present);
    }


    for (unsigned int i=0; i<links.size(); i++)
    {
        if (blocks[links[i].s_Block_No()].GetLimitedOutflow() && links[i].GetVal(blocks[links[i].s_Block_No()].Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present)>0)
            links[i].SetOutflowLimitFactor(blocks[links[i].s_Block_No()].GetOutflowLimitFactor(Expression::timing::present), Expression::timing::present);
        if (blocks[links[i].e_Block_No()].GetLimitedOutflow() && links[i].GetVal(blocks[links[i].e_Block_No()].Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present)<0)
            links[i].SetOutflowLimitFactor(blocks[links[i].e_Block_No()].GetOutflowLimitFactor(Expression::timing::present), Expression::timing::present);

    }

    for (unsigned int i=0; i<links.size(); i++)
    {
        F[links[i].s_Block_No()] += links[i].GetVal(blocks[links[i].s_Block_No()].Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present)*links[i].GetOutflowLimitFactor(Expression::timing::present);
        F[links[i].e_Block_No()] -= links[i].GetVal(blocks[links[i].s_Block_No()].Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present)*links[i].GetOutflowLimitFactor(Expression::timing::present);
    }
    return F;
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
  epsilon = -1e-6;
  CVector_arma V1(V);
  V1[i] += epsilon;
  CVector_arma F1;
  F1 = GetResiduals(variable,V1);
  CVector_arma grad = (F1 - F0) / epsilon;
  if (grad.norm2() == 0)
  {
    epsilon = 1e-6;
    V1 = V;
    V1[i] += epsilon;
    F1 = GetResiduals(variable,V1);
    grad = (F1 - F0) / epsilon;
  }
  return grad;

}

void System::SetVariableParents()
{
	for (unsigned int i = 0; i < links.size(); i++)
	{
		links[i].SetVariableParents();
        links[i].Set_s_Block(&blocks[int(links[i].s_Block_No())]);
		links[i].Set_e_Block(&blocks[links[i].e_Block_No()]);
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

void System::clear()
{
    blocks.clear();
    links.clear();
    Outputs.AllOutputs.clear();
    Outputs.ObservedOutputs.clear();
}

void System::TransferQuantitiesFromMetaModel()
{
    solvevariableorder = metamodel.solvevariableorder;
	SetNumberOfStateVariables(solvevariableorder.size()); // The size of the SolutionTemporaryVariables are adjusted based on the number of state variables.
    vector<string> out;
    for (map<string, QuanSet>::iterator it = metamodel.GetMetaModel()->begin(); it != metamodel.GetMetaModel()->end(); it++)
        GetVars()->Append(it->second);
}

#ifdef QT_version
void System::GetModelConfiguration()
{
    QList <Node*> nodes = diagramview->Nodes();
    QStringList nodenames_sorted = diagramview->nodeNames();
    nodenames_sorted.sort();

    for (int i = 0; i < nodenames_sorted.count(); i++)
    {
        Node* n = nodes[diagramview->nodeNames().indexOf(nodenames_sorted[i])];
        Block B;
        B.SetName(n->Name().toStdString());
        B.SetType(n->GetObjectType().toStdString());
        AddBlock(B);
        QStringList codes = n->codes();

        foreach (mProp mP , n->getmList(n->objectType).GetList())
        {
            QString code = mP.VariableCode;
            if (!n->val(code).isEmpty() && n->val(code) != ".")
                block(n->Name().toStdString())->SetVal(code.toStdString(), n->val(code).toFloat());
            if (mP.Delegate == "Browser" && !n->val(code).isEmpty() && n->val(code) != ".")
            {   block(n->Name().toStdString())->Variable(code.toStdString())->SetTimeSeries(fullFilename(n->val(code), diagramview->modelPathname()).toStdString());
                qDebug() << n->val(code).toQString() << "   path:" << diagramview->modelPathname();
            }
            qDebug()<<code<<"  "<<QString::fromStdString(block(n->Name().toStdString())->GetName())<<"  "<<QString::fromStdString(block(n->Name().toStdString())->GetType())<<"    "<<block(n->Name().toStdString())->GetVal(code.toStdString());
        }



/*      foreach (QString code , n->codes()) //Parameters
        {
            if (gw->EntityNames("Parameter").contains(n->val(code).toQString()))
            {
                if (lookup_parameters(n->val(code).toStdString()) != -1) {
                    parameters()[lookup_parameters(n->val(code).toStdString())].location.push_back(Blocks.size() - 1);  // Check for everything
                    parameters()[lookup_parameters(n->val(code).toStdString())].conversion_factor.push_back(n->val(code).conversionCoefficient(n->val(code).unit, n->val(code).defaultUnit));
                    parameters()[lookup_parameters(n->val(code).toStdString())].quan.push_back(code.toStdString());
                    parameters()[lookup_parameters(n->val(code).toStdString())].location_type.push_back(0);
                    parameters()[lookup_parameters(n->val(code).toStdString())].experiment_id.push_back(name);
                }
            }
        } //Controller
        foreach (QString code , n->codes())
        {
            if (gw->EntityNames("Controller").contains(n->val(code).toQString()))
            {
                if (lookup_controllers(n->val(code).toStdString()) != -1) {
                    controllers()[lookup_controllers(n->val(code).toStdString())].application_spec.location.push_back(Blocks.size() - 1);  // Check for everything
                    controllers()[lookup_controllers(n->val(code).toStdString())].application_spec.conversion_factor.push_back(n->val(code).conversionCoefficient(n->val(code).unit, n->val(code).defaultUnit));
                    controllers()[lookup_controllers(n->val(code).toStdString())].application_spec.quan.push_back(code.toStdString());
                    controllers()[lookup_controllers(n->val(code).toStdString())].application_spec.location_type.push_back(0);
                    controllers()[lookup_controllers(n->val(code).toStdString())].application_spec.experiment_id.push_back(name);
                }
            }
        }
*/
    }

    QList <Edge*> edges = diagramview->Edges();
    QStringList edgenames_sorted = diagramview->edgeNames();
    edgenames_sorted.sort();
//#pragma omp parallel for
    for (int i = 0; i < edges.count(); i++)
    {
        Edge *e = edges[diagramview->edgeNames().indexOf(edgenames_sorted[i])];
        Link L;
        L.SetName(e->Name().toStdString());
        L.SetType(e->GetObjectType().toStdString());
        AddLink(L,e->sourceNode()->Name().toStdString(),e->destNode()->Name().toStdString());

        foreach (mProp mP ,e->getmList(e->objectType).GetList())
        {   QString code = mP.VariableCode;
            if (!e->val(code).isEmpty() && e->val(code) != ".") link(e->Name().toStdString())->SetVal(code.toStdString(), e->val(code).toFloat());
            if (mP.Delegate == "Browser" && !e->val(code).isEmpty() && e->val(code) != ".")
                link(e->Name().toStdString())->Variable(code.toStdString())->SetTimeSeries(fullFilename(e->val(code), diagramview->modelPathname()).toStdString());

        }


        //progress->setValue(progress->value() + 1);

/*
        foreach (QString code , e->codes()) //Parameters
        {
            if (gw->EntityNames("Parameter").contains(e->val(code).toQString()))
            {
                if (lookup_parameters(e->val(code).toStdString()) != -1) {
                    parameters()[lookup_parameters(e->val(code).toStdString())].location.push_back(Connectors.size() - 1);  // Check for everything
                    parameters()[lookup_parameters(e->val(code).toStdString())].conversion_factor.push_back(e->val(code).conversionCoefficient(e->val(code).unit, e->val(code).defaultUnit));
                    parameters()[lookup_parameters(e->val(code).toStdString())].quan.push_back(code.toStdString());
                    parameters()[lookup_parameters(e->val(code).toStdString())].location_type.push_back(1);
                    parameters()[lookup_parameters(e->val(code).toStdString())].experiment_id.push_back(name);
                }
            }
        }
        foreach (QString code , e->codes()) //Controllers
        {
            if (gw->EntityNames("Controller").contains(e->val(code).toQString()))
            {
                if (lookup_controllers(e->val(code).toStdString()) != -1) {
                    controllers()[lookup_controllers(e->val(code).toStdString())].application_spec.location.push_back(Connectors.size() - 1);  // Check for everything
                    controllers()[lookup_controllers(e->val(code).toStdString())].application_spec.conversion_factor.push_back(e->val(code).conversionCoefficient(e->val(code).unit, e->val(code).defaultUnit));
                    controllers()[lookup_controllers(e->val(code).toStdString())].application_spec.quan.push_back(code.toStdString());
                    controllers()[lookup_controllers(e->val(code).toStdString())].application_spec.location_type.push_back(1);
                    controllers()[lookup_controllers(e->val(code).toStdString())].application_spec.experiment_id.push_back(name);
                }
            }
        }
*/


    }
}
#endif

void System::AppendObjectiveFunction(const string &name, const Objective_Function &obj, double weight)
{
    objective_function_set.Append(name,obj, weight);
    objective_function_set[name]->obj_funct.SetSystem(this);
    return;
}

bool System::AppendObjectiveFunction(const string &name, const string &location, const Expression &expr, double weight)
{
    Objective_Function obj(this,expr,location);
    obj.SetSystem(this);
    if (object(location)!=nullptr)
    {
        objective_function_set.Append(name,obj, weight);
        objective_function_set[name]->obj_funct.SetSystem(this);
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
            last_error = "In block" + location + ": variable " + quantity + " does not exist";
            errorhandler.Append(GetName(),"System","SetAsParameter",last_error, 604);
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
        param.SetRange(lower_limit, upper_limit);
        param.SetPriorDistribution(prior_distribution);
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
    for (map<string, Parameter>::iterator it = Parameters().begin(); it != Parameters().end(); it++)
        for (int i=0; i<GetParameter(it->first)->GetLocations().size();i++)
        {
            if (object(GetParameter(it->first)->GetLocations()[i])!=nullptr)
                object(GetParameter(it->first)->GetLocations()[i])->SetVal(GetParameter(it->first)->GetQuans()[i],GetParameter(it->first)->GetValue());
            else
            {
                errorhandler.Append(GetName(),"System","ApplyParameters" ,"Location '" + GetParameter(it->first)->GetLocations()[i] + "' does not exist!", 607);
                return false;
            }
        }
    return true;

}

void System::SetAllParents()
{
    SetVariableParents();
    for (unsigned int i=0; i<links.size(); i++)
        links[i].SetAllParents();

    for (unsigned int i=0; i<blocks.size(); i++)
        blocks[i].SetAllParents();

    for (unsigned int i=0; i<sources.size(); i++)
        sources[i].SetAllParents();

    for (map<string,obj_funct_weight>::iterator it=objective_function_set.begin(); it!=objective_function_set.end(); it++)
        objective_function_set[it->first]->obj_funct.SetSystem(this);
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
            cout<<object(obj)->toString()<<endl;
            return true;
        }
        else if (parameter(obj)!=nullptr)
        {
            cout<<parameter(obj)->toString()<<endl;
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
                cout<<object(obj)->Variable(quant)->ToString()<<endl;
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
                cout<<parameter(obj)->Variable(quant)<<endl;
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
