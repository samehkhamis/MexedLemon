#include "mex.h"
#include "matrix.h"
#include <lemon/smart_graph.h>
#include <lemon/gomory_hu.h>

namespace lemon
{
	double Tolerance<double>::def_epsilon = 1e-5;
}

using namespace lemon;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	// Verify input
	char* inputError = "Expected 4 parameters\n N - number of nodes\n from - 1xM vector of indices\n to - 1xM vector of indices\n cap - 1xM vector of capacities\n Example: [values, cuts] = lemon_gomoryhu(n, from, to, cap);";
	
	if (nrhs != 4 ||  !mxIsNumeric(prhs[0]) || !mxIsNumeric(prhs[1]) || !mxIsNumeric(prhs[2]) || !mxIsNumeric(prhs[3]))
		mexErrMsgTxt(inputError);
	
	mwSize m = mxGetN(prhs[1]);
	double* x = mxGetPr(prhs[1]);
	double* y = mxGetPr(prhs[2]);
	double* c = mxGetPr(prhs[3]);
	mwSize n = (mwSize)mxGetScalar(prhs[0]);
	
	if (mxGetM(prhs[1]) != 1 || mxGetM(prhs[2]) != 1 || mxGetM(prhs[3]) != 1 || mxGetN(prhs[2]) != m || mxGetN(prhs[3]) != m)
		mexErrMsgTxt(inputError);
	
	// Read input
	SmartGraph g;
	g.reserveNode(n);
	g.reserveEdge(m);
	
	typedef SmartGraph::EdgeMap<double> EdgeMap;
	typedef SmartGraph::NodeMap<double> NodeMap;
	EdgeMap cap(g);
	
	for (mwIndex i = 0; i < n; i++)
		g.addNode();
	
	for (mwIndex i = 0; i < m; i++)
	{
		SmartGraph::Edge edge = g.addEdge(g.nodeFromId(x[i] - 1), g.nodeFromId(y[i] - 1));
		cap[edge] = c[i];
	}
	
	// Do stuff
	GomoryHu<SmartGraph, EdgeMap> gomoryhu(g, cap);
	gomoryhu.run();
	
	// Create output
	mwSize l = (n * n - n) / 2, k = 0, nextk = 0;
	
	if (nlhs > 0)
	{
		plhs[0] = mxCreateDoubleMatrix(l, 1, mxREAL);
		double *values = mxGetPr(plhs[0]);
		k = 0;
		for (mwIndex i = 0; i < n; i++)
			for (mwIndex j = i + 1; j < n; j++)
				values[k++] = gomoryhu.minCutValue(g.nodeFromId(i), g.nodeFromId(j));
	}
	if (nlhs > 1)
	{
		NodeMap cutMap(g);
		plhs[1] = mxCreateLogicalMatrix(l, n);
		bool *cuts = mxGetLogicals(plhs[1]);
		for (mwIndex i = 0; i < n; i++)
		{
			for (mwIndex j = i + 1; j < n; j++)
			{
				k = nextk;
				gomoryhu.minCutMap(g.nodeFromId(i), g.nodeFromId(j), cutMap);
				for (mwIndex x = 0; x < n; x++)
				{
					cuts[k] = cutMap[g.nodeFromId(x)];
					k += l;
				}
				nextk++;
			}
		}
	}
	return;
}
