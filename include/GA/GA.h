#pragma once


#include "Individual.h"
#include "Distribution.h"
#include <stdio.h>
#ifdef GIFMOD
#include "Medium.h"
#include "MediumSet.h"
#endif
#ifdef GWA
#include "gwa.h"
#endif

#include "BTC.h"
#include "DistributionNUnif.h"
#include "math.h"
#include <iostream>
#include "Matrix.h"
#include "omp.h"
#include <vector>

//GUI
class runtimeWindow;

struct GA_Twiking_parameters
{
    int totnumparams;
	int maxpop;
	int nParam;
	int numenhancements, num_enh;
	int nGen;
	//int writeinterval; //MCMC
	// int nchains, burnout; //MCMC
	// int nMCMCsamples; //MCMC
	// double acceptance_rate; //MCMC
	// int nrealizations; // MCMC
	// int continue_mcmc; // MCMC
	int cross_over_type;
	int no_bins;

	// bool sensbasedpurt; //MCMC
	// bool justreadmcmc; //MCMC
	// bool mcmc_realization; //MCMC
	// bool calculate_correlation; // MCMC
	bool sens_out;
	// bool global_sensitivity; //MCMC
	// bool calculate_percentile; // MCMC
	// bool noinipurt; //MCMC
	bool RCGA;
	bool logtrans, fixedstd, mixederror;
	bool readfromgafile;
	// bool calc_distributions; //MCMC
	// bool noise_realization_writeout; // MCMC
	// bool obs_realization_writeout; //MCMC
	// bool const_realization_writeout; //MCMC
	// bool influent_realization; // MCMC

	// double dp_sens; // MCMC
	// double purtscale; //MCMC
	// double purt_fac; //MCMC

	double N;
	double pcross;
	double pmute;

	double exponentcoeff;
	double biasfact;
	double shakescale, shakescalered;
	// double pertscale, nonpurt; //MCMC
	// bool mcmc_run; // MCMC
    char fitnesstype;
};

struct _filenames
{
    string initialpopfilemame;
	string pathname;
	string getfromfilename;
	string outputfilename;
	string mcmcoutputfile;
};

using namespace std;

template<class T>
class CGA
{
public:

    double sumfitness;
	double MaxFitness;
    GA_Twiking_parameters GA_params;
    _filenames filenames;
	vector<double> calc_output_percentiles;
	vector<vector<double>> initial_pop;
	vector<double> final_params;
	vector<int> params;
	vector<int> loged;
	vector<int> to_ts;
	vector<double> fixedinputvale;
	vector<double> minval, maxval;
	vector<bool> apply_to_all;
	vector<vector<int>> outcompare;
	vector<CIndividual> Ind;
	vector<CIndividual> Ind_old;
	vector<string> paramname;
    vector<T> Models;
	T Model_out;
	T Model;

	CGA();
	CGA(int n);
	virtual ~CGA();
	CGA(int n, int nParams);
	CGA(const CGA &C);
    CGA operator=(CGA &C);
    CGA(string filename, const T&);
    void initialize();
    double getfromoutput(string filename);
    void getinifromoutput(string filename);
    void getinitialpop(string filename);
private:
    void Setminmax(int a, double minrange, double maxrange, int prec);
    void fitnessdistini();
    void crossover();
    double avgfitness();
    void mutate(double mu);
    void assignfitnesses();
    int maxfitness();
    double variancefitness();
    double stdfitness();
    double avg_actual_fitness();
	void write_to_detailed_GA(string s);
    int optimize();
    void setnumpop(int n);
    double avg_inv_actual_fitness();
    int optimize(int nGens, char DefOutPutFileName[]);
    void setnparams(int n);
    void assignfixedvalues();
    void assignrank();
    void assignfitness_rank(double N);
    void shake();
    void crossoverRC();
    void getfromfile(char filename[]);
    void fillfitdist();
    double assignfitnesses(vector<double> inp);
    int getparamno(int i, int ts);
    int get_act_paramno(int i);
    int get_time_series(int i);
    double evaluateforward();
    double evaluateforward_mixed(vector<double> v);
    CDistribution fitdist;

	// GUI
	int numberOfThreads;
	#ifdef QT_version
	int optimize(runtimeWindow* rtw);
	void assignfitnesses(runtimeWindow* rtw);
	void updateProgress(runtimeWindow* rtw, QMap<QString, QString> vars, bool finished = false) const;
	void updateProgress(runtimeWindow* rtw, bool resetGeneration = false) const;
	#endif // QT_version


};

#include "../src/GA/GA.cpp"



