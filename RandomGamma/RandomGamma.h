#ifndef _RandomGamma_h_
#define _RandomGamma_h_


double RandomGamma(const double shape);


inline
double
RandomGamma(const double shape, const double scale)
{
  return scale * RandomGamma(shape);
}



#endif
