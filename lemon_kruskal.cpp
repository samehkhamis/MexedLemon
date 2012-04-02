#include "mex.h"
#include "matrix.h"
#include <lemon/smart_graph.h>
#include <lemon/kruskal.h>

using namespace lemon;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	// Verify input
	char* inputError = "Expected 4 parameters\n N - number of nodes\n from - 1xM vector of indices\n to - 1xM vector of indices\n cost - 1xM vector of edge costs\n Example: [mst] = lemon_kruskal(n, from, to, cost);";
	
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
	EdgeMap cost(g);
	
	for (mwIndex i = 0; i < n; i++)
		g.addNode();
	
	for (mwIndex i = 0; i < m; i++)
	{
		SmartGraph::Edge edge = g.addEdge(g.nodeFromId(x[i] - 1), g.nodeFromId(y[i] - 1));
		cost[edge] = c[i];
	}
	
	// Do stuff
	EdgeMap tree(g);
	kruskal(g, cost, tree);
	
	// Create output
	if (nlhs > 0)
	{
		plhs[0] = mxCreateLogicalMatrix(1, m);
		bool *mst = mxGetLogicals(plhs[0]);
		for (mwIndex i = 0; i < m; i++)
			mst[i] =  tree[g.edgeFromId(i)];
	}
	
	return;
}
