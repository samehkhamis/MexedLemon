#include "mex.h"
#include "matrix.h"
#include <lemon/smart_graph.h>
#include <lemon/dfs.h>
#include <vector>

using namespace lemon;
using namespace std;

template <typename GR>
struct Visitor : public DfsVisitor<GR>
{
	vector<Node> list;
	void start(const Node& node) { list.clear(); }
	void reach(const Node& node) { list.push_back(node); }
};

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	// Verify input
	char* inputError = "Expected at least 3 parameters\n N - number of nodes\n from - 1xM vector of indices\n to - 1xM vector of indices\n s - (optional) source node in range (1:N)\n Example: [visit] = lemon_dfs(n, from, to, s);";
	
	if (nrhs < 3 ||  !mxIsNumeric(prhs[0]) || !mxIsNumeric(prhs[1]) || !mxIsNumeric(prhs[2]))
		mexErrMsgTxt(inputError);
	
	mwSize m = mxGetN(prhs[1]);
	double* x = mxGetPr(prhs[1]);
	double* y = mxGetPr(prhs[2]);
	mwSize n = (mwSize)mxGetScalar(prhs[0]);
	double s = 0;
	
	if (mxGetM(prhs[1]) != 1 || mxGetM(prhs[2]) != 1 || mxGetN(prhs[2]) != m)
		mexErrMsgTxt(inputError);
	
	if (nrhs > 3)
	{
		if (!mxIsNumeric(prhs[3]))
			mexErrMsgTxt(inputError);
		
		s = mxGetScalar(prhs[3]);
		if (s < 1 || s > n)
			mexErrMsgTxt(inputError);
		
	}
	
	// Read input
	SmartDigraph g;
	g.reserveNode(n);
	g.reserveArc(m);
	
	for (mwIndex i = 0; i < n; i++)
		g.addNode();
	
	for (mwIndex i = 0; i < m; i++)
		g.addArc(g.nodeFromId(x[i] - 1), g.nodeFromId(y[i] - 1));
	
	// Do stuff
	Visitor<SmartDigraph> v;
	DfsVisit<SmartDigraph, Visitor<SmartDigraph>> dfs(g, v);
	if (s != 0)
		dfs.run(g.nodeFromId(s - 1));
	else
		dfs.run();
	
	// Create output
	if (nlhs > 0)
	{
		plhs[0] = mxCreateDoubleMatrix(1, v.list.size(), mxREAL);
		double *visit = mxGetPr(plhs[0]);
		for (mwIndex i = 0; i < v.list.size(); i++)
			visit[i] = g.id(v.list[i]) + 1;
	}
	
	return;
}
