#ifndef _RAND_GEN
#define _RAND_GEN

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

/** Class to embed gsl random generator (standalone)
 * for more info see info:gsl-ref */

class RandGenerator {
  public:
   RandGenerator() = default;
   ~RandGenerator() { gsl_rng_free( rng );  rng = nullptr; };
   void set( unsigned long s ) { gsl_rng_set( rng, s ); };
   unsigned long min() const { return gsl_rng_min( rng ); }
   unsigned long max() const { return gsl_rng_max( rng ); }
   unsigned long get() { return gsl_rng_get( rng ); }
   // distributions
   double beta( double a, double b ) { return gsl_ran_beta( rng, a, b ); }
   double cauchy( double a ) { return gsl_ran_cauchy( rng, a ); }
   double chisq( double nu ) { return gsl_ran_chisq( rng, nu ); }
   double erlang( double a, double n ) { return gsl_ran_erlang( rng, a, n ); }
   double exponential( double mu ) { return gsl_ran_exponential( rng, mu ); }
   double exppow( double a, double b ) { return gsl_ran_exppow( rng, a, b ); }
   double fdist( double nu1, double nu2 ) { return gsl_ran_fdist( rng, nu1, nu2 ); }
   double flat( double a, double b ) { return gsl_ran_flat( rng, a, b ); }
   double gamma( double a, double b ) { return gsl_ran_gamma( rng, a, b ); }
   double gaussian( double sigma ) { return gsl_ran_gaussian( rng, sigma ); }
   double gaussian_tail( double a, double sigma ) { return gsl_ran_gaussian_tail( rng, a, sigma ); }
   double gumbel1( double a, double b ) { return gsl_ran_gumbel1( rng, a, b ); }
   double gumbel2( double a, double b ) { return gsl_ran_gumbel2( rng, a, b ); }
   double landau() { return gsl_ran_landau( rng ); }
   double laplace( double a ) { return gsl_ran_laplace( rng, a ); }
   double levy( double c, double alpha ) { return gsl_ran_levy( rng, c, alpha ); }
   double levy_skew( double c, double alpha, double beta ) { return gsl_ran_levy_skew( rng, c, alpha, beta ); }
   double logistic( double a ) { return gsl_ran_logistic( rng, a ); }
   double lognormal( double zeta, double sigma ) { return gsl_ran_lognormal( rng, zeta, sigma ); }
   double pareto( double a, double b ) { return gsl_ran_pareto( rng, a, b ); }
   double rayleigh( double sigma ) { return gsl_ran_rayleigh( rng, sigma ); }
   double rayleigh_tail( double a, double sigma ) { return gsl_ran_rayleigh_tail( rng, a, sigma ); }
   double tdist( double nu ) { return gsl_ran_tdist( rng, nu ); }
   double weibull( double a, double b ) { return gsl_ran_weibull( rng, a, b ); }
   unsigned bernoulli( double p ) { return gsl_ran_bernoulli( rng, p ); }
   unsigned binomial( double p, unsigned n ) { return gsl_ran_binomial( rng, p, n ); }
   unsigned geometric( double p ) { return gsl_ran_geometric( rng, p ); }
   unsigned hypergeometric( unsigned n1, unsigned n2, unsigned t ) { return gsl_ran_hypergeometric( rng,  n1, n2, t ); }
   unsigned logarithmic( double p ) { return gsl_ran_logarithmic( rng, p ); }
   unsigned negative_binomial( double p, double n ) { return gsl_ran_negative_binomial( rng, p, n ); }
   unsigned pascal( double p, unsigned n ) { return gsl_ran_pascal( rng, p, n ); }
   unsigned poisson( double mu ) { return gsl_ran_poisson( rng, mu ); }
  protected:
   gsl_rng *rng = gsl_rng_alloc ( env_tp );
   static const gsl_rng_type *env_tp;
};

#endif
