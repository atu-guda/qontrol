/***************************************************************************
                          tvibro.cpp  -  description
                             -------------------
    begin                : Mon Aug 28 2000
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

#include "tvibro.h"

const char* TVibro::helpstr = "<H1>TVibro</H1>\n"
 "Vibrational element <b> d2x/dt^2 + c0*dx/dt + Omega^2*f(x) = u</b>: <br>\n"
 "Parameters <b>c0</b> and <b>Omega</b> can be changed at any time.<br>\n"
 "<b>rfe</b> - NONE! name of element, which must input <b>x</b> as parm[0]. <br>\n"
 "If output of element depend of it's state, it must be flagged as '<b>noauto</b>'" 
 "If this name empty or bad, assumed <b>f(x) = x</b>.";

TClassInfo TVibro::class_info = {
  CLASS_ID_TVibro, "TVibro", TVibro::create,
  &TMiso::class_info, helpstr, clpElem };


TVibro::TVibro( TDataSet* aparent )
        :TMiso( aparent )
{
  Omega = 1.2; c0 = 0.4; 
  use_u1 = 0;
  isStart = 1; tdt2 = 1; u_old = f_old = x_old = x_old2 = 0;
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
  x_old = x_old2 = u_old = f_old = 0;
  isStart = 1;
  return 0;
}

double TVibro::f( double /*t*/ )
{
  double x, ctau;
  if( isStart == 1 ) {  // first step
    isStart = 2; x = 0;
    if( use_u1 ) {
      f_old = *in_so[1];
    } else {
      f_old = x;
    };
    u_old = *in_so[0];
    return x;
  };
  if( isStart == 2 ) {  // second step
    isStart = 0; x = x_old + 0.5 * tdt2 * u_old;
    if( use_u1 ) {
      f_old = *in_so[1];
    } else {
      f_old = x;
    };
    u_old = *in_so[0]; x_old2 = x_old; x_old = x;
    return x;
  };
  // all other steps
  ctau = tdt * c0 / 2; // c0 can be changed at any time, so here
  x = ( 2*x_old - x_old2 * (1-ctau) + tdt2 * (u_old - Omega * Omega *f_old) ) 
    / ( 1 + ctau );
  if( use_u1 ) {
    f_old = *in_so[1];
  } else {
    f_old = x;
  };
  u_old = *in_so[0]; x_old2 = x_old; x_old = x;
  return x;
}

int TVibro::registered = reg();

int TVibro::reg()
{
  return EFACT.registerElemType( &class_info  );
}

// end of tvibro.cpp

 
