#include "RandomGamma.h"
#include <TRandom3.h>
#include <cmath>

using namespace std;

/*
  GPLv2 and 2C-BSD
  Copyright (c) Darko Veberic, 2014

  This code generates Gamma variate in ROOT environment (using gRandom).
  The solution is based on:
  G. Marsaglia and W.W. Tsang, "Simple Method for Generating Gamma Variables",
  ACM Trans. Math. Soft. 26 (2000) 363-372.

*/

double
RandomGamma(const double shape)
{
  if (shape < 1)
    return RandomGamma(1 + shape) * pow(gRandom->Rndm(), 1/shape);

  const double d = shape - 1/3.;
  const double c = (1/3.) / sqrt(d);

  double v = 0;

  for (;;) {
    double x = 0;
    do {
      x = gRandom->Gaus();
      v = 1 + c * x;
    } while (v <= 0);

    v = v * v * v;
    x = x * x;
    const double u = gRandom->Rndm();

    if (u < 1 - 0.0331 * x * x ||
        log(u) < 0.5*x + d*(1 - v + log(v)))
      return d * v;
  }
}
