#include "mex.h"
#include "matrix.h"
#include <lemon/smart_graph.h>
#include <lemon/hao_orlin.h>

namespace lemon
{
	double Tolerance<double>::def_epsilon = 1e-5;
}

using namespace lemon;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	// Verify input
	char* inputError = "Expected at least 4 parameters\n N - number of nodes\n from - 1xM vector of indices\n to - 1xM vector of indices\n cap - 1xM vector of capacities\n s - (optional) source node in range (1:N)\n Example: [value, cut] = lemon_haoorlin(n, from, to, cap);";
	
	if (nrhs < 4 ||  !mxIsNumeric(prhs[0]) || !mxIsNumeric(prhs[1]) || !mxIsNumeric(prhs[2]) || !mxIsNumeric(prhs[3]))
		mexErrMsgTxt(inputError);
	
	mwSize m = mxGetN(prhs[1]);
	double* x = mxGetPr(prhs[1]);
	double* y = mxGetPr(prhs[2]);
	double* c = mxGetPr(prhs[3]);
	mwSize n = (mwSize)mxGetScalar(prhs[0]);
	double s = 0;
	
	if (mxGetM(prhs[1]) != 1 || mxGetM(prhs[2]) != 1 || mxGetM(prhs[3]) != 1 || mxGetN(prhs[2]) != m || mxGetN(prhs[3]) != m)
		mexErrMsgTxt(inputError);
	
	if (nrhs > 4)
	{
		if (!mxIsNumeric(prhs[4]))
			mexErrMsgTxt(inputError);
		
		s = mxGetScalar(prhs[4]);
		if (s < 1 || s > n)
			mexErrMsgTxt(inputError);
	}
	
	// Read input
	SmartDigraph g;
	g.reserveNode(n);
	g.reserveArc(m);
	
	typedef SmartDigraph::ArcMap<double> ArcMap;
	typedef SmartDigraph::NodeMap<double> NodeMap;
	ArcMap cap(g);
	
	for (mwIndex i = 0; i < n; i++)
		g.addNode();
	
	for (mwIndex i = 0; i < m; i++)
	{
		SmartDigraph::Arc arc = g.addArc(g.nodeFromId(x[i] - 1), g.nodeFromId(y[i] - 1));
		cap[arc] = c[i];
	}
	
	// Do stuff
	HaoOrlin<SmartDigraph, ArcMap> haoorlin(g, cap);
	if (s != 0)
		haoorlin.run(g.nodeFromId(s - 1));
	else
		haoorlin.run();
	
	// Create output
	if (nlhs > 0)
	{
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		double *value = mxGetPr(plhs[0]);
		value[0] = haoorlin.minCutValue();
	}
	if (nlhs > 1)
	{
		NodeMap cutMap(g);
		haoorlin.minCutMap(cutMap);
		plhs[1] = mxCreateLogicalMatrix(1, n);
		bool *cut = mxGetLogicals(plhs[1]);
		for (mwIndex i = 0; i < n; i++)
			cut[i] = cutMap[g.nodeFromId(i)];
	}
	return;
}
