#include "mex.h"
#include "matrix.h"
#include <lemon/smart_graph.h>
#include <lemon/cycle_canceling.h>

namespace lemon
{
	double Tolerance<double>::def_epsilon = 1e-5;
}

using namespace lemon;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	// Verify input
	char* inputError = "Expected at least 3 parameter\n N - number of nodes\n from - 1xN vector of indices\n to - 1xM vector of indices\n cost - (optional) 1xM vector of edge costs (default: 1)\n lower - (optional) 1xM vector of flow lower bound (default: 0)\n upper - (optional) 1xM vector of flow upper bound (default: INF)\n supply - (optional) 1xN vector of node supply (default: 0)\n Example: [value, flow, pot] = lemon_cyclecanceling(n, from, to, 'cost', cost, 'lower', lower, 'supply', supply);";
	
	if (nrhs < 3 || nrhs % 2 == 0 || !mxIsNumeric(prhs[0]) || !mxIsNumeric(prhs[1]) || !mxIsNumeric(prhs[2]))
		mexErrMsgTxt(inputError);
	
	mwSize m = mxGetN(prhs[1]);
	double* x = mxGetPr(prhs[1]);
	double* y = mxGetPr(prhs[2]);
	mwSize n = (mwSize)mxGetScalar(prhs[0]);
	
	if (mxGetM(prhs[1]) != 1 || mxGetM(prhs[2]) != 1 || mxGetN(prhs[2]) != m)
		mexErrMsgTxt(inputError);
	
	double *c = NULL, *l = NULL, *u = NULL, *s = NULL;
	
	for (int i = 3; i < nrhs; i += 2)
	{
		mwSize len = mxGetN(prhs[i]) + 1;
		char* str = (char*) mxCalloc(len, sizeof(char));
		mxGetString(prhs[i], str, len);
		
		if (strcmp(str, "cost") == 0)
		{
			if (!mxIsNumeric(prhs[i + 1]) || mxGetM(prhs[i + 1]) != 1 || mxGetN(prhs[i + 1]) != m)
				mexErrMsgTxt(inputError);
			c = mxGetPr(prhs[i + 1]);
		}
		else if (strcmp(str, "lower") == 0)
		{
			if (!mxIsNumeric(prhs[i + 1]) || mxGetM(prhs[i + 1]) != 1 || mxGetN(prhs[i + 1]) != m)
				mexErrMsgTxt(inputError);
			l = mxGetPr(prhs[i + 1]);
		}
		else if (strcmp(str, "upper") == 0)
		{
			if (!mxIsNumeric(prhs[i + 1]) || mxGetM(prhs[i + 1]) != 1 || mxGetN(prhs[i + 1]) != m)
				mexErrMsgTxt(inputError);
			u = mxGetPr(prhs[i + 1]);
		}
		else if (strcmp(str, "supply") == 0)
		{
			if (!mxIsNumeric(prhs[i + 1]) || mxGetM(prhs[i + 1]) != 1 || mxGetN(prhs[i + 1]) != n)
				mexErrMsgTxt(inputError);
			s = mxGetPr(prhs[i + 1]);
		}
		else
			mexErrMsgTxt(inputError);
	}
	
	// Read input
	SmartDigraph g;
	g.reserveNode(n);
	g.reserveArc(m);
	
	typedef SmartDigraph::ArcMap<double> ArcMap;
	typedef SmartDigraph::NodeMap<double> NodeMap;
	ArcMap lower(g);
	ArcMap upper(g);
	ArcMap cost(g);
	NodeMap supply(g);
	
	for (mwIndex i = 0; i < n; i++)
	{
		SmartDigraph::Node node = g.addNode();
		if (s) supply[node] = s[i];
	}
	
	for (mwIndex i = 0; i < m; i++)
	{
		SmartDigraph::Arc arc = g.addArc(g.nodeFromId(x[i] - 1), g.nodeFromId(y[i] - 1));
		if (c) cost[arc] = c[i];
		if (l) lower[arc] = l[i];
		if (u) upper[arc] = u[i];
	}
	
	// Do stuff
	CycleCanceling<SmartDigraph, double> cyclecanceling(g);
	if (c) cyclecanceling.costMap<ArcMap>(cost);
	if (l) cyclecanceling.lowerMap<ArcMap>(lower);
	if (u) cyclecanceling.upperMap<ArcMap>(upper);
	if (s) cyclecanceling.supplyMap<NodeMap>(supply);
	cyclecanceling.run();
	
	// Create output
	if (nlhs > 0)
	{
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		double *total = mxGetPr(plhs[0]);
		total[0] = cyclecanceling.totalCost();
	}
	if (nlhs > 1)
	{
		ArcMap flowMap(g);
		cyclecanceling.flowMap(flowMap);
		plhs[1] = mxCreateDoubleMatrix(1, m, mxREAL);
		double *flow = mxGetPr(plhs[1]);
		for (mwIndex i = 0; i < m; i++)
			flow[i] = flowMap[g.arcFromId(i)];
	}
	if (nlhs > 2)
	{
		NodeMap potMap(g);
		cyclecanceling.potentialMap(potMap);
		plhs[2] = mxCreateDoubleMatrix(1, n, mxREAL);
		double *pot = mxGetPr(plhs[2]);
		for (mwIndex i = 0; i < n; i++)
			pot[i] = potMap[g.nodeFromId(i)];
	}
	
	return;
}
