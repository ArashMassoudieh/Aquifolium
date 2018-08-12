#ifndef SYSTEM_H
#define SYSTEM_H

#include "Block.h"
#include "Link.h"
#include "Object.h"
#include "Vector_arma.h"
#include "Matrix_arma.h"
#include "MetaModel.h"

struct solversettings
{
    double t;
    double dt;
    double C_N_weight; //Crank-Nicholson Weight
    double NRtolerance; //Newton Raphson Tolerance
    double NR_coeff_reduction_factor; //The coefficient to reduce the Newton-Raphson coefficient
};

struct solvertemporaryvars
{
    CMatrix_arma Inverse_Jacobian;
    double NR_coefficient = 1;
    bool updatejacobian;
};

class System: public Object
{
    public:
        System();
        virtual ~System();
        System(const System& other);
        System& operator=(const System& other);
        double &GetTime() {return SolverSettings.t;};
        bool AddBlock(Block &blk);
        bool AddLink(Link &lnk, const string &source, const string &destination);
        Block *block(const string &s);
        Link *link(const string &s);
        int blockid(const string &s);
        int linkid(const string &s);
        bool GetQuanTemplate(string filename);
        void CopyQuansToMembers();
        double &dt() {return SolverSettings.dt;};
        bool OneStepSolve(const string &s);
		bool renew(const string &variable);
    protected:

    private:
        vector<Block> blocks;
        vector<Link> links;
        string last_error;
        MetaModel metamodel;
        CVector_arma GetResiduals(const string &variable, CVector_arma &X);
        CVector_arma GetStateVariables(const string &variable, const Expression::timing &tmg = Expression::timing::past);
        solversettings SolverSettings;
        bool OneStepSolve();
        CMatrix_arma Jacobian(const string &variale, CVector_arma &X);
        CVector_arma Jacobian(const string &variable, CVector_arma &V, CVector_arma &F0, int i);  //Works also w/o reference (&)
        bool CalculateFlows(const string &var, const Expression::timing &tmg = Expression::timing::present);
        void SetStateVariables(const string &variable, CVector_arma &X, const Expression::timing &tmg = Expression::timing::present);
        solvertemporaryvars SolverTempVars;
};

#endif // SYSTEM_H
