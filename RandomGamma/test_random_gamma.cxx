#include "RandomGamma.h"
#include <iostream>
#include <cstdlib>

using namespace std;


int
main(int argc, char* argv[])
{
  if (argc > 2)
    return 1;
  unsigned int n = 10;
  if (argc == 2)
    n = atoi(argv[1]);

  const double npe = 10;
  const double charge = 100;
  const double shape = 2;
  const double ms = charge / shape;

  for (unsigned int i = 0; i < n; ++i)
    cout << RandomGamma(npe*shape, ms) << endl;

  return 0;
}
