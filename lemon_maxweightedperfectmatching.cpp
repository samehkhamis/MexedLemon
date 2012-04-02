#include "mex.h"
#include "matrix.h"
#include <lemon/smart_graph.h>
#include <lemon/matching.h>

using namespace lemon;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	// Verify input
	char* inputError = "Expected 4 parameters\n N - number of nodes\n from - 1xM vector of indices\n to - 1xM vector of indices\n weight - 1xM vector of edge weights\n Example: [value, matches] = lemon_maxweightedperfectmatching(n, from, to, weight);";
	
	if (nrhs != 4 ||  !mxIsNumeric(prhs[0]) || !mxIsNumeric(prhs[1]) || !mxIsNumeric(prhs[2]) || !mxIsNumeric(prhs[3]))
		mexErrMsgTxt(inputError);
	
	mwSize m = mxGetN(prhs[1]);
	double* x = mxGetPr(prhs[1]);
	double* y = mxGetPr(prhs[2]);
	double* w = mxGetPr(prhs[3]);
	mwSize n = (mwSize)mxGetScalar(prhs[0]);
	
	if (mxGetM(prhs[1]) != 1 || mxGetM(prhs[2]) != 1 || mxGetM(prhs[3]) != 1 || mxGetN(prhs[2]) != m || mxGetN(prhs[3]) != m)
		mexErrMsgTxt(inputError);
	
	// Read input
	SmartGraph g;
	g.reserveNode(n);
	g.reserveEdge(m);
	
	typedef SmartGraph::EdgeMap<double> EdgeMap;
	EdgeMap weight(g);
	
	for (mwIndex i = 0; i < n; i++)
		g.addNode();
	
	for (mwIndex i = 0; i < m; i++)
	{
		SmartGraph::Edge edge = g.addEdge(g.nodeFromId(x[i] - 1), g.nodeFromId(y[i] - 1));
		weight[edge] = w[i];
	}
	
	// Do stuff
	MaxWeightedPerfectMatching<SmartGraph, EdgeMap> mwpm(g, weight);
	mwpm.run();
	
	// Create output
	if (nlhs > 0)
	{
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		double *value = mxGetPr(plhs[0]);
		value[0] = mwpm.matchingWeight();
	}
	if (nlhs > 1)
	{
		const SmartGraph::NodeMap<SmartGraph::Arc> &matchingMap = mwpm.matchingMap();
		plhs[1] = mxCreateDoubleMatrix(1, n, mxREAL);
		double *matches = mxGetPr(plhs[1]);
		for (mwIndex i = 0; i < n; i++)
		{
			SmartGraph::Arc arc = matchingMap[g.nodeFromId(i)];
			matches[i] = g.id(g.target(arc)) + 1;
		}
	}
	
	return;
}
