#include <cmath>
#include <string>
#include <cassert>
#include <iostream>
#include <TF1.h>
#include <TGraph.h>
#include "LinearFitter.hxx"

namespace
{

TGraph* gGraph = 0;

void fcn(Int_t& npar, Double_t* grad, Double_t& fval, Double_t* par, Int_t flag)
{
	fval = 0;
	if(grad && flag)
		;
	if(!gGraph)   {
		std::cerr << "gGraph == 0!\n";
		return;
	}
	assert(npar <= 2);

	int n = gGraph->GetN();
	double *x = gGraph->GetX();
	double *y = gGraph->GetY();
	double *exl = gGraph->GetEXlow();
	double *exh = gGraph->GetEXhigh();
	double *eyl = gGraph->GetEYlow();
	double *eyh = gGraph->GetEYhigh();

	fval = 0.;
	for(int i=0; i< n; ++i) {
		fval += dragon::LinearFitter::Chi2err(par[1], par[0], x[i], y[i], exl[i], exh[i], eyl[i], eyh[i]);
	}
}

struct gGraphSetter {
	gGraphSetter(TGraph* g) { gGraph = g; }
	~gGraphSetter() { gGraph = 0; }
};

}


dragon::LinearFitter::LinearFitter(int printLevel):
//	fMinuit(new TMinuit(2)),
	fSlope(0,0),
	fOffset(0,0),
	fFunction(0)
{
	/// Initialize internal TMinuit object
	// fMinuit->SetFCN(fcn);
	// fMinuit->SetPrintLevel(printLevel);

	Double_t arglist[4];
	Int_t ierflg = 0;

	arglist[0] = 1;
	//fMinuit->mnexcm("SET ERR", arglist ,1,ierflg);
}

double dragon::LinearFitter::Chi2err(double M, double B, double x, double y, double exl, double exh, double eyl, double eyh)
{
	double fx = M*x + B;
	double ey = fx < y ? eyl : eyh;

	double num = (y-fx)*(y-fx);
	double den = ey*ey + pow(0.5*M*(exl+exh), 2);
	if(den == 0) // no errors
		return num;
	return num / den;
}

void dragon::LinearFitter::Fit(TGraph* graph)
{
	std::cout <<"LinearFitter disabled due to lack of ability to link to TMinuit"<<std::endl;
#if 0
	gGraphSetter ggset(graph);

// Set starting values and step sizes for parameters
	if(gGraph->GetN() < 2) {
		std::cerr << "ERROR: Invalid number of points: " << gGraph->GetN() << "\n";
		return;
	}

// Initialize parameters
	double slp = (gGraph->GetY()[1] - gGraph->GetY()[0]) / (gGraph->GetX()[1] - gGraph->GetX()[0]);
	double off = gGraph->GetY()[0] - slp*gGraph->GetX()[0];

	int ierflg;
	double arglist[10];
	fMinuit->mnparm(0, "offset", off, 1e-9, 0,0,ierflg);
	fMinuit->mnparm(1, "slope",  slp, 1e-9, 0,0,ierflg);

// Now ready for minimization step
	arglist[0] = 500;
	arglist[1] = 1;
	int tries = 0;

	while(tries < 5) {
		fMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);
		tries++;
		if(!ierflg) // success!
			break;
		else if (ierflg == 4) // try again
			continue;
		else {      // bail out
			std::string err;
			switch(ierflg) {
			case 1: err = "command is blank, ignored";
				break;
			case 2: err = "command line unreadable, ignored";
				break;
			case 3: err = "unknown command, ignored";
				break;
			case 4: err = "abnormal termination (e.g., MIGRAD not converged)";
				break;
			case 9: err = "reserved";
				break;
			case 10: err = "END command";
				break;
			case 11: err = "EXIT or STOP command";
				break;
			case 12: err = "RETURN command";
				break;
			default: err = "UNKNOWN";
				break;
			}
			std::cout << "Error in MIGRAD (ierflg = " << ierflg << "): " << err << "\n";
		}
	}
//	std::cout << "MIGRAD tries: " << tries << "\n";

// And MINOS error finding
	fMinuit->mnexcm("MINOs", arglist, 0, ierflg);

// Set results
	double par, errl, errh, errs, gcc;

	fMinuit->GetParameter(0, par, errs);
	fOffset.SetNominal(par);

	fMinuit->GetParameter(1, par, errs);
	fSlope.SetNominal(par);

	fMinuit->mnerrs(0, errh, errl, errs, gcc);
	fOffset.SetErrLow (fabs(errl));
	fOffset.SetErrHigh(fabs(errh));

	fMinuit->mnerrs(1, errh, errl, errs, gcc);
	fSlope.SetErrLow (fabs(errl));
	fSlope.SetErrHigh(fabs(errh));

	fFunction.reset(new TF1("pol1", "[0] + [1]*x", -1e20, 1e20));
	fFunction->SetParameter(0, fOffset.GetNominal());
	fFunction->SetParameter(1, fSlope.GetNominal());
#endif
}

void dragon::LinearFitter::Fit(const std::vector<double>& x, const std::vector<double>& y,
															 const std::vector<double>& exl, const std::vector<double>& exh,
															 const std::vector<double>& eyl, const std::vector<double>& eyh)
{
	TGraphAsymmErrors graph (x.size(), &x[0], &y[0], &exl[0], &exh[0], &eyl[0], &eyh[0]);
	Fit(&graph);
}
