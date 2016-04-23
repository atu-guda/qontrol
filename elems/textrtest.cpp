/***************************************************************************
   textrtest.cpp - testremum test functions definitions
                             -------------------
    begin                : 2016.04.22
    copyright            : (C) 2016-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cmath>
#include "miscfun.h"
#include "textrtest.h"

using namespace std;

static double z_PotentialAdd( double v )
{
  return  -1.0 / (     pow2( v - 1 ) + 0.20 )
          -1.0 / ( 2 * pow2( v - 2 ) + 0.15 )
          -1.0 / ( 3 * pow2( v - 3 ) + 0.30 );
}

static double z1_Bossom( double x0, double x1 )
{
  return -pow2( pow2( x0 + 4 ) + pow2( x1 + 4 ) );
}

static double z2_Bossom( double x0, double x1 )
{
  return -pow2( pow2( x0 - 4 ) + pow2( x1 - 4 ) );
}

const char* TExtrTest::helpstr = "<h1>TExtrTest</h1>\n"
 "Special functions to test extremum searching algorithms<br/>\n"
 "See: Harrix-TestFunctions <br/>\n"
 "Inputs: in_0, in_1 <br/>\n";


STD_CLASSINFO(TExtrTest,clpElem|clpCalcAtStart);

CTOR(TExtrTest,TMiso)
{
}


double TExtrTest::f() noexcept
{
  double v, z1, z2;
  x0 = ( in_0 - x0_0 ) * c0;
  x1 = ( in_1 - x1_0 ) * c1;
  x02 = x0 * x0;
  x12 = x1 * x1;
  xc = x02 + x12;

  switch( (FuncType)(int)type ) {
    case ft_Ackley:
      v = 20.0 + M_E
          - p0 * 20 * exp( -0.2*sqrt( 0.5 * p0 *  xc ) )
          - p1 * exp(0.5 * ( cos( 2 * M_PI * x0 ) + cos( 2 * M_PI * x1 )) );
      break;
    case ft_Quartic:
      v = pow2( x02 ) + pow2( x12 );
      break;
    case ft_Griewangk:
      v = p0 * xc / 4000.0 - cos( x0 ) * cos( x1 / M_SQRT2 ) + 1.0;
      break;
    case ft_HyperEllipsoid:
      v = x02 + 2 * x12;
      break;
    case ft_ParaEllip:
      v = x02 + x12;
      break;
    case ft_Rastrigin:
      v = 20.0 + ( x02  - 10 * p0 * cos( 2*M_PI * x0 ))
               + ( x12  - 10 * p1 * cos( 2*M_PI * x1 ));
      break;
    case ft_RastriginNovg:
      v = 2.0  + ( x02  -  p0 * 5.0 * cos( 0.2 * 2*M_PI * x02 ))
               + ( x12  -  p1 * 5.0 * cos( 0.2 * 2*M_PI * x12 ));
      break;
    case ft_Rosenbrock:
      v =  p0 * pow2( 1 - x0 ) + 100.0 * p1 * pow2( x1 - x02 );
      break;
    case ft_Schwefel:
      v = 2 * 418.9829
        - p0 * x0 * sinqa( x0 )
        - p1 * x1 * sinqa( x1 );
      break;
    case ft_Step:
      if( ( abs(int(x0)) +abs(int(x1)) ) != 0 ) {
        v = p0 * pow2( int(x0) ) + p1 * pow2( int(x1) );
      } else {
        v = fabs(x0) + fabs(x1) - 1.0;
      }
      break;
    case ft_PotentialAdd:
      v = p0 * z_PotentialAdd( x0 ) + p1 * z_PotentialAdd( x1 );
      break;
    case ft_Bosom:
      z1 = z1_Bossom( x0, x1 );
      z2 = z2_Bossom( x0, x1 );
      v = p0 * ( exp( z1 / 1000 ) +        exp( z2 / 1000 ) )
        + p1 * ( 0.15 * exp( z1 ) + 0.15 * exp( z2 )        );
      break;
    case ft_EggHolder:
      v = -x0     * p0 * sinqa(      x0 - 47 - x1 )
        - (x1+47) * p1 * sinqa(  0.5*x0 + 47 + x1 );
      break;
    case ft_Himmelblau:
      v = p0 * pow2( x02 + x1 - 11 ) + p1 * pow2( x0 + x12 - 7 );
      break;
    case ft_Kantikov:
      v = 0.5 * 0.8 * xc * (
            2 + p0 * cos( 1.5         * x0 ) * cos( M_PI * x1 )
              + p1 * cos( 2.236067977 * x0 ) * cos( 3.5  * x1 ) // 2.23.. = sqrt(5)
          );
      break;
    case ft_PotentialMult:
      v = - z_PotentialAdd( x0 ) * z_PotentialAdd( x1 );
      break;
    case ft_Rana:
      z1 = sqrtabs( x1 + 1 - x0 );
      z2 = sqrtabs( x1 + 1 + x0 );
      v = p0 * x0 * sin( z1 ) * cos( z2 )
        + p1 * ( x1 +1 ) * cos( z1 ) * sin( z2 );
      break;
    case ft_ShekelsFoxholes:
      z1 = 0.002;
      for( int i=0; i<n_foxhole; ++i ) {
        z1 += 1.0 / (
              (i+1) + pow6( x0 - p0 * foxhole_xy[i][0] ) + p1 * pow6( x1 - p1 * foxhole_xy[i][1] )
            );
      }
      v = 1.0 / z1;
      break;
    case ft_Sombrero:
      v = ( 1 - pow2( sin( sqrt( xc )) ) )
        /
        ( 1 + 0.001 * p0 * xc );
      break;
    case ft_Multi1:
      v = p0 * pow2( x0-1 )
        +   ( 3.0 - 2.9 * exp( -2.77267 * x02 ) )
          * ( 1 - cos( x0 * ( 4 - 50 * exp(-2.77257 * x02 ) ) ) )
        + p1 * 0.1 * x12;
      break;
    case ft_Multi2:
      v = 1 - p0 * cos( 15 * x0 - 0.45 ) * cos( 10 * M_PI * x0 )
        + 0.5 * cos( 22.36067977 * x0 ) * cos( 35*x0 )
        + p1 * 0.1 * x12;
      break;
    case ft_Wave:
      v = p0 * exp( -xc ) + p1 * 0.01 * cos( 200 * ( x0 + x1 ) );
      break;
    case ft_Easom:
      v = - cos( x0 ) * cos( x1 ) * exp( -p0 * pow2( x0 - M_PI ) - p1 * pow2( x1 - M_PI ));
      break;
    default: v = 0;
  };
  v *= scale;
  v += shift;
  return v;
}

double TExtrTest::f_d( double arg0, double arg1, double /*arg2*/, double /*arg3*/ )
{
  in_0.setInput( arg0 );
  in_1.setInput( arg1 );
  return f();
}

const int TExtrTest::foxhole_xy[n_foxhole][2]
{
  { -32, -32 },
  { -16, -32 },
  {   0, -32 },
  {  16, -32 },
  {  32, -32 },
  { -32, -16 },
  { -16, -16 },
  {   0, -16 },
  {  16, -16 },
  {  32, -16 },
  { -32,   0 },
  { -16,   0 },
  {   0,   0 },
  {  16,   0 },
  {  32,   0 },
  { -32,  16 },
  { -16,  16 },
  {   0,  16 },
  {  16,  16 },
  {  32,  16 },
  { -32,  32 },
  { -16,  32 },
  {   0,  32 },
  {  16,  32 },
  {  32,  32 }
};

DEFAULT_FUNCS_REG(TExtrTest)


// end of textrtest.cpp

