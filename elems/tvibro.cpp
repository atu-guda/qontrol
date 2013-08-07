/***************************************************************************
                          tvibro.cpp  -  description
                             -------------------
    begin                : Mon Aug 28 2000
    copyright            : (C) 2000-2013 by atu
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

#include "tvibro.h"

const char* TVibro::helpstr = "<H1>TVibro</H1>\n"
 "Vibrational element <b> d2x/dt^2 + c0*dx/dt + Omega^2*f(x) = u</b>: <br>\n"
 "Parameters <b>c0</b> and <b>Omega</b> can be changed at any time.<br>\n";

TClassInfo TVibro::class_info = {
  "TVibro", TVibro::create,
  &TMiso::class_info, helpstr, clpElem };


TVibro::TVibro( TDataSet* aparent )
        :TMiso( aparent ),
	 c0(0.4), Omega(1.2), v0(0), use_u1(0), v(v0), isStart(1),
	 u_old(0), f_old(0), x_old(out0_init), x_old2(out0_init), tdt2(1)
{
}

TVibro::~TVibro()
{
}

TDataSet* TVibro::create( TDataSet* apar )
{
  return new TVibro( apar );
}

const TClassInfo* TVibro::getClassInfo(void) const
{
  return &class_info;
}

const char *TVibro::getHelp(void) const
{
  return helpstr;
}

int TVibro::do_preRun( int /*run_tp*/, int /*an*/, 
                       int /*anx*/, int /*any*/, double /*adt*/ )
{
  tdt2 = tdt * tdt;
  return 0;
}

int TVibro::do_startLoop( int /*acnx*/, int /*acny*/ )
{
  u_old = f_old = 0;
  v = v0;
  x_old = out0_init;
  x_old2 = out0_init - v0 * tdt;
  isStart = 1;
  return 0;
}

double TVibro::f( double /*t*/ )
{
  double x, ctau = tdt * c0 / 2; // c0 can be changed at any time, so here
  
  if( isStart == 1 ) {  // first step
    isStart = 2; x = out0_init; v = v0;
  } else if ( isStart == 2 ) {  // second step
    isStart = 0; 
    x = x_old + v0 * tdt + 0.5 * tdt2 * u_old;
    v = v0 + u_old * tdt;
  } else {  // all other steps
    x = ( 2*x_old - x_old2 * (1-ctau) + tdt2 * (u_old - Omega * Omega * f_old) ) 
      / ( 1 + ctau );
    v = ( x - x_old2 ) / ( 2 * tdt );
  }
  
  f_old = use_u1 ? *in_so[1] : x;
  
  u_old = *in_so[0]; 
  x_old2 = x_old; x_old = x;
  return x;
}

int TVibro::registered = reg();

int TVibro::reg()
{
  return EFACT.registerElemType( &class_info  );
}

// end of tvibro.cpp

 
