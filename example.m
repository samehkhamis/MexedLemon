% Maximum flow example
from = [1, 1, 3, 4, 3, 4];
to = [3, 4, 2, 2, 4, 3];
cap = [1, 2, 5, 6, 3, 4];
[value, cut, flow] = lemon_preflow(4, from, to, cap, 1, 2)

% Maximum weighted matching example
from = [1, 2, 2];
to = [3, 3, 4];
weight = [1, 0, 1];
[value, matches] = lemon_maxweightedmatching(4, from, to, weight)

% Dijkstra example
from = [1, 1, 2, 3];
to = [2, 3, 4, 4];
len = [1, 2, 5, 1];
[dist, pred] = lemon_dijkstra(4, from, to, len, 1, 4)

% Minimum cost flow example #1 MF
from = [1, 1, 3, 4, 3, 4, 2];
to = [3, 4, 2, 2, 4, 3, 1];
upper = [1, 2, 5, 6, 3, 4, 1e3];
cost = [0, 0, 0, 0, 0, 0, -1];
[value, flow, pot] = lemon_costscaling(4, from, to, 'upper', upper, 'cost', cost)

% Minimum cost flow example #2 MWM
from = [1, 2, 2];
to = [3, 3, 4];
upper = [1, 1, 1];
cost = [1, 0, 1];
supply = [1, 1, -1, -1];
[value, flow, pot] = lemon_costscaling(4, from, to, 'upper', upper, 'cost', cost, 'supply', supply)

% Minimum cost flow example #3 SP
from = [1, 1, 2, 3, 4];
to = [2, 3, 4, 4, 1];
upper = [1, 1, 1, 1, 1];
cost = [1, 2, 5, 1, -1e3];
[value, flow, pot] = lemon_costscaling(4, from, to, 'upper', upper, 'cost', cost)

% Kruskal example
from = [1, 2, 3, 4, 1];
to = [2, 3, 4, 1, 3];
cost = [1, 1, 1, 1, 0];
[mst] = lemon_kruskal(4, from, to, cost)

% Hao-Orlin example
from = [1, 1, 3, 4, 3, 4];
to = [3, 4, 2, 2, 4, 3];
cap = [1, 2, 5, 6, 3, 4];
[value, cut] = lemon_haoorlin(4, from, to, cap)
