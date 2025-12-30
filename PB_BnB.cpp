#include <bits/stdc++.h>

using namespace std;

#ifdef LOCAL
#include "C:\debug.h"
#else
#define debug(...) 42
#endif

bool use_lower_bound, use_upper_bound, use_ub2;
const int INF = 1e9;
const int threshold = 60 * 60;
int start_clock;

int n, m;
long long try_count, lb_count, ub1_count, ub2_count;
int lb_cost = -1, ub1_cost = -1, ub2_cost = -1;
double running_time, running_time_end;
vector<int> pr, weight, is_leaf, dist, deg, start, best_choice, trace_cost, trace_ending_cost;
vector<pair<int, int>> adj;
vector<vector<array<int, 3>>> nxt;
vector<array<int, 3>> nearest, nearest_inner;
vector<array<int, 4>> edges;

inline int find(int u) {
  return pr[u] < 0 ? u : pr[u] = find(pr[u]);
}

inline bool connected(int u, int v) {
  return find(u) == find(v);
}

inline bool merge(int u, int v) {
  if ((u = find(u)) == (v = find(v))) {
    return 0;
  }
  if (pr[u] > pr[v]) {
    swap(u, v);
  }
  pr[u] += pr[v];
  pr[v] = u;
  return 1;
}

inline int size(int u) {
  return -pr[find(u)];
}

inline void clear() {
  for (int i = 0; i < n; ++i) {
    pr[i] = -1;
    deg[i] = 0;
    dist[i] = is_leaf[i] == -1 ? INF : 0;
    nearest[i] = nearest_inner[i] = {INF, -1, -1};
  }
}

inline int get_next(int it) {
  int u = adj[it].first;
  while (it < 2 * m && adj[it].first == u && dist[adj[it].second] != INF) {
    ++it;
  } 
  return it < 2 * m && adj[it].first == u ? it : -1;
}

inline int dijkstra() {
  priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> q;
  for (int i = 0; i < n; ++i) {
    if (!is_leaf[i]) {
      int p = get_next(start[i]);
      if (~p) {
        q.push({weight[adj[p].second], p});
      }
    }
  }
  auto add = [&](int it) {
    if (it == -1) {
      return;
    }
    auto [u, v] = adj[it];
    q.push({dist[u] + weight[v], it});
  };
  int res = 0;
  while (q.size()) {
    auto [c, id] = q.top();
    q.pop();
    auto [u, v] = adj[id];
    if (dist[v] != INF) {
      continue;
    }
    dist[v] = c;
    res = max(res, dist[v] - weight[v]);
    add(get_next(start[v]));
    add(get_next(id));
  } 
  return res;
}

inline int calculate_mst_and_nearest_arrays() {
  clear();
  int cost = 0; 
  vector<int>().swap(trace_cost); // clear vector trace_cost
  for (auto [w, u, v, id] : edges) {
    if (is_leaf[u] < is_leaf[v]) { // reduce the number of cases that need to be examined
      swap(u, v);
    }
    if (is_leaf[u] == 1 && is_leaf[v] == 1) { // {1, 1}
      continue;
    }
    if (is_leaf[u] == 1) {
      nearest[u] = min({nearest[u], {w, v, id}});
      if (is_leaf[v] == 0) {
        nearest_inner[u] = min(nearest_inner[u], {w, v, id});
      }
      continue;
    }
    if (merge(u, v)) {
      // cạnh (u, v) thuộc cây T trên
      trace_cost.push_back(id);
      cost += w;
      ++deg[u];
      ++deg[v];
    }
    if (is_leaf[u] == -1) { // {-1, -1}
      nearest[u] = min(nearest[u], {w, v, id});
      nearest[v] = min(nearest[v], {w, u, id});
      continue;
    }
    if (is_leaf[v] == -1) { // {0, -1}
      nearest_inner[v] = min(nearest_inner[v], {w, u, id});
      nearest[v] = min(nearest[v], {w, u, id});
    }
  }
  return cost;
}

int current_cost, best_solution_so_far = INF;

inline void backtrack(int u, int cnt) {
  if (((double) (clock()) - start_clock)/CLOCKS_PER_SEC > threshold) {
    return;
  }
  ++try_count;
  int cost = calculate_mst_and_nearest_arrays();
  int cnt_leaf = count(is_leaf.begin(), is_leaf.end(), 1); // đếm số nút lá
  if ((u != n) && (size(u) != n - cnt_leaf)) {
    return;
  }
  for (int i = 0; i < n; ++i) {
    if (is_leaf[i] == 1 && nearest[i][0] == INF) {
      return;
    }
  }
  int cost_if_end = current_cost, future_edge_cost = 0;
  int sz = trace_ending_cost.size();
  for (int i = 0; i < n; ++i) {
    if (is_leaf[i] == 1) {
      ++deg[nearest[i][1]];
      cost_if_end = min(INF, cost_if_end + nearest_inner[i][0]);
      cost += nearest[i][0];
      future_edge_cost += nearest[i][0];
      trace_ending_cost.push_back(nearest_inner[i][2]);
      trace_cost.push_back(nearest[i][2]);
    } else if (is_leaf[i] == -1) {
      cost_if_end = min(INF, cost_if_end + nearest_inner[i][0]);
      future_edge_cost += nearest[i][0]; 
      trace_ending_cost.push_back({nearest_inner[i][2]});
    }
  }
  for (int i = 0; i < n; ++i) {
    if (deg[i] > 1) {
      cost += weight[i];
    }
  }
  int future_node_cost = dijkstra();
  int lower_bound = current_cost + future_node_cost + future_edge_cost;
  if (use_upper_bound && best_solution_so_far > cost) {
    ++ub1_count;
    best_solution_so_far = cost;
    lb_cost = lower_bound;
    ub1_cost = cost;
    ub2_cost = cost_if_end;
    running_time = ((double) (clock()) - start_clock)/CLOCKS_PER_SEC;
    best_choice = trace_cost;
  }
  if (use_ub2 && best_solution_so_far > cost_if_end) {
    ++ub2_count;
    best_solution_so_far = cost_if_end;
    lb_cost = lower_bound;
    ub1_cost = cost;
    ub2_cost = cost_if_end;
    running_time = ((double) (clock()) - start_clock)/CLOCKS_PER_SEC;
    best_choice = trace_ending_cost;
  }
  while (int(trace_ending_cost.size()) > sz) {
    trace_ending_cost.pop_back();
  }
  if (use_lower_bound && lower_bound >= best_solution_so_far) {
    ++lb_count;
    return;
  }
  if (cnt == n) {
    if (best_solution_so_far > cost) {
      best_solution_so_far = cost;
      lb_cost = lower_bound;
      ub1_cost = cost;
      ub2_cost = cost_if_end;
      running_time = ((double) (clock()) - start_clock)/CLOCKS_PER_SEC;
      best_choice = trace_cost;
    }
    return;
  }
  vector<array<int, 3>>().swap(nxt[u]);
  for (int i = 0; i < n; ++i) {
    if (is_leaf[i] == -1) {
      nxt[u].push_back({nearest_inner[i][0], nearest_inner[i][2], i});
    }
  }
  sort(nxt[u].begin(), nxt[u].end());
  for (auto [w, id, v] : nxt[u]) { 
    ++cnt;
    if (w != INF) {
      is_leaf[v] = 0;
      current_cost += weight[v] + w;
      trace_ending_cost.push_back(id); 
      backtrack(v, cnt);
      current_cost -= weight[v] + w;
      trace_ending_cost.pop_back();
    }
    is_leaf[v] = 1;
  }
  backtrack(n, cnt);
  for (auto [w, id, v] : nxt[u]) {
    is_leaf[v] = -1;
  }
}

int main() {
  ios::sync_with_stdio(false); 
  cin.tie(nullptr);
  ifstream in("test_info");
  in >> use_lower_bound >> use_upper_bound >> use_ub2;
  in.close();
  scanf("%d%d", &n, &m);
  edges.reserve(m);
  weight.reserve(n);
  nxt.resize(n);
  for (int i = 0; i < m; ++i) {
    int w, u, v;
    scanf("%d;%d;%d", &u, &v, &w);
    --u, --v;
    edges.push_back({w, u, v, i});
    adj.push_back({u, v});
    adj.push_back({v, u});
  }
  for (int i = 0; i < n; ++i) {
    int tmp, x;
    scanf("%d%d", &tmp, &x);
    weight.push_back(x);
  }
  pr = vector<int>(n, -1);
  is_leaf = vector<int>(n, -1);
  nearest_inner = nearest = vector<array<int, 3>>(n, {INF, -1, -1});
  dist = vector<int>(n, INF);
  deg = vector<int>(n, 0);
  start = vector<int>(n, -1);
  trace_cost.reserve(n);
  trace_ending_cost.reserve(n);
  sort(edges.begin(), edges.end());
  sort(adj.begin(), adj.end(), [&](auto i, auto j) {
    return i.first == j.first ? weight[i.second] < weight[j.second] : i.first < j.first;
  });
  for (int i = 0; i < 2 * m; ++i) {
    if (start[adj[i].first] == -1) {
      start[adj[i].first] = i;
    }
  }
  start_clock = clock();
  for (int i = 0; i < n; ++i) {
    is_leaf[i] = 0;
    current_cost += weight[i];
    backtrack(i, i + 1);
    if (((double) (clock()) - start_clock)/CLOCKS_PER_SEC > threshold) {
      break;
    }
    current_cost -= weight[i];
    is_leaf[i] = 1;
  }
  running_time_end = ((double) (clock()) - start_clock)/CLOCKS_PER_SEC;
  printf("%d %d %d %d %.4lf %lld %lld %lld %lld %.4lf\n", lb_cost, ub1_cost == INF ? -1 : ub1_cost, ub2_cost == INF ? -1 : ub2_cost, best_solution_so_far, running_time, lb_count, ub1_count, ub2_count, try_count, running_time_end);
  sort(best_choice.begin(), best_choice.end());
  for (auto id : best_choice) {
    printf("%d ", id + 1);
  }
}