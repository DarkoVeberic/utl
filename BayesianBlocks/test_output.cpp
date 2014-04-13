#include "BayesianBlocks.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <cmath>

using namespace std;


inline
bool
IsClose(const double a, const double b, const double eps = 1e-10)
{
  return fabs(a - b) < eps;
}


int
main(int argc, char* argv[])
{
  if (argc != 3)
    return 1;

  vector<double> input;
  {
    fstream f(argv[1]);
    if (!f.is_open())
      return 2;
    double x;
    while (f >> x)
      input.push_back(x);
  }
  cerr << "input size: " << input.size() << endl;

  sort(input.begin(), input.end());

  const vector<double> output = BayesianBlocks(input);

  vector<double> expectedInput;
  {
    fstream f(argv[2]);
    if (!f.is_open())
      return 3;
    double b;
    while (f >> b)
      expectedInput.push_back(b);
  }

  cerr << "output sizes: " << output.size() << ' ' << expectedInput.size() << endl;

  if (output.size() != expectedInput.size())
    return 10;

  for (unsigned int i = 0, n = output.size(); i < n; ++i)
    if (!IsClose(output[i], expectedInput[i])) {
      cerr << "item " << i << ": "
              "diff too large, " << output[i] << " - " << expectedInput[i]
           << " = " << (output[i] - expectedInput[i]) << endl;
      return 11;
    }

  return 0;
}
