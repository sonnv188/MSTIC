#include <bits/stdc++.h>

using namespace std;

#ifdef LOCAL
#include "C:\debug.h"
#else
#define debug(...) 42
#endif

const int INF = 1e9;

int n, m;
vector<int> pr, deg, weight, chosen;
vector<bool> bridge;
vector<vector<int>> mst;
vector<array<int, 3>> edges;
int cost, best_solution_so_far = INF;

bool use_lower_bound, use_upper_bound, use_ub2;
const int threshold = 60*60;
int start_clock;


long long try_count, lb_count, ub1_count, ub2_count;
int lb_cost = -1, ub1_cost = -1, ub2_cost = -1;
double running_time, running_time_end;
vector<int> is_leaf, dist, start, best_choice, trace_cost, trace_ending_cost, deg_use;
vector<pair<int, int>> adj;
vector<vector<array<int, 4>>> nxt;
vector<array<int, 3>> nearest, nearest_inner;

int find(int u) {
  return pr[u] < 0 ? u : pr[u] = find(pr[u]);
}

bool connected(int u, int v) {
  return find(u) == find(v);
}

void add_edge(int u, int v) {
  if (++deg[u] == 2) {
    cost += weight[u];
  }
  if (++deg[v] == 2) {
    cost += weight[v];
  }
} 

bool merge(int w, int u, int v) {
  if (connected(u, v)) {
    return 0;
  }
  add_edge(u, v);
  u = find(u), v = find(v);
  cost += w;
  if (pr[u] > pr[v]) {
    swap(u, v);
  }
  pr[u] += pr[v];
  pr[v] = u;
  return 1;
}


int size(int u) {
  return -pr[find(u)];
}

void clear() {
  for (int i = 0; i < n; ++i) {
    pr[i] = -1;
    deg[i] = 0;
  }
  cost = 0;
}

void backtrack(int index) {
  if (((double) (clock()) - start_clock)/CLOCKS_PER_SEC > threshold) {
    return;
  }
  ++try_count;
  if (index == m) {
    return;
  }
  auto [w, u, v] = edges[index];
  clear();
  
  for (int it : chosen) {
    auto [a, b, c] = edges[it];
    merge(a, b, c);
  }
  
  if (!connected(u, v)) {
    merge(w, u, v);
    chosen.push_back(index);
    int lower_bound = cost;
    for (int it : chosen) {
      auto [a, b, c] = edges[it];
      if (size(b) == 2) {
        lower_bound += min(weight[b], weight[c]);
      }
    }
    for (int it : mst[index]) {
      auto [a, b, c] = edges[it];
      if (merge(a, b, c)) {
        lower_bound += a;
      }
    }
	if (lower_bound >= best_solution_so_far){
		++lb_count;
	}
	if(use_lower_bound == 0){
		if(size(0) == n ){
			if(cost < best_solution_so_far){
				  best_solution_so_far = cost;
				  lb_cost = lower_bound;
				  ub1_cost = cost;
				  ++ub1_count;
				  running_time = ((double) (clock()) - start_clock)/CLOCKS_PER_SEC;
				  best_choice = chosen;
			}
			backtrack(index + 1);
		}
	}
	else{
		if (size(0) == n && lower_bound < best_solution_so_far) {
				if(cost < best_solution_so_far){
				  best_solution_so_far = cost;
				  lb_cost = lower_bound;
				  ub1_cost = cost;
				  ++ub1_count;
				  running_time = ((double) (clock()) - start_clock)/CLOCKS_PER_SEC;
				  best_choice = chosen;
				}
			  backtrack(index + 1);
			}
	}
	chosen.pop_back();
  }
  if (!bridge[index]) {
    backtrack(index + 1);
  }
}

int main() {
  ios::sync_with_stdio(false); 
  cin.tie(nullptr);
  scanf("%d%d", &n, &m);
  edges.reserve(m);
  chosen.reserve(m);
  weight.reserve(n);
  for (int i = 0; i < m; ++i) {
    int w, u, v;
    scanf("%d;%d;%d", &u, &v, &w);
    --u, --v;
    edges.push_back({w, u, v});
  }
  for (int i = 0; i < n; ++i) {
    int tmp, x;
    scanf("%d%d", &tmp, &x);
    weight.push_back(x);
  }
  mst.resize(m);
  pr = vector<int>(n, -1);
  deg = vector<int>(n, 0);
  bridge = vector<bool>(m, 0);
  start_clock = clock();
  for (int i = 0; i < m; ++i) {
    clear();
    vector<int> ord(m - i - 1);
    iota(ord.begin(), ord.end(), i + 1);
    sort(ord.begin(), ord.end(), [&](int u, int v) {
      return edges[u] < edges[v];
    });
    for (int j : ord) {
      auto [a, b, c] = edges[j];
      if (merge(a, b, c)) {
        mst[i].push_back(j);
      }
    }
    for (int j = 0; j < i; ++j) {
      auto [a, b, c] = edges[j];
      merge(a, b, c);
    }
    if (size(0) == n) {
      best_solution_so_far = min(best_solution_so_far, cost);
    } else {
      bridge[i] = 1;
    }
  }
  backtrack(0);
  running_time_end = ((double) (clock()) - start_clock)/CLOCKS_PER_SEC;
  printf("%d %d %d %d %.4lf %lld %lld %lld %lld %.4lf %d\n", lb_cost, ub1_cost == INF ? -1 : ub1_cost, ub2_cost == INF ? -1 : ub2_cost, best_solution_so_far, running_time, lb_count, ub1_count, ub2_count, try_count, running_time_end, best_choice.size());
  sort(best_choice.begin(), best_choice.end());
  for (auto id : best_choice) {
    printf("%d ", id);
  }
}