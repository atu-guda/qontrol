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
  double v;
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
    case ft_Griewangk:
    case ft_HyperEllipsoid:
    case ft_ParaEllip:
    case ft_Rastrigin:
    case ft_RastriginNovg:
    case ft_Rosenbrock:
    case ft_Schwefel:
    case ft_Step:
    case ft_PotentialAdd:
    case ft_Bosom:
    case ft_EggHolder:
    case ft_Himmelblau:
    case ft_Kentikov:
    case ft_PotentialMult:
    case ft_Rana:
    case ft_ShekelsFoxholes:
    case ft_Sombrero:
    case ft_Multi1:
    case ft_Multi2:
    case ft_Wave:
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

DEFAULT_FUNCS_REG(TExtrTest)


// end of textrtest.cpp

