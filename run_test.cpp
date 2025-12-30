#include <bits/stdc++.h>

using namespace std;

#ifdef LOCAL
#include "C:\debug.h"
#else
#define debug(...) 42
#endif

int main() {
  ios::sync_with_stdio(false);
  cin.tie(0);
  vector<array<int, 3>> config{{1, 1, 1}, {0, 0, 0}, {1, 1, 0}, {1, 0, 1}};
  cout << fixed << setprecision(4);
  int it = 1;
  //cin >> it;
  string data = "output_KB_BnB_";
  data += to_string(it);
  ofstream out(data.c_str());
  debug(data);
  out << "#instances\n";
  out << "#lb_cost ub1_cost ub2_cost best_solution_so_far running_time lb_count ub1_count ub2_count try_count running_time_end\n";
  out << "#best_choice\n";
  auto [x, y, z] = config[it];
  ofstream inn("test_info");
  inn << x << " " << y << " " << z;
  inn.close();
  for (int N : {10, 20, 30, 40, 50, 60, 70, 80, 90, 100}) {
    for (double P : {0.1, 0.15, 0.25, 0.55, 0.75, 0.85, 0.95}) {
      for (auto [M1, M2] : vector<pair<int, int>>{{75, 150},{150, 75}, {150, 150}}) { 
        ostringstream oss;
        oss << fixed << setprecision(2) << P;
        string pp = oss.str();  
        string input = "N_" + to_string(N) + "_P_" + pp + "_N_" + to_string(M2) + "_20_M_" + to_string(M1) + "_20";
        string cmd = "KB_BnB < " + input + " > tmp_out";
        debug(cmd);
        system(cmd.c_str());
        ifstream in("tmp_out");
        int lb_cost, ub1_cost, ub2_cost, best_solution_so_far, best_choice_size;
        long long lb_count, ub1_count, ub2_count, try_count;
        double running_time, running_time_end;
        out << input << "\n";
        in >> lb_cost >> ub1_cost >> ub2_cost >> best_solution_so_far >> running_time >> lb_count >> ub1_count >> ub2_count >> try_count >> running_time_end >> best_choice_size;
        out << lb_cost << " " << ub1_cost << " " << ub2_cost << " " << best_solution_so_far << " " << running_time << " " << lb_count << " " << ub1_count << " " << ub2_count << " " << try_count << " " << running_time_end << " " << best_choice_size << "\n";
        for (int i = 0; i < best_choice_size; ++i) {
          int id;
          in >> id;
          out << id << " ";
        }
        in.close();
        out << "\n";
      }
    }
  }
  out.close();
}