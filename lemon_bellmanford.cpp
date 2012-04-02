#include "mex.h"
#include "matrix.h"
#include <lemon/smart_graph.h>
#include <lemon/bellman_ford.h>

using namespace lemon;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	// Verify input
	char* inputError = "Expected at least 5 parameters\n N - number of nodes\n from - 1xM vector of indices\n to - 1xM vector of indices\n len - 1xM vector of arc lengths\n s - source node in range (1:N)\n Example: [dist, pred] = lemon_bellmanford(n, from, to, len, s);";
	
	if (nrhs < 5 ||  !mxIsNumeric(prhs[0]) || !mxIsNumeric(prhs[1]) || !mxIsNumeric(prhs[2]) || !mxIsNumeric(prhs[3]) || !mxIsNumeric(prhs[4]))
		mexErrMsgTxt(inputError);
	
	mwSize m = mxGetN(prhs[1]);
	double* x = mxGetPr(prhs[1]);
	double* y = mxGetPr(prhs[2]);
	double* l = mxGetPr(prhs[3]);
	mwSize n = (mwSize)mxGetScalar(prhs[0]);
	double s = mxGetScalar(prhs[4]);
	
	if (mxGetM(prhs[1]) != 1 || mxGetM(prhs[2]) != 1 || mxGetM(prhs[3]) != 1 || mxGetN(prhs[2]) != m || mxGetN(prhs[3]) != m || s < 1 || s > n)
		mexErrMsgTxt(inputError);
	
	// Read input
	SmartDigraph g;
	g.reserveNode(n);
	g.reserveArc(m);
	
	typedef SmartDigraph::ArcMap<double> ArcMap;
	typedef SmartDigraph::NodeMap<double> NodeMap;
	ArcMap len(g);
	
	for (mwIndex i = 0; i < n; i++)
		g.addNode();
	
	for (mwIndex i = 0; i < m; i++)
	{
		SmartDigraph::Arc arc = g.addArc(g.nodeFromId(x[i] - 1), g.nodeFromId(y[i] - 1));
		len[arc] = l[i];
	}
	
	// Do stuff
	BellmanFord<SmartDigraph, ArcMap> bellmanford(g, len);
	bellmanford.run(g.nodeFromId(s - 1));
	
	// Create output
	if (nlhs > 0)
	{
		const NodeMap &distMap = bellmanford.distMap();
		plhs[0] = mxCreateDoubleMatrix(1, n, mxREAL);
		double *dist = mxGetPr(plhs[0]);
		for (mwIndex i = 0; i < n; i++)
			dist[i] = distMap[g.nodeFromId(i)];
	}
	if (nlhs > 1)
	{
		const SmartDigraph::NodeMap<SmartDigraph::Arc> &predMap = bellmanford.predMap();
		plhs[1] = mxCreateDoubleMatrix(1, n, mxREAL);
		double *pred = mxGetPr(plhs[1]);
		for (mwIndex i = 0; i < n; i++)
		{
			if (i == s - 1)
				pred[i] = s;
			else
				pred[i] = g.id(g.source(predMap[g.nodeFromId(i)])) + 1;
		}
	}
	
	return;
}
