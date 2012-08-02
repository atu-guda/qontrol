/***************************************************************************
                          tlinp1q0.cpp  -  description
                             -------------------
    begin                : Fri Sep 1 2000
    copyright            : (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tmodel.h"
#include "tlinp1q0.h"

const char* TLinP1Q0::helpstr = "<H1>TLinP1Q0</H1>\n"
 "Integrate (linear if f(x)==x) diffefentional equation: <br>\n"
 "<b> dx/dt = a * f( ku * u(t) - x ) </b><br>\n"
 "Parameter <b>ku</b> -- amplification, can be changed at any time.<br>\n"
 "Parameter <b>a</b> -- frequency, can be changed at any time.<br>\n"
 " <b>a = 1 / tau; </B> "
 "where <B> tau </B> -- time constant.<br>\n"
 "Parametr: <b>fx</b> -- name of element, calculating f(x),"
 " if none - assumed f(x) = x. <br>\n"
 "Linear transfer function: <br>\n"
 "<B>W = ku / ( tau * p + 1 ) </B>.<br>\n"
 "If u(t) = theta(t) than x(t) = ku * ( 1 - exp(-t/tau) ), and<br>\n"
 " x(tau) = 0.6321 * ku \n"
 ;

TClassInfo TLinP1Q0::class_info = {
  CLASS_ID_TLinP1Q0, "TLinP1Q0", TLinP1Q0::create,
  &TMiso::class_info, helpstr, clpElem };


TLinP1Q0::TLinP1Q0( TDataSet* aparent )
        :TMiso( aparent )
{
  a = 1.0;  ku = 1.0; 
  use_u1 = 0;
  x_old = 0;
}

TLinP1Q0::~TLinP1Q0()
{
}

TDataSet* TLinP1Q0::create( TDataSet* apar )
{
  return new TLinP1Q0( apar );
}


const TClassInfo* TLinP1Q0::getClassInfo(void) const
{
  return &class_info;
}

const char *TLinP1Q0::getHelp(void) const
{
  return helpstr;
}

int TLinP1Q0::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  x_old = 0;
  return 0;
}

double TLinP1Q0::f( double /*t*/ )
{
  double x, f;
  if( use_u1 ) {
    f = *in_so[1];
  } else {
    f = x_old;
  };
  // TODO: check this for stability
  x = x_old + a * tdt * ( ku * *in_so[0] - f );
  x_old = x;
  return x;
}

int TLinP1Q0::registered = reg();

int TLinP1Q0::reg()
{
  return EFACT.registerElemType( &class_info  );
}


// end of tlinp1q0.cpp

