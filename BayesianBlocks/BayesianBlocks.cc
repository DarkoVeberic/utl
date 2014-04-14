#include "BayesianBlocks.h"
#include <cmath>

using namespace std;


// Darko Veberic. implementation according to the python code from
// http://jakevdp.github.io/blog/2012/09/12/dynamic-programming-in-python/

vector<double>
BayesianBlocks(const vector<double>& t /*sorted ascending*/)
{
  const unsigned int n = t.size();
  vector<double> edges;
  edges.reserve(n);
  edges.push_back(t.front());
  for (unsigned int i = 1; i < n; ++i)
    edges.push_back(0.5*(t[i] + t[i-1]));
  edges.push_back(t.back());
  vector<double> block_length;
  block_length.reserve(n);
  for (unsigned int i = 0; i < n; ++i)
    block_length.push_back(t.back() - edges[i]);
  vector<double> nn_vec(n, 1);
  vector<double> best(n);
  vector<unsigned int> last(n);
  vector<double> width;
  width.reserve(n);
  vector<double> count_vec;
  count_vec.reserve(n);
  vector<double> fit_vec;
  fit_vec.reserve(n);
  for (unsigned int k = 0; k < n; ++k) {
    width.clear();
    for (unsigned int i = 0; i <= k; ++i)
      width.push_back(block_length[i] - block_length[k+1]);
    count_vec.resize(k+1);
    {
      double sum = 0;
      for (unsigned int i = 0; i <= k; ++i) {
        sum += nn_vec[k-i];
        count_vec[k-i] = sum;
      }
    }
    fit_vec.clear();
    fit_vec.push_back(count_vec.front() * log(count_vec.front() / width.front()) - 4);
    for (unsigned int i = 1; i <= k; ++i)
      fit_vec.push_back(count_vec[i] * log(count_vec[i] / width[i]) - 4 + best[i-1]);
    {
      unsigned int i_max = 0;
      double max = fit_vec.front();
      for (unsigned int i = 1; i <= k; ++i)
        if (fit_vec[i] > max) {
          max = fit_vec[i];
          i_max = i;
        }
      last[k] = i_max;
      best[k] = max;
    }
  }
  /*vector<unsigned int> change_points;
  change_points.reserve(n);
  for (unsigned int index = last[n-1]; index > 0; index = last[index-1])
    change_points.push_back(index);
  vector<double> ret;
  ret.reserve(change_points.size());
  for (int i = change_points.size()-1; i >= 0; --i)
    ret.push_back(edges[change_points[i]]);*/
  vector<unsigned int> change_points(n);
  unsigned int i_cp = n;
  unsigned int ind = n;
  while (true) {
    --i_cp;
    change_points[i_cp] = ind;
    if (!ind)
      break;
    ind = last[ind-1];
  }
  vector<double> ret;
  ret.reserve(n - i_cp);
  for (unsigned int i = i_cp, m = change_points.size(); i < m; ++i)
    ret.push_back(edges[change_points[i]]);
  return ret;
}
