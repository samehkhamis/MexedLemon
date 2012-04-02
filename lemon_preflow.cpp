#include "mex.h"
#include "matrix.h"
#include <lemon/smart_graph.h>
#include <lemon/preflow.h>

namespace lemon
{
	double Tolerance<double>::def_epsilon = 1e-5;
}

using namespace lemon;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	// Verify input
	char* inputError = "Expected 6 parameters\n N - number of nodes\n from - 1xM vector of indices\n to - 1xM vector of indices\n cap - 1xM vector of capacities\n s - source node in range (1:N)\n t - target node in range (1:N)\n Example: [value, cut, flow] = lemon_preflow(n, from, to, cap, s, t);";
	
	if (nrhs != 6 ||  !mxIsNumeric(prhs[0]) || !mxIsNumeric(prhs[1]) || !mxIsNumeric(prhs[2]) || !mxIsNumeric(prhs[3]) || !mxIsNumeric(prhs[4]) || !mxIsNumeric(prhs[5]))
		mexErrMsgTxt(inputError);
	
	mwSize m = mxGetN(prhs[1]);
	double* x = mxGetPr(prhs[1]);
	double* y = mxGetPr(prhs[2]);
	double* c = mxGetPr(prhs[3]);
	mwSize n = (mwSize)mxGetScalar(prhs[0]);
	double s = mxGetScalar(prhs[4]);
	double t = mxGetScalar(prhs[5]);
	
	if (mxGetM(prhs[1]) != 1 || mxGetM(prhs[2]) != 1 || mxGetM(prhs[3]) != 1 || mxGetN(prhs[2]) != m || mxGetN(prhs[3]) != m || s == t || s < 1 || t < 1 || s > n || t > n)
		mexErrMsgTxt(inputError);
	
	// Read input
	SmartDigraph g;
	g.reserveNode(n);
	g.reserveArc(m);
	
	typedef SmartDigraph::ArcMap<double> ArcMap;
	ArcMap cap(g);
	
	for (mwIndex i = 0; i < n; i++)
		g.addNode();
	
	for (mwIndex i = 0; i < m; i++)
	{
		SmartDigraph::Arc arc = g.addArc(g.nodeFromId(x[i] - 1), g.nodeFromId(y[i] - 1));
		cap[arc] = c[i];
	}
	
	// Do stuff
	Preflow<SmartDigraph, ArcMap> preflow(g, cap, g.nodeFromId(s - 1), g.nodeFromId(t - 1));
	preflow.runMinCut();
	
	// Create output
	if (nlhs > 0)
	{
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		double *value = mxGetPr(plhs[0]);
		value[0] = preflow.flowValue();
	}
	if (nlhs > 1)
	{
		plhs[1] = mxCreateLogicalMatrix(1, n);
		bool *cut = mxGetLogicals(plhs[1]);
		for (mwIndex i = 0; i < n; i++)
			cut[i] = !preflow.minCut(g.nodeFromId(i));
	}
	if (nlhs > 2)
	{
		const ArcMap &flowMap = preflow.flowMap();
		plhs[2] = mxCreateDoubleMatrix(1, m, mxREAL);
		double *flow = mxGetPr(plhs[2]);
		for (mwIndex i = 0; i < m; i++)
			flow[i] = flowMap[g.arcFromId(i)];
	}
	
	return;
}
